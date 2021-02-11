// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__MESH_CONSISTENCY__INCLUDED
#define DELAUNAY__MESH_CONSISTENCY__INCLUDED

#include "MeshConsistencyStatus.h"
#include "Mesh.h"

namespace delaunay {

// This function performs several tests ensure that the mesh is consistent.
MeshConsistencyStatus TestMeshConsistency(MeshPtr meshPtr);

// In debug mode only, performs multiple assert calls to ensure that a mesh
// is consistent.
void AssertMeshConsistency(MeshPtr meshPtr);

} // namespace delaunay

#endif // DELAUNAY__MESH_CONSISTENCY__INCLUDED
