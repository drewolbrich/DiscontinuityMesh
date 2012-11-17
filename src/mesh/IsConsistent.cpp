// Copyright 2008 Retroactive Fiasco.

#include "IsConsistent.h"

#include <cassert>
#include <set>

#include "Mesh.h"

namespace mesh {

bool
IsConsistent(const Mesh &mesh)
{
    // Loop over all the vertices in the mesh.
    for (ConstVertexPtr vertexPtr = mesh.vertexBegin(); vertexPtr != mesh.vertexEnd(); 
         ++vertexPtr) {

        std::set<ConstEdgePtr> edgePtrMap;
        for (AdjacentEdgeConstIterator iterator = vertexPtr->adjacentEdgeBegin();
             iterator != vertexPtr->adjacentEdgeEnd(); ++iterator) {
            ConstEdgePtr edgePtr = *iterator;
            // The edges adjacent to this vertex must contain
            // a reference back to this vertex.
            if (!edgePtr->hasAdjacentVertex(vertexPtr)) {
                return false;
            }

            // The vertex must not have duplicate adjacent edges.
            if (edgePtrMap.find(edgePtr) != edgePtrMap.end()) {
                return false;
            }
            edgePtrMap.insert(edgePtr);
        }

        std::set<ConstFacePtr> facePtrMap;
        for (AdjacentFaceConstIterator iterator = vertexPtr->adjacentFaceBegin();
             iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
            ConstFacePtr facePtr = *iterator;
            // The faces adjacent to this vertex must contain
            // a reference back to this vertex.
            if (!facePtr->hasAdjacentVertex(vertexPtr)) {
                return false;
            }

            // The vertex must not have duplicate adjacent faces.
            if (facePtrMap.find(facePtr) != facePtrMap.end()) {
                return false;
            }
            facePtrMap.insert(facePtr);
        }
    }

    // Loop over all the edges in the mesh.
    for (ConstEdgePtr edgePtr = mesh.edgeBegin(); edgePtr != mesh.edgeEnd(); ++edgePtr) {

        if (edgePtr->adjacentVertexCount() < 2) {
            return false;
        }

        std::set<ConstVertexPtr> vertexPtrMap;
        for (AdjacentVertexConstIterator iterator = edgePtr->adjacentVertexBegin();
             iterator != edgePtr->adjacentVertexEnd(); ++iterator) {
            ConstVertexPtr vertexPtr = *iterator;
            // The vertices adjacent to this edge must contain
            // a reference back to this edge.
            if (!vertexPtr->hasAdjacentEdge(edgePtr)) {
                return false;
            }

            // The edge must not have duplicate adjacent vertices.
            if (vertexPtrMap.find(vertexPtr) != vertexPtrMap.end()) {
                return false;
            }
            vertexPtrMap.insert(vertexPtr);
        }

        std::set<ConstFacePtr> facePtrMap;
        for (AdjacentFaceConstIterator iterator = edgePtr->adjacentFaceBegin();
             iterator != edgePtr->adjacentFaceEnd(); ++iterator) {
            ConstFacePtr facePtr = *iterator;
            // The faces adjacent to this edge must contain
            // a reference back to this edge.
            if (!facePtr->hasAdjacentEdge(edgePtr)) {
                return false;
            }

            // The edge must not have duplicate adjacent faces.
            if (facePtrMap.find(facePtr) != facePtrMap.end()) {
                return false;
            }
            facePtrMap.insert(facePtr);
        }
    }

    // Loop over all the faces in the mesh.
    for (ConstFacePtr facePtr = mesh.faceBegin(); facePtr != mesh.faceEnd(); ++facePtr) {

        if (facePtr->adjacentVertexCount() < 3) {
            return false;
        }

        if (facePtr->adjacentEdgeCount() < 3) {
            return false;
        }

        if (facePtr->adjacentVertexCount() != facePtr->adjacentEdgeCount()) {
            return false;
        }

        std::set<ConstVertexPtr> vertexPtrMap;
        for (AdjacentVertexConstIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            ConstVertexPtr vertexPtr = *iterator;
            // The vertices adjacent to this face must contain
            // a reference back to this face.
            if (!vertexPtr->hasAdjacentFace(facePtr)) {
                return false;
            }

            // The face must not have duplicate adjacent vertices.
            if (vertexPtrMap.find(vertexPtr) != vertexPtrMap.end()) {
                return false;
            }
            vertexPtrMap.insert(vertexPtr);
        }

        std::set<ConstEdgePtr> edgePtrMap;
        for (AdjacentEdgeConstIterator iterator = facePtr->adjacentEdgeBegin();
             iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
            ConstEdgePtr edgePtr = *iterator;
            // The edges adjacent to this face must contain
            // a reference back to this face.
            if (!edgePtr->hasAdjacentFace(facePtr)) {
                return false;
            }

            // The face must not have duplicate adjacent edges.
            if (edgePtrMap.find(edgePtr) != edgePtrMap.end()) {
                return false;
            }
            edgePtrMap.insert(edgePtr);
        }

        // Make sure all of the face vertices reference vertices
        // that are adjacent to the face.
        // If this isn't the case, we probably have dangling pointers.
        for (Face::FaceVertexVectorConstIterator iterator = facePtr->faceVertexVectorBegin();
             iterator != facePtr->faceVertexVectorEnd(); ++iterator){
            const FaceVertex *faceVertexPtr = *iterator;
            if (!facePtr->hasAdjacentVertex(faceVertexPtr->vertexPtr())) {
                return false;
            }
        }
    }

    return true;
}

} // namespace mesh
