// Copyright 2008 Drew Olbrich

#ifndef CGMATH__PLANE_OPERATIONS__INCLUDED
#define CGMATH__PLANE_OPERATIONS__INCLUDED

#include "Vector3f.h"

namespace cgmath {

// Given a plane defined by a point on the plane and the normal of the plane,
// find the point on the plane closest to a reference point.
// The length of the normal vector must be 1.
bool GetClosestPointOnPlane(const Vector3f &pointOnPlane, const Vector3f &planeNormal,
    const Vector3f &referencePoint, Vector3f *closestPoint);

} // namespace cgmath

#endif // CGMATH__PLANE_OPERATIONS__INCLUDED
