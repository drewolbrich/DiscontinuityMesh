// Copyright 2008 Drew Olbrich

#ifndef MESHOBJ__CREATE_EDGES_FROM_FACE__INCLUDED
#define MESHOBJ__CREATE_EDGES_FROM_FACE__INCLUDED

#include <mesh/Mesh.h>

namespace meshobj {

// Create all the edges associated with a face. These aren't
// explicitly encoded in the OBJ file, so we have to derive all the
// edges based on the vertex and face tables.
void CreateEdgesFromFace(mesh::Mesh &mesh, mesh::FacePtr facePtr);

} // namespace meshobj

#endif // MESHOBJ__CREATE_EDGES_FROM_FACE__INCLUDED
