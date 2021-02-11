// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__MESH_CONNECTED__INCLUDED
#define DELAUNAY__MESH_CONNECTED__INCLUDED

#include "Mesh.h"

namespace delaunay {

// Returns true if the mesh consists of a single connected region, and is not disjoint.
// Also returns true if the mesh has no vertices at all.
bool MeshIsSingleConnectedRegion(MeshPtr meshPtr);

} // namespace delaunay

#endif // DELAUNAY__MESH_CONNECTED__INCLUDED
