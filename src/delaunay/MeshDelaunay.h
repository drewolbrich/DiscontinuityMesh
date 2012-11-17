// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH_DELAUNAY__INCLUDED
#define DELAUNAY__MESH_DELAUNAY__INCLUDED

#include <cgmath/Vector2f.h>

#include "Mesh.h"

namespace delaunay {

// Returns true if the mesh satisfies the Delaunay criterion.
// If the mesh is not Delaunay, the first detected triangle bounding circle
// that fails the test is returned if the last two arguments are not NULL.
bool MeshIsDelaunay(MeshPtr meshPtr, 
    cgmath::Vector2f *failedTestCircleCenter = NULL, float *failedTestCircleRadius = NULL);

} // namespace delaunay

#endif // DELAUNAY__MESH_DELAUNAY__INCLUDED
