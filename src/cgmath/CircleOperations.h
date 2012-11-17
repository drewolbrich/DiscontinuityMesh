// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__CIRCLE_OPERATIONS__INCLUDED
#define CGMATH__CIRCLE_OPERATIONS__INCLUDED

#include "Vector2f.h"

namespace cgmath {

// Given three points a, b, c on a circle, return the center of the circle and its radius.
// The three points must be specified in counterclockwise order.
// Returns true on success. If the three points are colinear, false is returned.
// If the three points are coincident, true is returned and the circle has a radius
// of zero.
bool CalculateCircleFromThreePoints(const Vector2f &a, const Vector2f &b, const Vector2f &c,
    Vector2f &center, float &radius);

// Map a point on a unit square to a point on a unit circle, with minimal distortion.
// Adjacency and fractional area are preserved.
Vector2f MapConcentricSquareToConcentricCircle(const Vector2f &pointOnSquare);

} // namespace cgmath

#endif // CGMATH__CIRCLE_OPERATIONS__INCLUDED
