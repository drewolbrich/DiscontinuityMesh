// Copyright 2009 Retroactive Fiasco.

#include "BoxCreator.h"

#include <cassert>
#include <cmath>
#include <vector>

#include <cgmath/Constants.h>
#include <cgmath/Matrix4f.h>
#include <mesh/Mesh.h>
#include <mesh/StandardAttributes.h>
#include <mesh/Types.h>
#include <mesh/Vertex.h>

namespace meshprim {

BoxCreator::BoxCreator()
    : mMesh(NULL),
      mTransformationMatrix(cgmath::Matrix4f::IDENTITY),
      mWidth(1.0),
      mHeight(1.0),
      mDepth(1.0),
      mFaceAttributes(),
      mShouldAssignNormals(false),
      mShouldAssignTextureCoordinates(false)
{
}

BoxCreator::~BoxCreator()
{
}

void
BoxCreator::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
BoxCreator::mesh() const
{
    return mMesh;
}

void
BoxCreator::setTransformationMatrix(const cgmath::Matrix4f &transformationMatrix)
{
    mTransformationMatrix = transformationMatrix;
}

const cgmath::Matrix4f &
BoxCreator::transformationMatrix() const
{
    return mTransformationMatrix;
}

void
BoxCreator::setWidth(float width)
{
    mWidth = width;
}

float
BoxCreator::width() const
{
    return mWidth;
}

void
BoxCreator::setHeight(float height)
{
    mHeight = height;
}

float
BoxCreator::height() const
{
    return mHeight;
}

void
BoxCreator::setDepth(float depth)
{
    mDepth = depth;
}

float
BoxCreator::depth() const
{
    return mDepth;
}

void
BoxCreator::setFaceAttributes(const mesh::AttributePossessor &faceAttributes)
{
    mFaceAttributes = faceAttributes;
}

const mesh::AttributePossessor &
BoxCreator::faceAttributes() const
{
    return mFaceAttributes;
}

void
BoxCreator::setShouldAssignNormals(bool shouldAssignNormals)
{
    mShouldAssignNormals = shouldAssignNormals;
}

bool
BoxCreator::shouldAssignNormals() const
{
    return mShouldAssignNormals;
}

void
BoxCreator::setShouldAssignTextureCoordinates(bool shouldAssignTextureCoordinates)
{
    mShouldAssignTextureCoordinates = shouldAssignTextureCoordinates;
}

bool
BoxCreator::shouldAssignTextureCoordinates() const
{
    return mShouldAssignTextureCoordinates;
}

void
BoxCreator::create()
{
    assert(mMesh != NULL);

    cgmath::Vector3f size(mWidth, mHeight, mDepth);
    
    // Create vertices.
    mesh::VertexPtr vertexPtrArray[2][2][2];
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                mesh::VertexPtr vertexPtr = mMesh->createVertex();
                vertexPtrArray[i][j][k] = vertexPtr;
                vertexPtr->setPosition(mTransformationMatrix
                    *(0.5*size*cgmath::Vector3f(i*2 - 1, j*2 - 1, k*2 - 1)));
            }
        }
    }

    // Create edges.
    mesh::EdgePtr edgePtrArray[3][2][2];
    for (unsigned axis = 0; axis < 3; ++axis) {
        for (unsigned i = 0; i < 2; ++i) {
            for (unsigned j = 0; j < 2; ++j) {
                mesh::EdgePtr edgePtr = mMesh->createEdge();
                edgePtrArray[axis][i][j] = edgePtr;
                mesh::VertexPtr v0;
                mesh::VertexPtr v1;
                switch (axis) {
                case 0:
                    v0 = vertexPtrArray[0][i][j];
                    v1 = vertexPtrArray[1][i][j];
                    break;
                case 1:
                    v0 = vertexPtrArray[j][0][i];
                    v1 = vertexPtrArray[j][1][i];
                    break;
                case 2:
                    v0 = vertexPtrArray[i][j][0];
                    v1 = vertexPtrArray[i][j][1];
                    break;
                }
                edgePtr->addAdjacentVertex(v0);
                edgePtr->addAdjacentVertex(v1);
                v0->addAdjacentEdge(edgePtr);
                v1->addAdjacentEdge(edgePtr);
            }
        }
    }

    mesh::AttributeKey texCoord2fAttributeKey = mesh::GetTexCoord2fAttributeKey(*mMesh);
    mesh::AttributeKey normal3fAttributeKey = mesh::GetNormal3fAttributeKey(*mMesh);

    cgmath::Matrix4f normalMatrix(cgmath::Matrix3f(mTransformationMatrix).inverse().transpose());

    // Create faces.
    for (unsigned axis = 0; axis < 3; ++axis) {
        for (unsigned side = 0; side < 2; ++side) {
            mesh::FacePtr facePtr = mMesh->createFace();

            // Assign the face's default attributes.
            facePtr->copyAttributes(mFaceAttributes);

            // Add adjacent vertices. These must be in clockwise order.
            mesh::VertexPtr vertexPtrArrayForFace[2][2];
            for (unsigned i = 0; i < 2; ++i) {
                for (unsigned j = 0; j < 2; ++j) {
                    switch (axis) {
                    case 0:
                        vertexPtrArrayForFace[i][j] = vertexPtrArray[side][i][j];
                        break;
                    case 1:
                        vertexPtrArrayForFace[i][j] = vertexPtrArray[j][side][i];
                        break;
                    case 2:
                        vertexPtrArrayForFace[i][j] = vertexPtrArray[i][j][side];
                        break;
                    }
                }
            }
            if (side == 0) {
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[0][0]);
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[0][1]);
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[1][1]);
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[1][0]);
            } else {
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[0][0]);
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[1][0]);
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[1][1]);
                facePtr->addAdjacentVertex(vertexPtrArrayForFace[0][1]);
            }
            for (unsigned i = 0; i < 2; ++i) {
                for (unsigned j = 0; j < 2; ++j) {
                    vertexPtrArrayForFace[i][j]->addAdjacentFace(facePtr);
                }
            }

            if (mShouldAssignTextureCoordinates) {
                // Assign texture coordinates to face vertices.
                unsigned vertexIndex = 0;
                for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
                     iterator != facePtr->adjacentVertexEnd(); ++iterator) {
                    mesh::VertexPtr vertexPtr = *iterator;
                    static const cgmath::Vector2f texCoord2fArray[4] = {
                        cgmath::Vector2f(0.0, 0.0),
                        cgmath::Vector2f(1.0, 0.0),
                        cgmath::Vector2f(1.0, 1.0),
                        cgmath::Vector2f(0.0, 1.0)
                    };
                    facePtr->setVertexVector2f(vertexPtr, texCoord2fAttributeKey,
                        texCoord2fArray[vertexIndex]);
                    ++vertexIndex;
                }
            }

            if (mShouldAssignNormals) {
                // Assign a normal vector to the face.
                cgmath::Vector3f normal = cgmath::Vector3f::ZERO;
                normal[axis] = (side == 0) ? -1.0 : 1.0;
                facePtr->setUnitVector3f(normal3fAttributeKey, (normalMatrix*normal).normalized());
            }

            // Add adjacent edges.
            for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
                 iterator != facePtr->adjacentVertexEnd(); ++iterator) {
                mesh::VertexPtr vertexPtr = *iterator;
                mesh::AdjacentVertexIterator nextIterator = iterator;
                ++nextIterator;
                if (nextIterator == facePtr->adjacentVertexEnd()) {
                    nextIterator = facePtr->adjacentVertexBegin();
                }
                mesh::VertexPtr nextVertexPtr = *nextIterator;

                mesh::EdgePtr edgePtr = mMesh->createEdge();

                edgePtr->addAdjacentVertex(vertexPtr);
                edgePtr->addAdjacentVertex(nextVertexPtr);

                vertexPtr->addAdjacentEdge(edgePtr);
                nextVertexPtr->addAdjacentEdge(edgePtr);

                facePtr->addAdjacentEdge(edgePtr);
                edgePtr->addAdjacentFace(facePtr);
            }
        }
    }
}

} // namespace meshprim
