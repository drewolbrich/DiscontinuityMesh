// Copyright 2009 Retroactive Fiasco.

#ifndef MESHRETRI__TRIANGLE_VECTOR__INCLUDED
#define MESHRETRI__TRIANGLE_VECTOR__INCLUDED

#include <vector>

#include <cgmath/Vector3f.h>

namespace meshretri {

// Vector of 3D space triangles.
struct Triangle {
    cgmath::Vector3f mPointArray[3];
};
typedef std::vector<Triangle> TriangleVector;

} // namespace meshretri

#endif // MESHRETRI__TRIANGLE_VECTOR__INCLUDED
