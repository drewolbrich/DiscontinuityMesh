// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__VERTEX_OPERATIONS__INCLUDED
#define DELAUNAY__VERTEX_OPERATIONS__INCLUDED

#include "Mesh.h"

namespace delaunay {

// Delete a vertex and all adjacent mesh elements (faces, edges, and vertices)
// that are orphaned as a result.
void DeleteVertexAndAllOrphanedAdjacentMeshElements(VertexPtr vertexPtr, MeshPtr meshPtr);

// Relative to the specified vertex, returns the next vertex around the perimeter
// of the mesh in the specified direction.
enum Direction {
    COUNTERCLOCKWISE,
    CLOCKWISE
};
VertexPtr GetAdjacentVertexAroundPerimeter(VertexPtr vertexPtr, Direction direction);

// Returns true if the given vertex has another vertex as its neighbor via
// an adjacent edge.
bool VertexHasVertexAsNeighbor(VertexPtr vertexPtr, VertexPtr neighboringVertexPtr);

// Returns the edge leading to the specified neighboring vertex, or meshPtr->edgeEnd()
// if no such edge exists.
EdgePtr GetEdgeLeadingToNeighboringVertex(VertexPtr vertexPtr, VertexPtr neighboringVertexPtr,
    MeshPtr meshPtr);

// Returns the vertex neighboring the specifeid vertex along the specified edge.
// The edge must be adjacent to the specified vertex.
VertexPtr GetNeighboringVertexAlongEdge(VertexPtr vertexPtr, EdgePtr adjacentEdgePtr);

} // namespace delaunay

#endif // DELAUNAY__VERTEX_OPERATIONS__INCLUDED
