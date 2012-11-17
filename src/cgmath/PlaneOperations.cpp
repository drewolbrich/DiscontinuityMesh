// Copyright 2008 Retroactive Fiasco.

#include "PlaneOperations.h"

#include <cassert>
#include <cmath>

namespace cgmath {

bool
GetClosestPointOnPlane(const Vector3f &pointOnPlane, const Vector3f &planeNormal,
    const Vector3f &referencePoint, Vector3f *closestPoint)
{
    // The length of the normal must be 1.
    assert(fabsf(planeNormal.length() - 1.0) < 0.001);

    *closestPoint = referencePoint - planeNormal*(referencePoint - pointOnPlane).dot(planeNormal);

    return true;
}

} // namespace cgmath
