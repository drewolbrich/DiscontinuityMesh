// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__EDGE_OPERATIONS__INCLUDED
#define DELAUNAY__EDGE_OPERATIONS__INCLUDED

#include <cgmath/Vector2f.h>

#include "MeshTypes.h"
#include "Mesh.h"

namespace delaunay {

// Given an edge with two adjacent faces (triangles) that form a convex quadrilateral,
// remove the edge and replace it with one that joins the previously unshared vertices
// of the two triangles.
// That is, given faces with vertices A, B, C and B, C, D (sharing vertices B and C),
// remove the specified edge BC, and replace it with edge AD.
void SwapEdge(EdgePtr edgePtr);

// Split an edge into two edges by inserting a new vertex into it at the specified position.
// One new vertex, three edges, and two faces are created in the mesh.
// A pointer to the newly created vertex is returned.
VertexPtr SplitEdge(EdgePtr edgePtr, const cgmath::Vector2f &newVertexPosition, MeshPtr meshPtr);

// Returns true if the specified vertex is adjacent to the specified edge.
bool EdgeHasAdjacentVertex(EdgePtr edgePtr, VertexPtr vertexPtr);

// Returns true if the two specified vertices are both adjacent to the specified edge.
bool EdgeHasAdjacentVertices(EdgePtr edgePtr, VertexPtr v1, VertexPtr v2);

} // namespace delaunay

#endif // DELAUNAY__EDGE_OPERATIONS__INCLUDED
