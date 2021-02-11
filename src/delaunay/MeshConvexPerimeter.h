// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__MESH_CONVEX_PERIMETER__INCLUDED
#define DELAUNAY__MESH_CONVEX_PERIMETER__INCLUDED

#include "Mesh.h"

namespace delaunay {

// Returns true if the specified mesh consists of a single connected region
// with no holes and has a convex perimeter.
bool MeshHasConvexPerimeter(MeshPtr meshPtr);

} // namespace delaunay

#endif // DELAUNAY__MESH_CONVEX_PERIMETER__INCLUDED
