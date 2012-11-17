// Copyright 2009 Retroactive Fiasco.

#include "CylinderCreator.h"

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

CylinderCreator::CylinderCreator()
    : mMesh(NULL),
      mTransformationMatrix(cgmath::Matrix4f::IDENTITY),
      mHeight(1.0),
      mRadius(0.5),
      mDivisions(12),
      mFaceAttributes(),
      mShouldAssignNormals(false),
      mShouldAssignTextureCoordinates(false)

{
}

CylinderCreator::~CylinderCreator()
{
}

void
CylinderCreator::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
CylinderCreator::mesh() const
{
    return mMesh;
}

void
CylinderCreator::setTransformationMatrix(const cgmath::Matrix4f &transformationMatrix)
{
    mTransformationMatrix = transformationMatrix;
}

const cgmath::Matrix4f &
CylinderCreator::transformationMatrix() const
{
    return mTransformationMatrix;
}

void
CylinderCreator::setHeight(float height)
{
    mHeight = height;
}

float
CylinderCreator::height() const
{
    return mHeight;
}

void
CylinderCreator::setRadius(float radius)
{
    mRadius = radius;
}

float
CylinderCreator::radius() const
{
    return mRadius;
}

void
CylinderCreator::setDivisions(const unsigned &divisions)
{
    mDivisions = divisions;
}

const unsigned &
CylinderCreator::divisions() const
{
    return mDivisions;
}

void
CylinderCreator::setFaceAttributes(const mesh::AttributePossessor &faceAttributes)
{
    mFaceAttributes = faceAttributes;
}

const mesh::AttributePossessor &
CylinderCreator::faceAttributes() const
{
    return mFaceAttributes;
}

void
CylinderCreator::setShouldAssignNormals(bool shouldAssignNormals)
{
    mShouldAssignNormals = shouldAssignNormals;
}

bool
CylinderCreator::shouldAssignNormals() const
{
    return mShouldAssignNormals;
}

void
CylinderCreator::setShouldAssignTextureCoordinates(bool shouldAssignTextureCoordinates)
{
    mShouldAssignTextureCoordinates = shouldAssignTextureCoordinates;
}

bool
CylinderCreator::shouldAssignTextureCoordinates() const
{
    return mShouldAssignTextureCoordinates;
}

void
CylinderCreator::create()
{
    assert(mMesh != NULL);

    // Create vertices.
    std::vector<mesh::VertexPtr> vertexPtrArray[2];
    for (size_t i = 0; i < 2; ++i) {
        vertexPtrArray[i].resize(mDivisions);
        for (size_t j = 0; j < mDivisions; ++j) {
            mesh::VertexPtr vertexPtr = mMesh->createVertex();
            assert(i < 2);
            assert(j < vertexPtrArray[i].size());
            vertexPtrArray[i][j] = vertexPtr;
            float angle = float(j)/mDivisions*2.0*cgmath::PI;
            vertexPtr->setPosition(mTransformationMatrix
                *cgmath::Vector3f(
                    sinf(angle)*mRadius,
                    0.5*mHeight*(int(i)*2 - 1),
                    cosf(angle)*mRadius)); 
        }
    }

    // Create edges.
    std::vector<mesh::EdgePtr> latitudeEdgePtrArray[2];
    for (size_t i = 0; i < 2; ++i) {
        latitudeEdgePtrArray[i].resize(mDivisions);
        for (size_t j = 0; j < mDivisions; ++j) {
            size_t j2 = (j + 1) % mDivisions;
            mesh::EdgePtr edgePtr = mMesh->createEdge();
            assert(i < 2);
            assert(j < latitudeEdgePtrArray[i].size());
            latitudeEdgePtrArray[i][j] = edgePtr;
            assert(i < 2);
            assert(j < vertexPtrArray[i].size());
            mesh::VertexPtr v0 = vertexPtrArray[i][j];
            assert(j2 < vertexPtrArray[i].size());
            mesh::VertexPtr v1 = vertexPtrArray[i][j2];
            edgePtr->addAdjacentVertex(v0);
            edgePtr->addAdjacentVertex(v1);
            v0->addAdjacentEdge(edgePtr);
            v1->addAdjacentEdge(edgePtr);
        }
    }
    std::vector<mesh::EdgePtr> longitudeEdgePtrArray;
    longitudeEdgePtrArray.resize(mDivisions);
    for (size_t j = 0; j < mDivisions; ++j) {
        mesh::EdgePtr edgePtr = mMesh->createEdge();
        assert(j < longitudeEdgePtrArray.size());
        longitudeEdgePtrArray[j] = edgePtr;
        assert(j < vertexPtrArray[0].size());
        mesh::VertexPtr v0 = vertexPtrArray[0][j];
        assert(j < vertexPtrArray[1].size());
        mesh::VertexPtr v1 = vertexPtrArray[1][j];
        edgePtr->addAdjacentVertex(v0);
        edgePtr->addAdjacentVertex(v1);
        v0->addAdjacentEdge(edgePtr);
        v1->addAdjacentEdge(edgePtr);
    }

    mesh::AttributeKey texCoord2fAttributeKey = mesh::GetTexCoord2fAttributeKey(*mMesh);
    mesh::AttributeKey normal3fAttributeKey = mesh::GetNormal3fAttributeKey(*mMesh);

    cgmath::Matrix4f normalMatrix(cgmath::Matrix3f(mTransformationMatrix).inverse().transpose());

    // Create faces with face attributes, face normals or face vertex normals,
    // and texture coordinates.

    for (size_t i = 0; i < 2; ++i) {
        mesh::FacePtr facePtr = mMesh->createFace();
        facePtr->copyAttributes(mFaceAttributes);

        for (size_t j2 = 0; j2 < mDivisions; ++j2) {
            size_t j;
            if (i == 0) {
                j = mDivisions - 1 - j2;
            } else {
                j = j2;
            }

            assert(i < 2);
            assert(j < vertexPtrArray[i].size());
            facePtr->addAdjacentVertex(vertexPtrArray[i][j]);
            vertexPtrArray[i][j]->addAdjacentFace(facePtr);

            if (mShouldAssignTextureCoordinates) {
                float angle = float(j)/mDivisions*2.0*cgmath::PI;
                if (i == 0) {
                    facePtr->setVertexVector2f(vertexPtrArray[i][j], texCoord2fAttributeKey,
                        cgmath::Vector2f(
                            sinf(angle)*0.5 + 0.5,
                            cosf(angle)*0.5 + 0.5));
                } else {
                    facePtr->setVertexVector2f(vertexPtrArray[i][j], texCoord2fAttributeKey,
                        cgmath::Vector2f(
                            cosf(angle)*0.5 + 0.5,
                            sinf(angle)*0.5 + 0.5));
                }
            }

            if (i == 0) {
                j = (j + 1) % mDivisions;
            }

            assert(i < 2);
            assert(j < latitudeEdgePtrArray[i].size());
            facePtr->addAdjacentEdge(latitudeEdgePtrArray[i][j]);
            latitudeEdgePtrArray[i][j]->addAdjacentFace(facePtr);
        }

        if (mShouldAssignNormals) {
            cgmath::Vector3f normal = cgmath::Vector3f::ZERO;
            normal[1] = (i == 0) ? -1.0 : 1.0;
            facePtr->setUnitVector3f(normal3fAttributeKey, (normalMatrix*normal).normalized());
        }
    }

    for (size_t j = 0; j < mDivisions; ++j) {
        mesh::FacePtr facePtr = mMesh->createFace();
        facePtr->copyAttributes(mFaceAttributes);

        size_t edgeIndex = 0;
        for (size_t m = 0; m < 2; ++m) {
            for (size_t n2 = 0; n2 < 2; ++n2) {
                size_t n = (m == 0) ? n2 : 1 - n2;
                size_t tn = (j + n);
                n = (j + n) % mDivisions;
                assert(m < 2);
                assert(n < vertexPtrArray[m].size());
                facePtr->addAdjacentVertex(vertexPtrArray[m][n]);
                vertexPtrArray[m][n]->addAdjacentFace(facePtr);

                if (mShouldAssignTextureCoordinates) {
                    facePtr->setVertexVector2f(vertexPtrArray[m][n], texCoord2fAttributeKey,
                        cgmath::Vector2f(
                            float(tn)/mDivisions, float(m)));
                }

                if (mShouldAssignNormals) {
                    float angle = float(n)/mDivisions*2.0*cgmath::PI;
                    cgmath::Vector3f normal(sinf(angle), 0.0, cosf(angle));
                    assert(m < 2);
                    assert(n < vertexPtrArray[m].size());
                    facePtr->setVertexUnitVector3f(vertexPtrArray[m][n],
                        normal3fAttributeKey, (normalMatrix*normal).normalized());
                }

                if (edgeIndex % 2 == 0) {
                    assert(m < 2);
                    assert(n2 < latitudeEdgePtrArray[m].size());
                    facePtr->addAdjacentEdge(latitudeEdgePtrArray[m][n2]);
                    latitudeEdgePtrArray[m][n2]->addAdjacentFace(facePtr);
                } else {
                    assert(n < longitudeEdgePtrArray.size());
                    facePtr->addAdjacentEdge(longitudeEdgePtrArray[n]);
                    longitudeEdgePtrArray[n]->addAdjacentFace(facePtr);
                }

                ++edgeIndex;
            }
        }
    }
}

} // namespace meshprim
