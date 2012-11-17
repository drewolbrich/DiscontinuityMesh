// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH_OPERATIONS__INCLUDED
#define DELAUNAY__MESH_OPERATIONS__INCLUDED

#include <cgmath/BoundingBox2f.h>

#include "Mesh.h"

namespace delaunay {

// Compute the bounding box of a mesh by iterating over each of the mesh's vertices.
cgmath::BoundingBox2f GetMeshBoundingBox(MeshPtr MeshPtr);

} // namespace delaunay

#endif // DELAUNAY__MESH_OPERATIONS__INCLUDED
