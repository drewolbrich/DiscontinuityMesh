// Copyright 2008 Retroactive Fiasco.

#include "LineOperations.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector3d.h"
#include "Tolerance.h"

namespace cgmath {

// Return true if the arguments have the same signs.
static bool SameSigns(double a, double b);

bool 
GetClosestPointsOnLines(const Vector3f &p0, const Vector3f &p1,
    const Vector3f &q0, const Vector3f &q1, Vector3f *cp, Vector3f *cq)
{
    // From http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm

    // The input is invalid if either pair of points is coincident,
    // in which case no unique line is defined.
    assert(p0 != p1);
    assert(q0 != q1);

    Vector3d u = Vector3d(p1 - p0);
    Vector3d v = Vector3d(q1 - q0);

    double a = u.dot(u);
    double b = u.dot(v);
    double c = v.dot(v);
    Vector3d w0 = Vector3d(p0 - q0);
    double d = u.dot(w0);
    double e = v.dot(w0);

    double divisor = a*c - b*b;
    if (divisor == 0.0) {
        return false;
    }

    double s = (b*e - c*d)/divisor;
    assert(cp != NULL);
    *cp = Vector3f(Vector3d(p0) + s*u);

    double t = (a*e - b*d)/divisor;
    assert(cq != NULL);
    *cq = Vector3f(Vector3d(q0) + t*v);

    return true;
}

bool 
ClipLineSegmentAgainstPlane(Vector3f *ep0, Vector3f *ep1,
    const Vector3f &p, const Vector3f &n)
{
    // The normal vector must have a length of 1.
    assert(std::abs(1.0 - n.length()) < 0.001);

    Vector3f delta = *ep1 - *ep0;
    double t0 = (*ep0 - p).dot(n);
    double t1 = (*ep1 - p).dot(n);

    if (*ep0 == p) {
        t0 = 0.0;
    }

    if (*ep1 == p) {
        t1 = 0.0;
    }

    // The line segment lies entirely on the back side of the plane.
    // The result of the clipping operation is the empty set.
    if (t0 <= 0.0
        && t1 <= 0.0) {
        return false;
    }

    // Clip the line segment.
    if (t0 < 0.0) {
        assert(ep0 != NULL);
        *ep0 = *ep0 + delta*-t0/(-t0 + t1);
    } else if (t1 < 0.0) {
        assert(ep1 != NULL);
        *ep1 = *ep1 - delta*-t1/(-t1 + t0);
    }

    return true;
}

bool 
GetIntersectionOfLineWithPlane(const Vector3f &p0, const Vector3f &p1,
    const Vector3f &p, const Vector3f &n, Vector3f *ip)
{
    // The normal vector must have a length of 1.
    assert(std::abs(1.0 - n.length()) < 0.001);

    Vector3d p0d(p0);
    Vector3d p1d(p1);
    Vector3d pd(p);
    Vector3d nd(n);

    double divisor = (p1d - p0d).dot(nd);
    if (std::abs(divisor) < TOLERANCE) {
        return false;
    }

    double t = (pd - p0d).dot(nd)/divisor;

    assert(ip != NULL);
    *ip = Vector3f(p0d + t*(p1d - p0d));

    return true;
}

bool 
GetIntersectionOfTwoLines(const Vector2f &p0, const Vector2f &p1,
    const Vector2f &p2, const Vector2f &p3, Vector2f *intersectionPoint)
{
    // Adapted from the function lines_intersect by Mukesh Prasad,
    // in the book Graphics Gems II, pp. 473-476.

    double x1 = p0[0];
    double y1 = p0[1];
    double x2 = p1[0];
    double y2 = p1[1];
    double x3 = p2[0];
    double y3 = p2[1];
    double x4 = p3[0];
    double y4 = p3[1];

    // Compute a1, b1, c1, where line joining points 1 and 2
    // is a1*x + b1*y + c1 = 0.
    double a1 = y2 - y1;
    double b1 = x1 - x2;
    double c1 = x2*y1 - x1*y2;

    double a2 = y4 - y3;
    double b2 = x3 - x4;
    double c2 = x4*y3 - x3*y4;

    // Line segments intersect: compute intersection point. 
    double denom = a1*b2 - a2*b1;
    if (denom == 0.0) {
        // The two line segments are colinear.
        return false;
    }

    if (intersectionPoint != NULL) {
        (*intersectionPoint)[0] = (b1*c2 - b2*c1)/denom;
        (*intersectionPoint)[1] = (a2*c1 - a1*c2)/denom;
    }

    return true;
}

bool
GetIntersectionOfTwoLineSegments(const Vector2f &p0, const Vector2f &p1,
    const Vector2f &p2, const Vector2f &p3, Vector2f *intersectionPoint)
{
    // Adapted from the function lines_intersect by Mukesh Prasad,
    // in the book Graphics Gems II, pp. 473-476.
    
    double x1 = p0[0];
    double y1 = p0[1];
    double x2 = p1[0];
    double y2 = p1[1];
    double x3 = p2[0];
    double y3 = p2[1];
    double x4 = p3[0];
    double y4 = p3[1];

    // Compute a1, b1, c1, where line joining points 1 and 2
    // is a1*x + b1*y + c1 = 0.
    double a1 = y2 - y1;
    double b1 = x1 - x2;
    double c1 = x2*y1 - x1*y2;

    double r3 = a1*x3 + b1*y3 + c1;
    double r4 = a1*x4 + b1*y4 + c1;

    // Check signs of r3 and r4. If both point 3 and point 4 lie on
    // same side of line 1, the line segments do not intersect.
    if (r3 != 0 
        && r4 != 0 
        && SameSigns(r3, r4)) {
        return false;
    }

    double a2 = y4 - y3;
    double b2 = x3 - x4;
    double c2 = x4*y3 - x3*y4;

    double r1 = a2*x1 + b2*y1 + c2;
    double r2 = a2*x2 + b2*y2 + c2;

    // Check signs of r1 and r2. If both point 1 and point 2 lie
    // on same side of second line segment, the line segments do
    // not intersect.
    if (r1 != 0 
        && r2 != 0
        && SameSigns(r1, r2)) {
        return false;
    }

    // Line segments intersect: compute intersection point. 
    double denom = a1*b2 - a2*b1;
    if (denom == 0.0) {
        // The two line segments are colinear.
        return false;
    }

    if (intersectionPoint != NULL) {
        (*intersectionPoint)[0] = (b1*c2 - b2*c1)/denom;
        (*intersectionPoint)[1] = (a2*c1 - a1*c2)/denom;
    }

    return true;
}

void
GetClosestPointOnLineSegment2f(const Vector2f &q0, const Vector2f &q1, const Vector2f &p,
    Vector2f *nearestPoint)
{
    if (q0 == q1) {
        *nearestPoint = q0;
        return;
    }

    float t = (q1 - q0).normalized().dot(p - q0)/(q1 - q0).length();
    if (t <= 0.0) {
        *nearestPoint = q0;
    } else if (t >= 1.0) {
        *nearestPoint = q1;
    } else {
        *nearestPoint = q0*(1.0 - t) + q1*t;
    }
}

void
GetClosestPointOnLineSegment3f(const Vector3f &q0, const Vector3f &q1, const Vector3f &p, 
    Vector3f *nearestPoint)
{
    if (q0 == q1) {
        *nearestPoint = q0;
        return;
    }

    float t = (q1 - q0).normalized().dot(p - q0)/(q1 - q0).length();
    if (t <= 0.0) {
        *nearestPoint = q0;
    } else if (t >= 1.0) {
        *nearestPoint = q1;
    } else {
        *nearestPoint = q0*(1.0 - t) + q1*t;
    }
}

float
GetDistanceFromLineSegmentToPoint2f(const Vector2f &q0, const Vector2f &q1,
    const Vector2f &p)
{
    Vector2f nearestPoint;
    GetClosestPointOnLineSegment2f(q0, q1, p, &nearestPoint);

    return (nearestPoint - p).length();
}

float 
GetDistanceFromLineSegmentToPoint3f(const Vector3f &q0, const Vector3f &q1, const Vector3f &p)
{
    Vector3f nearestPoint;
    GetClosestPointOnLineSegment3f(q0, q1, p, &nearestPoint);

    return (nearestPoint - p).length();
}

static bool 
SameSigns(double a, double b)
{
    if (a < 0.0 && b < 0.0) {
        return true;
    } else if (a > 0.0 && b > 0.0) {
        return true;
    } else if (a == 0.0 && b == 0.0) {
        return true;
    }
    
    return false;
}

} // namespace cgmath
