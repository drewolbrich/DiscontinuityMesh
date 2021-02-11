// Copyright 2008 Drew Olbrich
#ifndef DELAUNAY__FACE_OPERATIONS__INCLUDED
#define DELAUNAY__FACE_OPERATIONS__INCLUDED

#include <cgmath/Vector2f.h>

#include "MeshTypes.h"
#include "Mesh.h"

namespace delaunay {

// Rotate the ordering of the vertices and edges within the face one step
// counterclockwise. This is only useful for unit testing, to help ensure that all code
// paths of other mesh operation functions get coverage. Otherwise, it should have
// no effect.
void RotateFace(FacePtr facePtr);

// Split a face into three faces by inserting a new vertex into it at the specified
// position. One new vertex, three edges and two faces are created in the mesh.

// A pointer to the newly created vertex is returned.
VertexPtr SplitFace(FacePtr facePtr, const cgmath::Vector2f &newVertexPosition, MeshPtr meshPtr);

// Delete a face and all adjacent mesh elements (edges and vertices) 
// that are orphaned as a result.
void DeleteFaceAndAllOrphanedAdjacentMeshElements(FacePtr facePtr, MeshPtr meshPtr);

// Delete a face and all adjacent edges that are orphaned as a result
// but that are not marked as constrained edges. Adjacent vertices are not deleted.
void DeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges(FacePtr facePtr, MeshPtr meshPtr);

// Given three noncolinear vertices in counterclockwise order, create the corresponding face
// and any edges that do not yet exist.
FacePtr CreateFaceAndAdjacentEdges(VertexPtr v1, VertexPtr v2, VertexPtr v3, MeshPtr meshPtr);

// Given a face, return the neighboring face across the specified edge.
// The edge must have two adjacent faces.
FacePtr GetNeighboringFaceAcrossEdge(FacePtr facePtr, EdgePtr adjacentEdgePtr);

// Return the vertex opposite the specified edge on a face.
VertexPtr GetVertexOppositeEdge(FacePtr facePtr, EdgePtr edgePtr);

// Return the edge opposite the specified vertex on a face.
EdgePtr GetEdgeOppositeVertex(FacePtr facePtr, VertexPtr vertexPtr);

} // namespace delaunay

#endif // DELAUNAY__FACE_OPERATIONS__INCLUDED
