// Copyright 2008 Drew Olbrich

#include "PointOperations.h"

#include "Vector2f.h"
#include "Vector3f.h"

#include <cmath>

namespace cgmath {

float
SignedDistanceFromPointToLine(const Vector2f &point, 
    const Vector2f &line0, const Vector2f &line1)
{
    return ((line0[1] - line1[1])*point[0] + (line1[0] - line0[0])*point[1]
        + (line0[0]*line1[1] - line1[0]*line0[1]))
        /sqrtf((line1[0] - line0[0])*(line1[0] - line0[0])
            + (line1[1] - line0[1])*(line1[1] - line0[1]));
}

} // namespace cgmath
