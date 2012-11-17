// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH_EULER_CHARACTERISTIC__INCLUDED
#define DELAUNAY__MESH_EULER_CHARACTERISTIC__INCLUDED

#include "Mesh.h"

namespace delaunay {

// Return the Euler characteristic of a mesh (vertices - edges + faces).
// For a 2D mesh with a single connected region, the Euler characteristic
// will be 1 if the mesh has no holes.
int GetMeshEulerCharacteristic(MeshPtr meshPtr);

} // namespace delaunay

#endif // DELAUNAY__MESH_EULER_CHARACTERISTIC__INCLUDED
