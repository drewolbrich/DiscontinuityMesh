// Copyright 2008 Drew Olbrich

#ifndef CGMATH__LINE_OPERATIONS__INCLUDED
#define CGMATH__LINE_OPERATIONS__INCLUDED

namespace cgmath {

class Vector2f;
class Vector3f;

// Given two lines defined by the pairs of points p0,p1 and q0,q1 along the lines,
// return points cp and cq that are the points along the lines where the
// lines are closest to each other.
// Returns false is the lines are parallel.
// The pair of points defining each line must not be coincident.
bool GetClosestPointsOnLines(const Vector3f &p0, const Vector3f &p1,
    const Vector3f &q0, const Vector3f &q1, Vector3f *cp, Vector3f *cq);

// Given the line segment defined by points ep0 and ep1, clip them in place against
// the plane defined by point p and normal n. Vector n must have a length of 1.
// The returned portion of the line segment will lie on the front side of the plane.
// False is returned if the line segment is entirely clipped away because
// points ep0, and ep1 both lie on the back side of the plane.
bool ClipLineSegmentAgainstPlane(Vector3f *ep0, Vector3f *ep1,
    const Vector3f &p, const Vector3f &n);

// Return the point of intersection of a line, defined by points p0, p1, 
// and a plane, defined by point p and normal vector n.
// Returns false if the line is parallel to the plane.
bool GetIntersectionOfLineWithPlane(const Vector3f &p0, const Vector3f &p1,
    const Vector3f &p, const Vector3f &n, Vector3f *ip);

// This function returns true if the pair of 2D lines described by the
// points p1, p2 and p3, p4 intersect. If the argument
// 'intersectionPoint' is not NULL, the point of intersection is stored in
// the Vector2f it points to. If the two line segments are colinear,
// or either line segment is degenerate, this is not considered an intersection.
bool GetIntersectionOfTwoLines(const Vector2f &p0, const Vector2f &p1,
    const Vector2f &p2, const Vector2f &p3, Vector2f *intersectionPoint);

// This function returns true if the pair of 2D line segments described by the
// endpoints p1, p2 and p3, p4 intersect. If the argument
// 'intersectionPoint' is not NULL, the point of intersection is stored in
// the Vector2f it points to. If the two line segments are colinear,
// or either line segment is degenerate, this is not considered an intersection.
bool GetIntersectionOfTwoLineSegments(const Vector2f &p0, const Vector2f &p1,
    const Vector2f &p2, const Vector2f &p3, Vector2f *intersectionPoint);

// Returns the point on 2D line segment q0, q1 closest to point p.
void GetClosestPointOnLineSegment2f(const Vector2f &q0, const Vector2f &q1, const Vector2f &p,
    Vector2f *nearestPoint);

// Returns the point on 3D line segment q0, q1 closest to point p.
void GetClosestPointOnLineSegment3f(const Vector3f &q0, const Vector3f &q1, const Vector3f &p,
    Vector3f *nearestPoint);

// Return the distance from 2D line segment q0, q1 to point p.
float GetDistanceFromLineSegmentToPoint2f(const Vector2f &q0, const Vector2f &q1, 
    const Vector2f &p);

// Return the distance from 3D line segment q0, q1 to point p.
float GetDistanceFromLineSegmentToPoint3f(const Vector3f &q0, const Vector3f &q1, 
    const Vector3f &p);

} // namespace cgmath

#endif // CGMATH__LINE_OPERATIONS__INCLUDED
