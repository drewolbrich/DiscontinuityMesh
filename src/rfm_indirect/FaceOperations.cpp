// Copyright 2010 Drew Olbrich

#include "FaceOperations.h"

#include <cassert>

#include <mesh/Face.h>
#include <mesh/Vertex.h>
#include <mesh/FaceOperations.h>

cgmath::Vector3f
GetFaceVertexSamplePosition(mesh::ConstFacePtr facePtr, mesh::ConstVertexPtr vertexPtr)
{
    mesh::VertexPtr nextVertexPtr;
    mesh::VertexPtr previousVertexPtr;
    GetFaceOtherVertices(facePtr, vertexPtr, &nextVertexPtr, &previousVertexPtr);

    cgmath::Vector3f nextEdgeMidpoint = (vertexPtr->position() 
        + nextVertexPtr->position())*0.5;
    cgmath::Vector3f previousEdgeMidpoint = (vertexPtr->position() 
        + previousVertexPtr->position())*0.5;

    return (vertexPtr->position() + nextEdgeMidpoint + previousEdgeMidpoint)/3.0;
}

void
GetFaceOtherVertices(mesh::ConstFacePtr facePtr, mesh::ConstVertexPtr vertexPtr,
    mesh::VertexPtr *nextVertexPtr, mesh::VertexPtr *previousVertexPtr)
{
    // We're assuming the face is a triangle.
    assert(facePtr->adjacentVertexCount() == 3);

    // Find the adjacent vertices.
    mesh::VertexPtr vertexPtrArray[3];
    mesh::GetTriangularFaceAdjacentVertices(facePtr, 
        &vertexPtrArray[0], &vertexPtrArray[1], &vertexPtrArray[2]);
    int vertexIndex = -1;
    for (int index = 0; index < 3; ++index) {
        if (vertexPtrArray[index] == vertexPtr) {
            vertexIndex = index;
        }
    }
    assert(vertexIndex != -1);

    *nextVertexPtr = vertexPtrArray[(vertexIndex + 1) % 3];
    *previousVertexPtr = vertexPtrArray[(vertexIndex + 2) % 3];
}
