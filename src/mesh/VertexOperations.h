// Copyright 2009 Retroactive Fiasco.

#ifndef MESH__VERTEX_OPERATIONS__INCLUDED
#define MESH__VERTEX_OPERATIONS__INCLUDED

#include "Types.h"

namespace mesh {

class Mesh;
class DeletedElementTracker;

// Destroy a vertex and remove all references to it possessed by adjacent primitives.
void DeleteVertex(Mesh *mesh, VertexPtr vertexPtr,
    DeletedElementTracker *deletedElementTracker = NULL);

// Merge one vertex into another. The adjacent edges and faces of the source vertex are
// transferred to the target vertex. The source vertex is then deleted. 
// Attributes not already present in the target vertex are copied from the source vertex
// to the target vertex. If the two vertices share an adjacent edges, that edge
// will be collapsed, causing other edges and faces to be deleted.
// If the optional deletedElementTracker argument is specified, it will track
// vertex, edges, and faces that are deleted as a result of the vertex merging.
// This function only handles the case of triangular meshes.
void MergeVertexIntoVertex(Mesh *mesh, VertexPtr sourceVertexPtr, 
    VertexPtr targetVertexPtr, DeletedElementTracker *deletedElementTracker = NULL);

// Just like MergeVertexIntoVertex, but only handles the case when the two vertices 
// do not have any adjacent edges or faces in common.
void MergeVertexIntoNonadjacentVertex(Mesh *mesh, VertexPtr sourceVertexPtr, 
    VertexPtr targetVertexPtr, DeletedElementTracker *deletedElementTracker = NULL);

// Returns true if an edge exists that is adjacent to the two vertices.
// The edge is assigned to edgePtr.
bool FindEdgeConnectingVertices(VertexPtr vertexPtr0, VertexPtr vertexPtr1, EdgePtr *edgePtr);

} // namespace mesh

#endif // MESH__VERTEX_OPERATIONS__INCLUDED
