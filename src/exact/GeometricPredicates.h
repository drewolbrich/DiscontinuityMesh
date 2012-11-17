// Copyright 2008 Retroactive Fiasco.

#ifndef EXACT__GEOMETRIC_PREDICATES__INCLUDED
#define EXACT__GEOMETRIC_PREDICATES__INCLUDED

namespace cgmath {
class Vector2f;
class Vector3f;
}

namespace exact {

// The following functions use exact arithmetic as described in Jonathan Richard
// Shewchuk's paper, "Adaptive Precision Floating-Point Arithmetic and Fast Robust
// Geometric Predicates", to robustly calculate various geometric predicates. Correct
// results should always be returned, even in extreme cases where floating point roundoff
// error would cause a naive implemention to return the wrong answer. See the file
// predicates.c for more information.

// Returns a positive value if points A, B, and C are specified in counterclockwise order,
// a negative value if they are specified in clockwise order, and zero if they are
// colinear. This function also returns zero if any two input points are coincident.
float TestOrientation2d(const cgmath::Vector2f &a, const cgmath::Vector2f &b, 
    const cgmath::Vector2f &c);

// Returns a positive value if point D lies below the plane passing through points A, B,
// and C, with "below" defined such that points A, B, and C appear in a counterclockwise
// order when viewed from above the plane. Returns a negative value if D lies above the
// plane. Returns zero if the points are coplanar.
float TestOrientation3d(const cgmath::Vector3f &a, const cgmath::Vector3f &b, 
    const cgmath::Vector3f &c, const cgmath::Vector3f &d);

// Returns a positive value if point D lies inside the circle passing through points A, B,
// and C. Returns a negative value if it lies outside the circle. Returns zero if the four
// points are cocircular. The points A, B, and C must be in counterclockwise order, or the
// sign of the result will be reversed.
float TestInCircle(const cgmath::Vector2f &a, const cgmath::Vector2f &b, 
    const cgmath::Vector2f &c, const cgmath::Vector2f &d);

// Returns a positive value if point E lies inside the sphere passing through points A, B,
// C, and D. Returns a negative value if it lies outside the sphere. Returns zero if the
// five points are cospherical. The points A, B, C, and D must be ordered such that point
// D lies below the plane passing through points A, B, and C, with "below" defined such
// that points A, B, and C appear in a counterclockwise order when viewed from above the
// plane.
float TestInSphere(const cgmath::Vector3f &a, const cgmath::Vector3f &b, 
    const cgmath::Vector3f &c, const cgmath::Vector3f &d, const cgmath::Vector3f &e);

// Returns true if point C lies exactly on the line segment defined by points A and B.
bool TestLineSegmentIntersectsPoint2f(const cgmath::Vector2f &a, const cgmath::Vector2f &b, 
    const cgmath::Vector2f &c);

// Returns true if point C lies exactly on the line passing through points A and B.
bool TestLineIntersectsPoint2f(const cgmath::Vector2f &a, const cgmath::Vector2f &b, 
    const cgmath::Vector2f &c);

// Returns true if the line segment A1 A2 intersects line segment B1 B2.
// True is also returned if the line segments have endpoints in common.
bool TestLineSegmentsIntersect2f(const cgmath::Vector2f &a1, const cgmath::Vector2f &a2,
    const cgmath::Vector2f &b1, const cgmath::Vector2f &b2);

// Returns true if line segment A1 A2 is colinear with line segment B1 B2.
// This will also be true if either line segment is degenerate
// and coincident with the other line segment.
bool TestLineSegmentsAreColinear2f(const cgmath::Vector2f &a1, const cgmath::Vector2f &a2,
    const cgmath::Vector2f &b1, const cgmath::Vector2f &b2);

} // namespace exact

#endif // EXACT__GEOMETRIC_PREDICATES__INCLUDED
