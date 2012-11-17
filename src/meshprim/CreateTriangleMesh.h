// Copyright 2009 Retroactive Fiasco.

#ifndef MESHPRIM__CREATE_TRIANGLE_MESH__INCLUDED
#define MESHPRIM__CREATE_TRIANGLE_MESH__INCLUDED

#include <vector>

#include <mesh/Types.h>

namespace mesh {
class Mesh;
}

namespace meshprim {

// Wrapper around TriangleMeshCreator, for use in unit tests.

void CreateTriangleMesh(mesh::Mesh *mesh, 
    size_t vertexCount, float *vertexPositionArray, 
    size_t faceCount, int *faceVertexIndexArray, 
    std::vector<mesh::VertexPtr> *vertexPtrVector,
    std::vector<mesh::EdgePtr> *edgePtrVector,
    std::vector<mesh::FacePtr> *facePtrVector);

} // namespace meshprim

#endif // MESHPRIM__CREATE_TRIANGLE_MESH__INCLUDED
