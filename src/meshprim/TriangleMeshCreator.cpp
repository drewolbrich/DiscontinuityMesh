// Copyright 2009 Drew Olbrich

#include "TriangleMeshCreator.h"

#include <cassert>
#include <algorithm>
#include <map>

#include <mesh/Mesh.h>
#include <mesh/FaceOperations.h>
#include <mesh/StandardAttributes.h>

namespace meshprim {

TriangleMeshCreator::TriangleMeshCreator()
    : mMesh(NULL),
      mTransformationMatrix(cgmath::Matrix4f::IDENTITY),
      mVertexPositionVector(NULL),
      mTriangleVector(NULL),
      mFaceAttributes(),
      mShouldAssignNormals(false),
      mVertexPtrVector(),
      mEdgePtrVector(),
      mFacePtrVector()
{
}

TriangleMeshCreator::~TriangleMeshCreator()
{
}

void
TriangleMeshCreator::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
TriangleMeshCreator::mesh() const
{
    return mMesh;
}

void
TriangleMeshCreator::setTransformationMatrix(const cgmath::Matrix4f &transformationMatrix)
{
    mTransformationMatrix = transformationMatrix;
}

const cgmath::Matrix4f &
TriangleMeshCreator::transformationMatrix() const
{
    return mTransformationMatrix;
}

void
TriangleMeshCreator::setVertexPositionVector(const VertexPositionVector *vertexPositionVector)
{
    mVertexPositionVector = vertexPositionVector;
}

const TriangleMeshCreator::VertexPositionVector *
TriangleMeshCreator::vertexPositionVector() const
{
    return mVertexPositionVector;
}

void
TriangleMeshCreator::setTriangleVector(const TriangleVector *triangleVector)
{
    mTriangleVector = triangleVector;
}

const TriangleMeshCreator::TriangleVector *
TriangleMeshCreator::triangleVector() const
{
    return mTriangleVector;
}

void
TriangleMeshCreator::setFaceAttributes(const mesh::AttributePossessor &faceAttributes)
{
    mFaceAttributes = faceAttributes;
}

const mesh::AttributePossessor &
TriangleMeshCreator::faceAttributes() const
{
    return mFaceAttributes;
}

void
TriangleMeshCreator::setShouldAssignNormals(bool shouldAssignNormals)
{
    mShouldAssignNormals = shouldAssignNormals;
}

bool
TriangleMeshCreator::shouldAssignNormals() const
{
    return mShouldAssignNormals;
}

void
TriangleMeshCreator::create()
{
    assert(mVertexPositionVector != NULL);
    assert(mTriangleVector != NULL);

    mVertexPtrVector.clear();
    mEdgePtrVector.clear();
    mFacePtrVector.clear();

    // Create all the vertices.
    for (size_t index = 0; index < mVertexPositionVector->size(); ++index) {
        mesh::VertexPtr vertexPtr = mMesh->createVertex();
        vertexPtr->setPosition(mTransformationMatrix*(*mVertexPositionVector)[index]);
        mVertexPtrVector.push_back(vertexPtr);
    }

    // Map of VertexPtr pairs to EdgePtrs. As we create new edges, we add them to this
    // map to ensure that we don't create the same edge multiple times.
    typedef std::pair<mesh::VertexPtr, mesh::VertexPtr> VertexPtrPair;
    typedef std::map<VertexPtrPair, mesh::EdgePtr> VertexPtrPairToEdgePtrMap;
    VertexPtrPairToEdgePtrMap vertexPtrPairToEdgePtrMap;

    mesh::AttributeKey normal3fAttributeKey = mesh::GetNormal3fAttributeKey(*mMesh);

    // Create all the faces.
    for (size_t index = 0; index < mTriangleVector->size(); ++index) {
        mesh::FacePtr facePtr = mMesh->createFace();
        facePtr->copyAttributes(mFaceAttributes);

        mFacePtrVector.push_back(facePtr);
        for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
            size_t vertexPtrIndex = (*mTriangleVector)[index].mVertexIndexArray[vertexIndex];
            assert(vertexPtrIndex < mVertexPtrVector.size());
            mesh::VertexPtr vertexPtr = mVertexPtrVector[vertexPtrIndex];
            facePtr->addAdjacentVertex(vertexPtr);
            vertexPtr->addAdjacentFace(facePtr);
        }

        if (mShouldAssignNormals) {
            facePtr->setUnitVector3f(normal3fAttributeKey,
                mesh::GetFaceGeometricNormal(facePtr));
        }

        // Create the edges for this face.
        for (size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
            size_t vertexPtrIndex0 = (*mTriangleVector)[index].mVertexIndexArray[vertexIndex];
            assert(vertexPtrIndex0 < mVertexPtrVector.size());
            mesh::VertexPtr v0 = mVertexPtrVector[vertexPtrIndex0];

            size_t vertexPtrIndex1 = (*mTriangleVector)[index].mVertexIndexArray[
                (vertexIndex + 1) % 3];
            assert(vertexPtrIndex1 < mVertexPtrVector.size());
            mesh::VertexPtr v1 = mVertexPtrVector[vertexPtrIndex1];

            VertexPtrPair vertexPtrPair = std::make_pair(v0, v1);

            // Sort the vertices by pointer address, so that we're always
            // looking up the vertices for a given edge in a consistent order.
            // We only swap the vertices in the VertexPtrPair, not v0 and v1,
            // so that the edge is created with vertices v0 and v1 in their
            // original order. Some unit tests rely on this for
            // deterministic behavior.
            if (&*vertexPtrPair.first > &*vertexPtrPair.second) {
                std::swap(vertexPtrPair.first, vertexPtrPair.second);
            }

            VertexPtrPairToEdgePtrMap::iterator iterator 
                = vertexPtrPairToEdgePtrMap.find(vertexPtrPair);
            mesh::EdgePtr edgePtr;
            if (iterator != vertexPtrPairToEdgePtrMap.end()) {
                // An edge already exists between these two vertices.
                edgePtr = iterator->second;
            } else {
                // No edge already exists between these two vertices. Create a new one.
                edgePtr = mMesh->createEdge();
                edgePtr->addAdjacentVertex(v0);
                edgePtr->addAdjacentVertex(v1);
                v0->addAdjacentEdge(edgePtr);
                v1->addAdjacentEdge(edgePtr);
                vertexPtrPairToEdgePtrMap[vertexPtrPair] = edgePtr;
                mEdgePtrVector.push_back(edgePtr);
            }

            edgePtr->addAdjacentFace(facePtr);
            facePtr->addAdjacentEdge(edgePtr);
        }
    }
}

const TriangleMeshCreator::VertexPtrVector &
TriangleMeshCreator::vertexPtrVector() const
{
    return mVertexPtrVector;
}

const TriangleMeshCreator::EdgePtrVector &
TriangleMeshCreator::edgePtrVector() const
{
    return mEdgePtrVector;
}

const TriangleMeshCreator::FacePtrVector &
TriangleMeshCreator::facePtrVector() const
{
    return mFacePtrVector;
}

} // namespace meshprim
