// Copyright 2008 Drew Olbrich

#ifndef MESH__EDGE_OPERATIONS__INCLUDED
#define MESH__EDGE_OPERATIONS__INCLUDED

#include <limits>
#include <vector>

#include <cgmath/BoundingBox3f.h>
#include <cgmath/Tolerance.h>

#include "Types.h"

namespace cgmath {
class Vector3f;
}

namespace mesh {

class Mesh;
class DeletedElementTracker;

// Returns the two vertices adjacent to the edge.
// Assumes that the edge has two adjacent vertices.
void GetEdgeAdjacentVertices(ConstEdgePtr edgePtr, VertexPtr *v0, VertexPtr *v1);

// Returns the positions of the two endpoints of the edge.
// Assumes that the edge has two adjacent vertices.
void GetEdgeVertexPositions(ConstEdgePtr edgePtr, cgmath::Vector3f *p0, cgmath::Vector3f *p1);

// Return the length of the edge.
float GetEdgeLength(ConstEdgePtr edgePtr);

// Destroy an edge and remove all references to it possessed by adjacent primitives.
void DeleteEdge(Mesh *mesh, EdgePtr edgePtr,
    DeletedElementTracker *deletedElementTracker = NULL);

// Split an edge at the specified position. The new edge that 
// is created inherits the attributes of the original edge.
// The attributes of the new vertex are interpolated from the
// attributes of the vertices adjacent to the original edge.
// The newly created vertex and edge are returned.
// The newly created edge is adjacent to the input edge's
// first vertex and the new vertex.
// The input edge is modified so that it is adjacent to the new vertex
// and its original second vertex.
void SplitEdge(Mesh *mesh, EdgePtr edgePtr, const cgmath::Vector3f &position, 
    VertexPtr *vertexPtrResult, EdgePtr *edgePtrResult);

// Split an edge in a triangular mesh, splitting adjacent faces if necessary
// to ensure that only triangular faces remain. The newly created vertex is returned.
// Adjacent faces that are destroyed when they are replaced by triangles
// are tracked by the optional DeletedElementTracker.
void SplitEdgeAndAdjacentTriangularFaces(Mesh *mesh, EdgePtr edgePtr, 
    const cgmath::Vector3f &position, VertexPtr *vertexPtrResult, 
    DeletedElementTracker *deletedElementTracker = NULL);

// Collapse an edge to a single vertex. 
// This function only handles the case when all faces adjacent to the edge
// are triangles.
// Attributes not already present in the edge's second vertex are copied 
// from the edge's first vertex.
// The edge's first vertex is deleted. Faces adjacent to the edge will be deleted, 
// and the remaining edges of those faces will be merged, which requires
// deleting edges.
// The remaining vertex of the edge after it is collapsed is returned.
// If the optional deletedElementTracker argument is specified, it will track the
// deleted vertex, as well as possibly deleted faces and edges.
// This function cannot correctly handle the case when the edge's two vertices
// are both adjacent to another edge. Consequently, it is probably almost always
// better to call MergeVertexIntoVertex instead.
VertexPtr CollapseEdge(Mesh *mesh, EdgePtr edgePtr, 
    DeletedElementTracker *deletedElementTracker = NULL);

// Derive an appropriate value of epsilon to use on calculations on an edge.
float GetEpsilonFromEdge(ConstEdgePtr edgePtr, 
    float absoluteTolerance = cgmath::TOLERANCE, float relativeTolerance = cgmath::TOLERANCE);

// Merge together two edges that may or may not have vertices in common.
// The first vertices of one of the edges must not be the second vertices of the other edge.
// The first vertex of the source edge is merged into the first vertex of the target edge.
// The second vertex of the source edge is merged into the second vertex of the target edge.
// The vertices of the source edge, when not already shared by the target edge, are deleted.
// The source edge is deleted.
// Attributes not already present in the target edge are copied from the source edge
// to the target edge.
// If the optional deletedElementTracker argument is specified, it will track deleted
// vertices, edges, and faces.
void MergeEdgeIntoEdge(Mesh *mesh, EdgePtr sourceEdgePtr, EdgePtr targetEdgePtr, 
    DeletedElementTracker *deletedElementTracker = NULL);

// Merge together two edges that already have their adjacent vertices in common.
// The adjacent faces of the source edge are
// transferred to the target face. The source edge is then deleted. 
// Attributes not already present in the target edge are copied from the source edge
// to the target edge.
// If the optional deletedElementTracker argument is specified, it will track the
// deleted edge.
void MergeEdgeIntoEdgeWithSharedVertices(Mesh *mesh, 
    EdgePtr sourceEdgePtr, EdgePtr targetEdgePtr, 
    DeletedElementTracker *deletedElementTracker = NULL);

// Returns the bounding box of an edge.
cgmath::BoundingBox3f GetEdgeBoundingBox(ConstEdgePtr edgePtr);

} // namespace mesh

#endif // MESH__EDGE_OPERATIONS__INCLUDED
