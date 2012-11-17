// Copyright 2008 Retroactive Fiasco.

#include "Vector3fOperations.h"

#include <cassert>
#include <cmath>

#include "Vector3f.h"

namespace cgmath {

void
GetPrimaryAxesMostOrthogonalToVector(const Vector3f &vector, 
    unsigned *axis1, unsigned *axis2, unsigned *axis3)
{
    if (fabsf(vector[0]) > fabsf(vector[1])
        && fabsf(vector[0]) > fabsf(vector[2])) {
        if (vector[0] > 0.0) {
            *axis1 = 1;
            *axis2 = 2;
        } else {
            *axis1 = 2;
            *axis2 = 1;
        }
        if (axis3 != NULL) {
            *axis3 = 0;
        }
    } else if (fabsf(vector[1]) > fabsf(vector[0])
        && fabsf(vector[1]) > fabsf(vector[2])) {
        if (vector[1] > 0.0) {
            *axis1 = 2;
            *axis2 = 0;
        } else {
            *axis1 = 0;
            *axis2 = 2;
        }
        if (axis3 != NULL) {
            *axis3 = 1;
        }
    } else {
        assert(fabsf(vector[2]) >= fabsf(vector[0])
            && fabsf(vector[2]) >= fabsf(vector[1]));
        if (vector[2] > 0.0) {
            *axis1 = 0;
            *axis2 = 1;
        } else {
            *axis1 = 1;
            *axis2 = 0;
        }
        if (axis3 != NULL) {
            *axis3 = 2;
        }
    }
}

unsigned
GetLongestAxis(const Vector3f &vector)
{
    if (fabsf(vector[0]) >= fabsf(vector[1])
        && fabsf(vector[0]) >= fabsf(vector[2])) {
        return 0;
    } else if (fabsf(vector[1]) >= fabsf(vector[0])
        && fabsf(vector[1]) >= fabsf(vector[2])) {
        return 1;
    }

    assert(fabsf(vector[2]) >= fabsf(vector[0])
        && fabsf(vector[2]) >= fabsf(vector[1]));

    return 2;
}

Vector3f
InterpolateVector3f(const Vector3f &p0, const Vector3f &p1, float t)
{
    // The following expression is more numerically stable than p0 + t*(p1 - p0).
    return p0*(1.0 - t) + p1*t;
}

} // namespace cgmath
