// Copyright 2009 Drew Olbrich

#include "VertexOperations.h"

#include <cassert>

#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "Mesh.h"
#include "AttributePossessorOperations.h"
#include "DeletedElementTracker.h"
#include "EdgeOperations.h"

namespace mesh {

void 
DeleteVertex(Mesh *mesh, VertexPtr vertexPtr, DeletedElementTracker *deletedElementTracker)
{
    for (AdjacentEdgeIterator iterator = vertexPtr->adjacentEdgeBegin();
         iterator != vertexPtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr edgePtr = *iterator;

        // The adjacent edge must refer back to the vertex.
        assert(edgePtr->hasAdjacentVertex(vertexPtr));
            
        edgePtr->removeAdjacentVertex(vertexPtr);
    }

    for (AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;

        // The adjacent face must refer back to the vertex.
        assert(facePtr->hasAdjacentVertex(vertexPtr));
            
        facePtr->removeAdjacentVertex(vertexPtr);
    }
    
    mesh->destroyVertex(vertexPtr);

    // Track the deleted vertex.
    if (deletedElementTracker != NULL) {
        deletedElementTracker->addVertex(vertexPtr);
    }
}

void
MergeVertexIntoVertex(Mesh *mesh, VertexPtr sourceVertexPtr,
    VertexPtr targetVertexPtr, DeletedElementTracker *deletedElementTracker)
{
#ifdef DEBUG
    // Each of the faces adjacent to the vertices must be triangles.
    // We don't currently handle any other case.
    for (AdjacentFaceIterator iterator = sourceVertexPtr->adjacentFaceBegin();
         iterator != sourceVertexPtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;
        assert(facePtr->adjacentVertexCount() == 3);
    }
    for (AdjacentFaceIterator iterator = targetVertexPtr->adjacentFaceBegin();
         iterator != targetVertexPtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;
        assert(facePtr->adjacentVertexCount() == 3);
    }
#endif

    // Find all the edges that are adjacent to both vertices.
    std::vector<EdgePtr> sharedEdgePtrVector;
    for (AdjacentEdgeIterator iterator = sourceVertexPtr->adjacentEdgeBegin();
         iterator != sourceVertexPtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr edgePtr = *iterator;
        if (edgePtr->hasAdjacentVertex(targetVertexPtr)) {
            sharedEdgePtrVector.push_back(edgePtr);
        }
    }

    // If the vertices have no adjacent edges in common, this case can
    // be handled by MergeVertexIntoNonadjacentVertex.
    if (sharedEdgePtrVector.empty()) {
        MergeVertexIntoNonadjacentVertex(mesh, sourceVertexPtr, targetVertexPtr, 
            deletedElementTracker);
        return;
    }

    EdgePtr edgePtr = sharedEdgePtrVector[0];

    // If the vertices have more then one adjacent edge in common,
    // we must first merge all the edges into the first of these edges.
    // Later, we'll call CollapseEdge to merge the vertices, and this function
    // can't handle multiple edges sharing the same vertices.
    if (sharedEdgePtrVector.size() > 1) {
        for (size_t index = 1; index < sharedEdgePtrVector.size(); ++index) {
            MergeEdgeIntoEdgeWithSharedVertices(mesh,
                sharedEdgePtrVector[index], edgePtr, deletedElementTracker);
        }
    }

    // Remove the vertices adjacent to the edge, and then add them back
    // in an order that ensures that CollapseEdge will merge sourceVertexPtr
    // into targetVertexPtr.
    edgePtr->removeAdjacentVertex(sourceVertexPtr);
    edgePtr->removeAdjacentVertex(targetVertexPtr);
    edgePtr->addAdjacentVertex(sourceVertexPtr);
    edgePtr->addAdjacentVertex(targetVertexPtr);

    // Collapse the edge.
    CollapseEdge(mesh, edgePtr, deletedElementTracker);
}

void
MergeVertexIntoNonadjacentVertex(Mesh *mesh, VertexPtr sourceVertexPtr, 
    VertexPtr targetVertexPtr, DeletedElementTracker *deletedElementTracker)
{
    assert(sourceVertexPtr != targetVertexPtr);

    // Copy the adjacent edges of the source vertex to the target vertex.
    for (AdjacentEdgeIterator iterator = sourceVertexPtr->adjacentEdgeBegin();
         iterator != sourceVertexPtr->adjacentEdgeEnd(); ++iterator) {
        EdgePtr sourceEdgePtr = *iterator;

        // The target vertex must not already have this adjacent edge in common
        // with the source vertex.
        assert(!targetVertexPtr->hasAdjacentEdge(sourceEdgePtr));
        targetVertexPtr->addAdjacentEdge(sourceEdgePtr);

        assert(!sourceEdgePtr->hasAdjacentVertex(targetVertexPtr));
        sourceEdgePtr->addAdjacentVertex(targetVertexPtr);
    }

    // Copy the adjacent faces of the source vertex to the target vertex.
    for (AdjacentFaceIterator iterator = sourceVertexPtr->adjacentFaceBegin();
         iterator != sourceVertexPtr->adjacentFaceEnd(); ++iterator) {
        FacePtr sourceFacePtr = *iterator;

        // The target vertex must not already have this adjacent face in common
        // with the source vertex.
        assert(!targetVertexPtr->hasAdjacentFace(sourceFacePtr));
        targetVertexPtr->addAdjacentFace(sourceFacePtr);

        assert(!sourceFacePtr->hasAdjacentVertex(targetVertexPtr));
        // We use the function insertAdjacentVertex so that the existing vertex order
        // on the target face is maintained.
        sourceFacePtr->insertAdjacentVertex(targetVertexPtr, sourceVertexPtr);

        // Transfer over any FaceVertex attributes.
        if (sourceFacePtr->hasFaceVertex(sourceVertexPtr)) {
            MergeAttributePossessorAttributes(*mesh, 
                *sourceFacePtr->getFaceVertex(sourceVertexPtr),
                sourceFacePtr->getFaceVertex(targetVertexPtr));
        }
    }

    // Copy over all vertex attributes not already present in the target vertex.
    MergeAttributePossessorAttributes(*mesh, *sourceVertexPtr, &*targetVertexPtr);

    // Destroy the source vertex and remove it from all its adjacent elements.
    DeleteVertex(mesh, sourceVertexPtr, deletedElementTracker);
}

bool
FindEdgeConnectingVertices(VertexPtr vertexPtr0, VertexPtr vertexPtr1, EdgePtr *edgePtr)
{
    for (AdjacentEdgeIterator iterator = vertexPtr0->adjacentEdgeBegin();
         iterator != vertexPtr0->adjacentEdgeEnd(); ++iterator) {
        if ((*iterator)->hasAdjacentVertex(vertexPtr1)) {
            *edgePtr = *iterator;
            return true;
        }
    }

    return false;
}

} // namespace mesh
