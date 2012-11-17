// Copyright 2010 Retroactive Fiasco.

#ifndef RFM_INDIRECT__FACE_OPERATIONS__INCLUDED
#define RFM_INDIRECT__FACE_OPERATIONS__INCLUDED

#include <cgmath/Vector3f.h>
#include <mesh/Types.h>

// Return the sample position for a given face vertex.
cgmath::Vector3f GetFaceVertexSamplePosition(mesh::ConstFacePtr facePtr, 
    mesh::ConstVertexPtr vertexPtr);

// Given the vertex of a triangular face, return the other two vertices.
void GetFaceOtherVertices(mesh::ConstFacePtr facePtr, mesh::ConstVertexPtr vertexPtr,
    mesh::VertexPtr *nextVertexPtr, mesh::VertexPtr *previousVertexPtr);

#endif // RFM_INDIRECT__FACE_OPERATIONS__INCLUDED
