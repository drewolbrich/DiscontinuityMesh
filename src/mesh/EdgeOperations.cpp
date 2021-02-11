// Copyright 2008 Drew Olbrich

#include "EdgeOperations.h"

#include <cassert>
#include <vector>
#include <algorithm>

#include "InterpolateAttributes.h"
#include "Mesh.h"
#include "AttributePossessorOperations.h"
#include "VertexOperations.h"
#include "FaceOperations.h"
#include "DeletedElementTracker.h"

using cgmath::Vector3f;

namespace mesh {

void
GetEdgeAdjacentVertices(ConstEdgePtr edgePtr, VertexPtr *v0, VertexPtr *v1)
{
    assert(edgePtr->adjacentVertexCount() == 2);

    AdjacentVertexConstIterator iterator = edgePtr->adjacentVertexBegin();
    *v0 = *iterator;
    ++iterator;
    *v1 = *iterator;
#ifdef DEBUG
    ++iterator;
    assert(iterator == edgePtr->adjacentVertexEnd());
#endif
}

void
GetEdgeVertexPositions(ConstEdgePtr edgePtr, cgmath::Vector3f *p0, cgmath::Vector3f *p1)
{
    assert(edgePtr->adjacentVertexCount() == 2);

    AdjacentVertexConstIterator iterator = edgePtr->adjacentVertexBegin();
    *p0 = (*iterator)->position();
    ++iterator;
    *p1 = (*iterator)->position();
#ifdef DEBUG
    ++iterator;
    assert(iterator == edgePtr->adjacentVertexEnd());
#endif
}

float
GetEdgeLength(ConstEdgePtr edgePtr)
{
    cgmath::Vector3f p0;
    cgmath::Vector3f p1;
    GetEdgeVertexPositions(edgePtr, &p0, &p1);

    return (p1 - p0).length();
}

void 
DeleteEdge(Mesh *mesh, EdgePtr edgePtr, DeletedElementTracker *deletedElementTracker)
{
    for (AdjacentVertexIterator iterator = edgePtr->adjacentVertexBegin();
         iterator != edgePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;

        // The adjacent vertex must refer back to the edge.
        assert(vertexPtr->hasAdjacentEdge(edgePtr));
            
        vertexPtr->removeAdjacentEdge(edgePtr);
    }
    
    for (AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;

        // The adjacent face must refer back to the edge.
        assert(facePtr->hasAdjacentEdge(edgePtr));
            
        facePtr->removeAdjacentEdge(edgePtr);
    }

    mesh->destroyEdge(edgePtr);

    // Track the deleted edge.
    if (deletedElementTracker != NULL) {
        deletedElementTracker->addEdge(edgePtr);
    }
}

void 
SplitEdge(Mesh *mesh, EdgePtr edgePtr, const cgmath::Vector3f &position, 
    VertexPtr *vertexPtrResult, EdgePtr *edgePtrResult)
{
    // Create the new vertex that will be inserted into the edge.
    VertexPtr newVertexPtr = mesh->createVertex();
    newVertexPtr->setPosition(position);

    // The new edge inherits the attributes of the original edge.
    EdgePtr newEdgePtr = mesh->createEdge();
    newEdgePtr->copyAttributes(*edgePtr);

    // The edge being split must have exactly two adjacent vertices
    // for it to be a valid edge.
    assert(edgePtr->adjacentVertexCount() == 2);

    VertexPtr firstVertexPtr;
    VertexPtr secondVertexPtr;
    GetEdgeAdjacentVertices(edgePtr, &firstVertexPtr, &secondVertexPtr);

    // The attributes of the new vertex should be assigned based on
    // its relative position to the two vertices of the original edge.
    std::vector<ConstVertexPtr> vertexPtrVector;
    vertexPtrVector.push_back(firstVertexPtr);
    vertexPtrVector.push_back(secondVertexPtr);

    AssignInterpolatedVertexAttributes(newVertexPtr, vertexPtrVector);
    
    // The original edge should be adjacent to the second vertex
    // and the new vertex. It should no longer be adjacent
    // to the first vertex.
    // We remove the second vertex and then add it back in after
    // the new vertex to maintain a consistent ordering that
    // user code can rely on when calling SplitEdge repeatedly.
    edgePtr->removeAdjacentVertex(firstVertexPtr);
    edgePtr->removeAdjacentVertex(secondVertexPtr);
    edgePtr->addAdjacentVertex(newVertexPtr);
    edgePtr->addAdjacentVertex(secondVertexPtr);

    // The new edge should be adjacent to the new vertex and
    // the first vertex.
    newEdgePtr->addAdjacentVertex(firstVertexPtr);
    newEdgePtr->addAdjacentVertex(newVertexPtr);

    // The first vertex should be adjacent to the new edge,
    // and no longer be adjacent to the old edge.
    firstVertexPtr->addAdjacentEdge(newEdgePtr);
    firstVertexPtr->removeAdjacentEdge(edgePtr);
    
    // The new vertex should be adjacent to the new and old edges.
    newVertexPtr->addAdjacentEdge(newEdgePtr);
    newVertexPtr->addAdjacentEdge(edgePtr);

    for (AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr(*iterator);

        // The new vertex should be adjacent to the faces that were
        // adjacent to the original edge.
        newVertexPtr->addAdjacentFace(facePtr);

        // The new edge should have the same adjacent faces as the
        // original edge.
        newEdgePtr->addAdjacentFace(facePtr);

        // The face must not be degenerate.
        assert(facePtr->adjacentVertexCount() > 2);
        
        // The faces that were adjacent to the old edge should be
        // adjacent to the new vertex. However, we need to insert the
        // new vertex in the correct order.
        bool foundInsertionPoint = false;
        for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            VertexPtr vertexPtr(*iterator);
            AdjacentVertexIterator nextIterator = iterator;
            ++nextIterator;
            VertexPtr nextVertexPtr;
            if (nextIterator == facePtr->adjacentVertexEnd()) {
                nextVertexPtr = *(facePtr->adjacentVertexBegin());
            } else {
                nextVertexPtr = *nextIterator;
            }
            if (vertexPtr == firstVertexPtr
                && nextVertexPtr == secondVertexPtr) {
                facePtr->insertAdjacentVertex(newVertexPtr, secondVertexPtr);
                foundInsertionPoint = true;
                break;
            } else if (vertexPtr == secondVertexPtr
                && nextVertexPtr == firstVertexPtr) {
                facePtr->insertAdjacentVertex(newVertexPtr, firstVertexPtr);
                foundInsertionPoint = true;
                break;
            }
        }

        assert(foundInsertionPoint);
        
        // The faces that were adjacent to the old edge should be
        // adjacent to the new edge.
        facePtr->addAdjacentEdge(newEdgePtr);

        // Assign interpolated face vertex attributes
        // for the newly created vertex, for this face.
        AssignInterpolatedFaceVertexAttributes(facePtr,
            newVertexPtr, facePtr, vertexPtrVector);
    }

    assert(edgePtr->adjacentVertexCount() == 2);
    assert(newEdgePtr->adjacentVertexCount() == 2);
    assert(newVertexPtr->adjacentEdgeCount() == 2);

    if (vertexPtrResult != NULL) {
        *vertexPtrResult = newVertexPtr;
    }

    if (edgePtrResult != NULL) {
        *edgePtrResult = newEdgePtr;
    }
}

void
SplitEdgeAndAdjacentTriangularFaces(Mesh *mesh, EdgePtr edgePtr,
    const cgmath::Vector3f &position, VertexPtr *vertexPtrResult,
    DeletedElementTracker *deletedElementTracker)
{
#ifdef DEBUG
    // Each of the faces adjacent to the edge must be triangles.
    for (AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;
        assert(facePtr->adjacentVertexCount() == 3);
    }
#endif

    // Create a vector of all faces adjacent to the edge we're about to split.
    std::vector<FacePtr> adjacentFacePtrVector;
    std::copy(edgePtr->adjacentFaceBegin(), edgePtr->adjacentFaceEnd(),
        std::back_inserter(adjacentFacePtrVector));

    // Split the edge.
    VertexPtr newVertexPtr;
    SplitEdge(mesh, edgePtr, position, &newVertexPtr, NULL);

    // Retriangulate the faces adjacent to the edge we just split.
    for (size_t index = 0; index < adjacentFacePtrVector.size(); ++index) {
        FacePtr facePtr = adjacentFacePtrVector[index];
        TriangulateQuadrilateralFaceFromVertex(mesh, facePtr, newVertexPtr, deletedElementTracker);
    }

    if (vertexPtrResult != NULL) {
        *vertexPtrResult = newVertexPtr;
    }
}

VertexPtr
CollapseEdge(Mesh *mesh, EdgePtr edgePtr, DeletedElementTracker *deletedElementTracker)
{
#ifdef DEBUG
    // Each of the faces adjacent to the edge must be triangles.
    // We don't currently handle any other case.
    for (AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;
        assert(facePtr->adjacentVertexCount() == 3);
    }
#endif

    VertexPtr v0;
    VertexPtr v1;
    GetEdgeAdjacentVertices(edgePtr, &v0, &v1);

#ifdef DEBUG
    // There must not be any other edges that are adjacent to the same two vertices
    // as the edge being collapsed. If there were, these edges would
    // wind up with duplicate adjacent vertices.
    // In these cases, it is best to ues MergeVertexIntoVertex instead.
    for (AdjacentEdgeIterator iterator = v0->adjacentEdgeBegin();
         iterator != v0->adjacentEdgeEnd(); ++iterator) {
        EdgePtr otherEdgePtr = *iterator;
        if (otherEdgePtr == edgePtr) {
            continue;
        }
        VertexPtr otherEdgeV0;
        VertexPtr otherEdgeV1;
        GetEdgeAdjacentVertices(otherEdgePtr, &otherEdgeV0, &otherEdgeV1);
        assert(!(otherEdgeV0 == v0 && otherEdgeV1 == v1)
            && !(otherEdgeV0 == v1 && otherEdgeV1 == v0));
    }
#endif

    // Loop over all adjacent faces, and create a vector of the pairs of edges
    // on those triangles that aren't the edge we're collapsing.
    // After we delete those faces, we'll merge the edges.
    typedef std::pair<EdgePtr, EdgePtr> EdgePtrPair;
    typedef std::vector<EdgePtrPair> EdgePtrPairVector;
    EdgePtrPairVector edgePtrPairVector;
    edgePtrPairVector.reserve(edgePtr->adjacentFaceCount());
    for (AdjacentFaceIterator iterator = edgePtr->adjacentFaceBegin();
         iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;
        EdgePtr otherEdgePtr1;
        EdgePtr otherEdgePtr2;
        GetTriangularFaceOtherEdges(facePtr, edgePtr, &otherEdgePtr1, &otherEdgePtr2);
        // Order the edges such that when we collapse them later,
        // the edges adjacent to v0 will be merged into the edges adjacent
        // to v1, which is consistent with how v0 will be merged into v1.
        if (otherEdgePtr2->hasAdjacentVertex(v0)) {
            std::swap(otherEdgePtr1, otherEdgePtr2);
        }

        // In the case where, adjacent to the edge we're collapsing, there
        // are two faces that share the same three edges,
        // don't record the same edge pair twice, because later we'd
        // wind up trying to merge these edges twice, after one of them
        // had been deleted.
        if (std::find(edgePtrPairVector.begin(), edgePtrPairVector.end(),
                std::make_pair(otherEdgePtr1, otherEdgePtr2))
            != edgePtrPairVector.end()) {
            continue;
        }

        // In the case where collapsing faces would result in the same edge being
        // merged into two other edges, arrange that all three edges
        // are merged together later.
        for (EdgePtrPairVector::const_iterator iterator = edgePtrPairVector.begin();
             iterator != edgePtrPairVector.end(); ++iterator) {
            if (iterator->first == otherEdgePtr1) {
                otherEdgePtr1 = otherEdgePtr2;
                otherEdgePtr2 = iterator->second;
                break;
            }
        }

        // We're assuming that the edge swapping has put the edges in an
        // order that is consistent regardless of the face.
        assert(std::find(edgePtrPairVector.begin(), edgePtrPairVector.end(),
                std::make_pair(otherEdgePtr2, otherEdgePtr1))
            == edgePtrPairVector.end());

        edgePtrPairVector.push_back(std::make_pair(otherEdgePtr1, otherEdgePtr2));
    }

    // Delete the adjacent faces.
    // Keep referencing the first iterator, to handle the fact
    // that the adjacent vector of faces will change as each face is deleted.
    while (edgePtr->adjacentFaceCount() != 0) {
        FacePtr facePtr = *edgePtr->adjacentFaceBegin();
        DeleteFace(mesh, facePtr, deletedElementTracker);
    }

    // Delete the edge we're collapsing.
    DeleteEdge(mesh, edgePtr, deletedElementTracker);

    // Merge the edge's vertices together.
    MergeVertexIntoNonadjacentVertex(mesh, v0, v1, deletedElementTracker);

    // Merge together the pairs of edges we stored earlier.
    // This closes the hole we created in the mesh by removing
    // the collapsed edge and its adjacent faces.
    for (EdgePtrPairVector::const_iterator iterator = edgePtrPairVector.begin();
         iterator != edgePtrPairVector.end(); ++iterator) {
        const EdgePtrPair &edgePtrPair = *iterator;

        MergeEdgeIntoEdgeWithSharedVertices(mesh, 
            edgePtrPair.first, edgePtrPair.second, deletedElementTracker);
    }

    return v1;
}

float
GetEpsilonFromEdge(ConstEdgePtr edgePtr, float absoluteTolerance, float relativeTolerance)
{
    float epsilon = 0.0;
    for (AdjacentVertexConstIterator iterator = edgePtr->adjacentVertexBegin();
         iterator != edgePtr->adjacentVertexEnd(); ++iterator) {
        ConstVertexPtr vertexPtr = *iterator;
        epsilon = std::max(epsilon, vertexPtr->position().maxAbs()*relativeTolerance);
    }

    return std::max(epsilon, absoluteTolerance);
}

void
MergeEdgeIntoEdge(Mesh *mesh, EdgePtr sourceEdgePtr, EdgePtr targetEdgePtr, 
    DeletedElementTracker *deletedElementTracker)
{
    // We have to track deleted edges, so we don't try to merge
    // edges that are deleted as a side effect of vertex merging.
    // We have to do this even no DeletedElementTracker was specified.
    // To this end, we'll use a local DeletedElementTracker and then merge it
    // with the specified DeletedElementTracker if necessary.
    DeletedElementTracker localElementTracker;
    if (deletedElementTracker != NULL) {
        localElementTracker.setElementMask(DeletedElementTracker::EDGES 
            | deletedElementTracker->elementMask());
    } else {
        localElementTracker.setElementMask(DeletedElementTracker::EDGES);
    }

    VertexPtr sourceV0;
    VertexPtr sourceV1;
    GetEdgeAdjacentVertices(sourceEdgePtr, &sourceV0, &sourceV1);

    VertexPtr targetV0;
    VertexPtr targetV1;
    GetEdgeAdjacentVertices(targetEdgePtr, &targetV0, &targetV1);

    // The vertex orders of the edges determine which vertices will be merged.
    // Therefore, configurations that would result in degenerate edges with
    // duplicate vertices are not allowed.
    assert(sourceV0 != targetV1);
    assert(targetV0 != sourceV1);

    // Merge the vertices of the edges that are not already shared.
    if (sourceV0 != targetV0) {
        MergeVertexIntoVertex(mesh, sourceV0, targetV0, &localElementTracker);
    }
    if (sourceV1 != targetV1) {
        MergeVertexIntoVertex(mesh, sourceV1, targetV1, &localElementTracker);
    }

    // If we know we already deleted the source edge as a side effect
    // of the vertex merging steps above, we shouldn't attempt to merge it.
    if (!localElementTracker.hasEdge(sourceEdgePtr)) {
        MergeEdgeIntoEdgeWithSharedVertices(mesh, sourceEdgePtr, targetEdgePtr, 
            deletedElementTracker);
    }

    // Merge our local DeletedElementTracker's elements with the caller's 
    // DeletedElementTracker, if any.
    if (deletedElementTracker != NULL) {
        deletedElementTracker->addElementsFromElementTracker(localElementTracker);
    }
}

void
MergeEdgeIntoEdgeWithSharedVertices(Mesh *mesh, EdgePtr sourceEdgePtr, EdgePtr targetEdgePtr,
    DeletedElementTracker *deletedElementTracker)
{
    assert(sourceEdgePtr != targetEdgePtr);

#ifdef DEBUG
    // The edges must already have both endpoints in common.
    assert(sourceEdgePtr->adjacentVertexCount() == 2);
    assert(targetEdgePtr->adjacentVertexCount() == 2);
    for (AdjacentVertexIterator iterator = sourceEdgePtr->adjacentVertexBegin();
         iterator != sourceEdgePtr->adjacentVertexEnd(); ++iterator) {
        VertexPtr vertexPtr = *iterator;
        assert(targetEdgePtr->hasAdjacentVertex(vertexPtr));
    }
#endif

    // Copy over all adjacent face references.
    for (AdjacentFaceIterator iterator = sourceEdgePtr->adjacentFaceBegin();
         iterator != sourceEdgePtr->adjacentFaceEnd(); ++iterator) {
        FacePtr facePtr = *iterator;

        // This face must not already be adjacent to the target edge.
        assert(!targetEdgePtr->hasAdjacentFace(facePtr));
        targetEdgePtr->addAdjacentFace(facePtr);

        // Fix the edge references of the adjacent faces.
        assert(!facePtr->hasAdjacentEdge(targetEdgePtr));
        facePtr->insertAdjacentEdge(targetEdgePtr, sourceEdgePtr);
    }

    // Copy over all edge attributes not already present in the target edge.
    MergeAttributePossessorAttributes(*mesh, *sourceEdgePtr, &*targetEdgePtr);

    // Destroy the source edge and remove it from all its adjacent elements.
    DeleteEdge(mesh, sourceEdgePtr, deletedElementTracker);
}

cgmath::BoundingBox3f
GetEdgeBoundingBox(ConstEdgePtr edgePtr)
{
    cgmath::BoundingBox3f boundingBox;

    boundingBox.reset();
    for (AdjacentVertexConstIterator iterator = edgePtr->adjacentVertexBegin();
         iterator != edgePtr->adjacentVertexEnd(); ++iterator) {
        ConstVertexPtr vertexPtr = *iterator;
        boundingBox.extendByVector3f(vertexPtr->position());
    }

    return boundingBox;
}

} // namespace mesh
