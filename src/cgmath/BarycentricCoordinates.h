// Copyright 2010 Drew Olbrich

#ifndef CGMATH__BARYCENTRIC_COORDINATES__INCLUDED
#define CGMATH__BARYCENTRIC_COORDINATES__INCLUDED

#include "Vector2f.h"
#include "Vector3f.h"

namespace cgmath {

// Returns the barycentric coordinates of point P on a triangle R0, R1, R2 in 2D space.
Vector3f GetBarycentricCoordinatesOfPointOnTriangle2f(const Vector2f &p,
    const Vector2f &r0, const Vector2f &r1, const Vector2f &r2);

// Returns the barycentric coordinates of point P on a triangle R0, R1, R2 in 3D space.
Vector3f GetBarycentricCoordinatesOfPointOnTriangle3f(const Vector3f &p,
    const Vector3f &r0, const Vector3f &r1, const Vector3f &r2);

} // namespace cgmath

#endif // CGMATH__BARYCENTRIC_COORDINATES__INCLUDED
