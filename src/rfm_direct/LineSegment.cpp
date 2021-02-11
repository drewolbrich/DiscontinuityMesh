// Copyright 2008 Drew Olbrich

#include "LineSegment.h"

#include <cassert>
#include <iostream>
#include <algorithm>

#include <exact/GeometricPredicates.h>

LineSegment::LineSegment()
    : mPoint0(),
      mPoint1(),
      mFacePtr(),
      mIntersectionVector(),
      mWasReordered(false),
      mIsAboveOccluder(false)
{
}

LineSegment::~LineSegment()
{
}

void
LineSegment::setPoint0(const Endpoint &point0)
{
    mPoint0 = point0;
}

const Endpoint &
LineSegment::point0() const
{
    return mPoint0;
}

void
LineSegment::setPoint1(const Endpoint &point1)
{
    mPoint1 = point1;
}

const Endpoint &
LineSegment::point1() const
{
    return mPoint1;
}

void
LineSegment::setFacePtr(const mesh::FacePtr &facePtr)
{
    mFacePtr = facePtr;
}

const mesh::FacePtr &
LineSegment::facePtr() const
{
    return mFacePtr;
}

void
LineSegment::addIntersection(const Intersection &intersection)
{
    mIntersectionVector.push_back(intersection);
}

void
LineSegment::sortIntersectionVector()
{
    std::sort(mIntersectionVector.begin(), mIntersectionVector.end());
}

const LineSegment::IntersectionVector &
LineSegment::intersectionVector() const
{
    return mIntersectionVector;
}

bool
LineSegment::operator<(const LineSegment &rhs) const
{
    const cgmath::Vector2f &p0 = mPoint0.wedgePosition();
    const cgmath::Vector2f &p1 = mPoint1.wedgePosition();
    const cgmath::Vector2f &q0 = rhs.mPoint0.wedgePosition();
    const cgmath::Vector2f &q1 = rhs.mPoint1.wedgePosition();

    // The minimum Y values (those farthest away from the light source)
    // are compared, such that when this operator is used for sorting,
    // the line segment farthest away from the light source
    // (with the minimum Y value) will appear first in the list.
    return std::min(p0[1], p1[1]) < std::min(q0[1], q1[1]);
}

bool
LineSegment::potentiallyOccludes(const LineSegment &rhs) const
{
    const cgmath::Vector2f &p0 = mPoint0.wedgePosition();
    const cgmath::Vector2f &p1 = mPoint1.wedgePosition();
    const cgmath::Vector2f &q0 = rhs.mPoint0.wedgePosition();
    const cgmath::Vector2f &q1 = rhs.mPoint1.wedgePosition();
 
    return std::max(p0[1], p1[1]) > std::min(q0[1], q1[1]);
}

bool 
LineSegment::occludes(const LineSegment &rhs) const
{
    const cgmath::Vector2f &p0 = mPoint0.wedgePosition();
    const cgmath::Vector2f &p1 = mPoint1.wedgePosition();
    const cgmath::Vector2f &q0 = rhs.mPoint0.wedgePosition();
    const cgmath::Vector2f &q1 = rhs.mPoint1.wedgePosition();

    // Returns true if P (LHS) occludes Q (RHS) from the point of view
    // of the light source. The focal point of the light source is point V (the origin).

#if 0
    // If the rest of Newell's Algorithm is working correctly, we must already have
    // established that there is some overlap along the Y axis between P and Q.
    assert(std::max(p0[1], p1[1]) >= std::min(q0[1], q1[1]));
    assert(std::min(p0[1], p1[1]) <= std::max(q0[1], q1[1]));
#endif

#if 0
    // P and Q must both be on one side of V or the other.
    // Clipping in WedgeIntersector will split any line segments
    // which straddle V.
    assert((p0[1] <= 0.0 && p1[1] <= 0.0 && q0[1] <= 0.0 && q1[1] <= 0.0)
        || (p0[1] >= 0.0 && p1[1] >= 0.0 && q0[1] >= 0.0 && q1[1] >= 0.0));
#endif

    // We're assuming that neither line segment is degenerate.
    assert(p0 != p1);
    assert(q0 != q1);

    // I've disabled this check, because it'll get triggered if
    // input polygons are coplanar.
#if 0
    // We're assuming that either the line segments share endpoints, or the line segments
    // do not intersect. We've already clipped the line segments against each other.
    assert(p0 == q0 || p0 == q1 || p1 == q0 || p1 == q1
        || !exact::TestLineSegmentsIntersect(p0, p1, q0, q1));
#endif

    // If P and Q are both above V, some of the comparison logic must be
    // inverted.
    bool aboveV = p0[1] >= 0.0 && p1[1] >= 0.0 && q0[1] >= 0.0 && q1[1] >= 0.0;

    const cgmath::Vector2f v = cgmath::Vector2f::ZERO;

    // If both P and Q have endpoints that are coincident with V,
    // by definition neither one can occlude the other.
    if ((p0 == v || p1 == v)
        && (q0 == v || q1 == v)) {
        // P does not occlude Q.
        return false;
    }

    // Only bother testing the projections of the line segments if none of their
    // endpoints are coincident with V. If any of the endpoints are
    // coincident with V, then by definition the projections intersect.
    // (This way, we avoid dividing by zero.)
    if (p0 != v || p1 != v || q0 != v || q1 != v) {
        const float p0x = p0[0]/p0[1];
        const float p1x = p1[0]/p1[1];
        const float q0x = q0[0]/q0[1];
        const float q1x = q1[0]/q1[1];
        if (std::min(p0x, p1x) >= std::max(q0x, q1x)) {
            // The projected line segments do not intersect,
            // so P does not occlude Q.
            return false;
        }
        if (std::min(q0x, q1x) >= std::max(p0x, p1x)) {
            // The projected line segments do not intersect,
            // so P does not occlude Q.
            return false;
        }
    }

    float orientationP0 = exact::TestOrientation2d(q0, q1, p0);
    float orientationP1 = exact::TestOrientation2d(q0, q1, p1);
    if (exact::TestOrientation2d(q0, q1, v) > 0.0
        // The following expression is equivalent to "xor aboveV".
        != aboveV) {
        if (orientationP0 <= 0.0
            && orientationP1 <= 0.0) {
            // P is is entirely in the back half space of Q
            // with respect to the light source,
            // so P does not occlude Q.
            return false;
        }
    } else {
        if (orientationP0 >= 0.0
            && orientationP1 >= 0.0) {
            // P is is entirely in the back half space of Q
            // with respect to the light source,
            // so P does not occlude Q.
            return false;
        }
    }

    float orientationQ0 = exact::TestOrientation2d(p0, p1, q0);
    float orientationQ1 = exact::TestOrientation2d(p0, p1, q1);
    if (exact::TestOrientation2d(p0, p1, v) > 0.0
        // The following expression is equivalent to "xor aboveV".
        != aboveV) {
        if (orientationQ0 >= 0.0
            && orientationQ1 >= 0.0) {
            // Q is entirely in the front half space of P
            // with respect to the light source,
            // so P does not occlude Q.
            return false;
        }
    } else {
        if (orientationQ0 <= 0.0
            && orientationQ1 <= 0.0) {
            // Q is entirely in the front half space of P
            // with respect to the light source,
            // so P does not occlude Q.
            return false;
        }
    }

    // P occludes Q.
    return true;
}

bool
LineSegment::occludesSlightlyIntersecting(const LineSegment &rhs) const
{
    const cgmath::Vector2f p0 = rhs.point0().wedgePosition();

    cgmath::Vector2f dx = rhs.point1().wedgePosition() - p0;

    cgmath::Vector2f normal(dx[1], -dx[0]);

    cgmath::Vector2f midpoint = (point0().wedgePosition() + point1().wedgePosition())*0.5;

    float dot = (midpoint - p0).dot(normal);

    if (normal[1] > 0.0) {
        return dot > 0.0;
    } else {
        return dot < 0.0;
    }
}

void
LineSegment::setWasReordered(bool wasReordered)
{
    mWasReordered = wasReordered;
}

bool
LineSegment::wasReordered() const
{
    return mWasReordered;
}

void
LineSegment::setIsAboveOccluder(bool isAboveOccluder)
{
    mIsAboveOccluder = isAboveOccluder;
}

bool
LineSegment::isAboveOccluder() const
{
    return mIsAboveOccluder;
}
