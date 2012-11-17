// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__POINT_OPERATIONS__INCLUDED
#define CGMATH__POINT_OPERATIONS__INCLUDED

namespace cgmath {

class Vector2f;

// Calculate the signed distance from a point to a line in 2D.
// The distance is positive if the three points are specified in
// counterclockwise order, and negative if the three points are specified
// in clockwise order.
float SignedDistanceFromPointToLine(const Vector2f &point, 
    const Vector2f &line0, const Vector2f &line1);

} // namespace cgmath

#endif // CGMATH__POINT_OPERATIONS__INCLUDED
