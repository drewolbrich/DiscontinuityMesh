// Copyright 2009 Retroactive Fiasco.

#ifndef CGMATH__TRIANGLE_OPERATIONS__INCLUDED
#define CGMATH__TRIANGLE_OPERATIONS__INCLUDED

namespace cgmath {

class Vector3f;

// Compute the area of a triangle defined by three points.
float GetTriangleArea(const Vector3f &p0, const Vector3f &p1, const Vector3f &p2);

// Computes the intersection of triangle p0, p1, p2 and line segment q0, q1,
// given an absolute and relative tolerance (used to test that the line segment
// lies in the plane of the triangle). If line segment lies in the plane of the triangle,
// and intersects the triangle, true is returned, and the line segment of intersection
// is returned as r0, r1.
// clippingEdgeIndex0 and clippingEdgeIndex1 are assigned the indices
// of the triangle edges that clipped the corresponding line segment endpoints.
// Edge index 0 corresponds to triangle points p0, p1. Edge index 1 corresponds
// to triangle points p1, p2.
// clippingEdgeIndex0 and clippingEdgeIndex1 are assigned -1 if the corresponding
// line segment endpoint is not clipped.
bool GetIntersectionOfTriangleWithCoplanarLineSegment(
    const Vector3f &p0, const Vector3f &p1, const Vector3f &p2,
    const Vector3f &q0, const Vector3f &q1, 
    float absoluteTolerance, float relativeTolerance,
    Vector3f *r0, Vector3f *r1, 
    int *clippingEdgeIndex0, int *clippingEdgeIndex1);

} // namespace cgmath

#endif // CGMATH__TRIANGLE_OPERATIONS__INCLUDED
