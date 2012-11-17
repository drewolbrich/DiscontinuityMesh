// Copyright 2009 Retroactive Fiasco.

#include "CreateTriangleMesh.h"

#include "TriangleMeshCreator.h"

namespace meshprim {

void
CreateTriangleMesh(mesh::Mesh *mesh,
    size_t vertexCount, float *vertexPositionArray,
    size_t faceCount, int *faceVertexIndexArray,
    std::vector<mesh::VertexPtr> *vertexPtrVector,
    std::vector<mesh::EdgePtr> *edgePtrVector,
    std::vector<mesh::FacePtr> *facePtrVector)
{
    TriangleMeshCreator triangleMeshCreator;

    TriangleMeshCreator::VertexPositionVector vertexPositionVector;
    for (size_t index = 0; index < vertexCount; ++index) {
        vertexPositionVector.push_back(
            cgmath::Vector3f(
                vertexPositionArray[3*index + 0],
                vertexPositionArray[3*index + 1],
                vertexPositionArray[3*index + 2]));
    }

    TriangleMeshCreator::TriangleVector triangleVector;
    for (size_t index = 0; index < faceCount; ++index) {
        triangleVector.push_back(
            TriangleMeshCreator::Triangle(
                faceVertexIndexArray[3*index + 0],
                faceVertexIndexArray[3*index + 1],
                faceVertexIndexArray[3*index + 2]));
    }

    triangleMeshCreator.setMesh(mesh);
    triangleMeshCreator.setVertexPositionVector(&vertexPositionVector);
    triangleMeshCreator.setTriangleVector(&triangleVector);
    triangleMeshCreator.create();

    if (vertexPtrVector != NULL) {
        *vertexPtrVector = triangleMeshCreator.vertexPtrVector();
    }

    if (edgePtrVector != NULL) {
        *edgePtrVector = triangleMeshCreator.edgePtrVector();
    }

    if (facePtrVector != NULL) {
        *facePtrVector = triangleMeshCreator.facePtrVector();
    }
}

} // namespace meshprim
