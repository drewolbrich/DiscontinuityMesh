// Copyright 2010 Retroactive Fiasco.

#include "SplitEdgeTriangulator.h"

#include <cassert>

#include "Mesh.h"
#include "MeshOperations.h"
#include "FaceOperations.h"
#include "DeletedElementTracker.h"

namespace mesh {

SplitEdgeTriangulator::SplitEdgeTriangulator()
    : mMesh(NULL),
      mElementTracker(NULL),
      mSplitEdgeVertexAttributeKey()
{
}

SplitEdgeTriangulator::~SplitEdgeTriangulator()
{
}

void
SplitEdgeTriangulator::setMesh(Mesh *mesh)
{
    mMesh = mesh;
}

Mesh *
SplitEdgeTriangulator::mesh() const
{
    return mMesh;
}

void
SplitEdgeTriangulator::setDeletedElementTracker(DeletedElementTracker *deletedElementTracker)
{
    mElementTracker = deletedElementTracker;
}

DeletedElementTracker *
SplitEdgeTriangulator::deletedElementTracker() const
{
    return mElementTracker;
}

void
SplitEdgeTriangulator::initialize()
{
    mSplitEdgeVertexAttributeKey = mMesh->getAttributeKey(
        "__SplitEdgeTriangulator_SplitEdgeVertex",
        AttributeKey::BOOL, AttributeKey::TEMPORARY);
        
}

const AttributeKey &
SplitEdgeTriangulator::splitEdgeVertexAttributeKey() const
{
    assert(mSplitEdgeVertexAttributeKey.isDefined());

    return mSplitEdgeVertexAttributeKey;
}

void
SplitEdgeTriangulator::triangulate()
{
    assert(mMesh != NULL);
    assert(mSplitEdgeVertexAttributeKey.isDefined());

    // Put all the untriangulated faces in a vector, so we can loop over them again later
    // as new faces are being created without worrying about invalidated iterators.
    typedef std::vector<FacePtr> FacePtrVector;
    FacePtrVector facePtrVector;
    for (FacePtr facePtr = mMesh->faceBegin(); 
         facePtr != mMesh->faceEnd(); ++facePtr) {
        // If the face has only three vertices, it's already a triangle.
        if (facePtr->adjacentVertexCount() > 3) {
            facePtrVector.push_back(facePtr);
        }
    }

    // Triangulate all the faces.
    for (FacePtrVector::iterator iterator = facePtrVector.begin();
         iterator != facePtrVector.end(); ++iterator) {
        FacePtr facePtr = *iterator;
        triangulateFace(facePtr);
    }

    // Remove the split edge vertex attribute from all the vertices
    // so they can be assigned again on the next edge splitting pass.
    RemoveAttributeFromMeshAndAllElements(mMesh, mSplitEdgeVertexAttributeKey);
}

void
SplitEdgeTriangulator::triangulateFace(FacePtr facePtr)
{
    // We're assuming that the face was a triangle and one to three of its
    // edges have been split.
    assert(facePtr->adjacentVertexCount() > 3);
    assert(facePtr->adjacentVertexCount() <= 6);

#if DEBUG
    // There should be one to three vertices that were the result of split edges.
    // Anything else is unexpected.
    unsigned splitEdgeVertexCount = 0;
    for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        if (vertexPtr->getBool(mSplitEdgeVertexAttributeKey)) {
            ++splitEdgeVertexCount;
        }
    }
    assert(splitEdgeVertexCount >= 1);
    assert(splitEdgeVertexCount <= 3);
#endif

    // Create an array of the potential six vertices. If an edge was not split,
    // the value mMesh->vertexEnd() will be stored as a placeholder
    // at that position in the array. We'll use this information later
    // to determine how to tessellate the face.
    VertexPtr vertexPtrArray[6];
    for (size_t index = 0; index < 6; ++index) {
        vertexPtrArray[index] = mMesh->vertexEnd();
    }
    size_t index = 1;
    bool lastVertexWasSplitEdgeVertex = false;
    for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        if (!lastVertexWasSplitEdgeVertex
            && !vertexPtr->getBool(mSplitEdgeVertexAttributeKey)) {
            ++index;
        }
        vertexPtrArray[index % 6] = vertexPtr;
        lastVertexWasSplitEdgeVertex = vertexPtr->getBool(mSplitEdgeVertexAttributeKey);
        ++index;
    }

    // Vector of new faces that are created by this function.
    typedef std::vector<FacePtr> FacePtrVector;
    FacePtrVector newFacePtrVector;

    // There are three cases.
    switch (facePtr->adjacentVertexCount()) {
    case 4:
        if (vertexPtrArray[1] != mMesh->vertexEnd()) {
            //     0
            //    / \   .
            //   / __1
            //  /_/   \ .
            // 4-------2
            assert(vertexPtrArray[3] == mMesh->vertexEnd());
            assert(vertexPtrArray[5] == mMesh->vertexEnd());
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[4], vertexPtrArray[0], vertexPtrArray[1]));
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[1], vertexPtrArray[2], vertexPtrArray[4]));
        } else if (vertexPtrArray[3] != mMesh->vertexEnd()) {
            //     0
            //    /|\   .
            //   / | \  .
            //  /  |  \ .
            // 4---3---2
            assert(vertexPtrArray[1] == mMesh->vertexEnd());
            assert(vertexPtrArray[5] == mMesh->vertexEnd());
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[2], vertexPtrArray[3], vertexPtrArray[0]));
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[3], vertexPtrArray[4], vertexPtrArray[0]));
        } else {
            //     0
            //    / \   .
            //   5__ \  .
            //  /   \_\ .
            // 4-------2
            assert(vertexPtrArray[5] != mMesh->vertexEnd());
            assert(vertexPtrArray[1] == mMesh->vertexEnd());
            assert(vertexPtrArray[3] == mMesh->vertexEnd());
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[2], vertexPtrArray[5], vertexPtrArray[0]));
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[4], vertexPtrArray[5], vertexPtrArray[2]));
        }
        break;
    case 5:
        if (vertexPtrArray[1] != mMesh->vertexEnd()
            && vertexPtrArray[3] != mMesh->vertexEnd()) {
            assert(vertexPtrArray[5] == mMesh->vertexEnd());
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[1], vertexPtrArray[2], vertexPtrArray[3]));
            if ((vertexPtrArray[4]->position() - vertexPtrArray[1]->position()).length()
                < (vertexPtrArray[3]->position() - vertexPtrArray[0]->position()).length()) {
                //     0
                //    / \   .
                //   / __1
                //  /_/ / \ .
                // 4---3---2
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[0], vertexPtrArray[1], vertexPtrArray[4]));
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[1], vertexPtrArray[3], vertexPtrArray[4]));
            } else {
                //     0
                //    /|\   .
                //   / | 1
                //  /  |/ \ .
                // 4---3---2
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[0], vertexPtrArray[1], vertexPtrArray[3]));
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[0], vertexPtrArray[3], vertexPtrArray[4]));
            }
        } else if (vertexPtrArray[3] != mMesh->vertexEnd()
            && vertexPtrArray[5] != mMesh->vertexEnd()) {
            assert(vertexPtrArray[1] == mMesh->vertexEnd());
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[5], vertexPtrArray[3], vertexPtrArray[4]));
            if ((vertexPtrArray[5]->position() - vertexPtrArray[2]->position()).length()
                < (vertexPtrArray[3]->position() - vertexPtrArray[0]->position()).length()) {
                //     0
                //    / \   .
                //   5__ \  .
                //  / \ \_\ .
                // 4---3---2
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[0], vertexPtrArray[2], vertexPtrArray[5]));
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[2], vertexPtrArray[3], vertexPtrArray[5]));
            } else {
                //     0
                //    /|\   .
                //   5 | \  .
                //  / \|  \ .
                // 4---3---2
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[0], vertexPtrArray[3], vertexPtrArray[5]));
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[2], vertexPtrArray[3], vertexPtrArray[0]));
            }
        } else {
            assert(vertexPtrArray[5] != mMesh->vertexEnd());
            assert(vertexPtrArray[1] != mMesh->vertexEnd());
            assert(vertexPtrArray[3] == mMesh->vertexEnd());
            newFacePtrVector.push_back(
                CreateTriangularFaceAndEdgesFromVertices(mMesh,
                    vertexPtrArray[0], vertexPtrArray[1], vertexPtrArray[5]));
            if ((vertexPtrArray[5]->position() - vertexPtrArray[2]->position()).length()
                < (vertexPtrArray[4]->position() - vertexPtrArray[1]->position()).length()) {
                //     0
                //    / \   .
                //   5---1
                //  / \___\ .
                // 4-------2
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[1], vertexPtrArray[2], vertexPtrArray[5]));
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[2], vertexPtrArray[4], vertexPtrArray[5]));
            } else {
                //     0
                //    / \   .
                //   5---1
                //  /___/ \ .
                // 4-------2
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[5], vertexPtrArray[1], vertexPtrArray[4]));
                newFacePtrVector.push_back(
                    CreateTriangularFaceAndEdgesFromVertices(mMesh,
                        vertexPtrArray[1], vertexPtrArray[2], vertexPtrArray[4]));
            }
        }
        break;
    case 6:
        //     0
        //    / \   .
        //   5---1
        //  / \ / \ .
        // 4---3---2
        newFacePtrVector.push_back(
            CreateTriangularFaceAndEdgesFromVertices(mMesh,
                vertexPtrArray[1], vertexPtrArray[5], vertexPtrArray[0]));
        newFacePtrVector.push_back(
            CreateTriangularFaceAndEdgesFromVertices(mMesh,
                vertexPtrArray[3], vertexPtrArray[4], vertexPtrArray[5]));
        newFacePtrVector.push_back(
            CreateTriangularFaceAndEdgesFromVertices(mMesh,
                vertexPtrArray[1], vertexPtrArray[2], vertexPtrArray[3]));
        newFacePtrVector.push_back(
            CreateTriangularFaceAndEdgesFromVertices(mMesh,
                vertexPtrArray[3], vertexPtrArray[5], vertexPtrArray[1]));
        break;
    }

    // Copy all attributes from the old face to the new faces.
    for (FacePtrVector::iterator iterator = newFacePtrVector.begin();
         iterator != newFacePtrVector.end(); ++iterator) {
        FacePtr newFacePtr = *iterator;
        newFacePtr->copyAttributes(*facePtr);
    }

    // Copy all face vertex attributes from the old face to the new faces.
    for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        for (FacePtrVector::iterator iterator = newFacePtrVector.begin();
             iterator != newFacePtrVector.end(); ++iterator) {
            FacePtr newFacePtr = *iterator;
            if (newFacePtr->hasAdjacentVertex(vertexPtr)) {
                if (facePtr->hasFaceVertex(vertexPtr)) {
                    newFacePtr->getFaceVertex(vertexPtr)->copyAttributes(
                        *facePtr->getFaceVertex(vertexPtr));
                }
            }
        }
    }

    // Delete the original face from the mesh.
    DeleteFace(mMesh, facePtr, mElementTracker);
}

} // namespace mesh
