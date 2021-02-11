// Copyright 2008 Drew Olbrich

#include "GeometricPredicates.h"

#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>

#include "predicates.h"

namespace exact {

bool gInitialized = false;

float 
TestOrientation2d(const cgmath::Vector2f &a, const cgmath::Vector2f &b, 
    const cgmath::Vector2f &c)
{
    if (!gInitialized) {
        exactinit();
        gInitialized = true;
    }

    return orient2d(
        const_cast<float *>(a.asFloatPtr()), 
        const_cast<float *>(b.asFloatPtr()), 
        const_cast<float *>(c.asFloatPtr()));
}

float
TestOrientation3d(const cgmath::Vector3f &a, const cgmath::Vector3f &b,
    const cgmath::Vector3f &c, const cgmath::Vector3f &d)
{
    if (!gInitialized) {
        exactinit();
        gInitialized = true;
    }

    return orient3d(
        const_cast<float *>(a.asFloatPtr()), 
        const_cast<float *>(b.asFloatPtr()), 
        const_cast<float *>(c.asFloatPtr()),
        const_cast<float *>(d.asFloatPtr()));
}

float
TestInCircle(const cgmath::Vector2f &a, const cgmath::Vector2f &b,
    const cgmath::Vector2f &c, const cgmath::Vector2f &d)
{
    if (!gInitialized) {
        exactinit();
        gInitialized = true;
    }

    return incircle(
        const_cast<float *>(a.asFloatPtr()), 
        const_cast<float *>(b.asFloatPtr()), 
        const_cast<float *>(c.asFloatPtr()),
        const_cast<float *>(d.asFloatPtr()));
}

float
TestInSphere(const cgmath::Vector3f &a, const cgmath::Vector3f &b,
    const cgmath::Vector3f &c, const cgmath::Vector3f &d, const cgmath::Vector3f &e)
{
    if (!gInitialized) {
        exactinit();
        gInitialized = true;
    }

    return insphere(
        const_cast<float *>(a.asFloatPtr()), 
        const_cast<float *>(b.asFloatPtr()), 
        const_cast<float *>(c.asFloatPtr()),
        const_cast<float *>(d.asFloatPtr()),
        const_cast<float *>(e.asFloatPtr()));
}

bool
TestLineSegmentIntersectsPoint2f(const cgmath::Vector2f &a, const cgmath::Vector2f &b,
    const cgmath::Vector2f &c)
{
    if (orient2d(
        const_cast<float *>(a.asFloatPtr()), 
        const_cast<float *>(b.asFloatPtr()), 
        const_cast<float *>(c.asFloatPtr())) != 0.0) {
        // Point 'a' does not lie exactly on the line passing through 
        // points 'b' and 'c'.
        return false;
    }

    // Now that we know that point 'a' lies on the line that passes through
    // points 'b' and 'c', we have to check if point 'a' lies on the
    // line segment with endpoints 'b' and 'c'.
    // We can easily do this by testing if 'a' lies in or on the bounding box
    // of the line segment.

    return ((a[0] <= c[0] && c[0] <= b[0]) || (b[0] <= c[0] && c[0] <= a[0]))
        && ((a[1] <= c[1] && c[1] <= b[1]) || (b[1] <= c[1] && c[1] <= a[1]));
}

bool
TestLineIntersectsPoint2f(const cgmath::Vector2f &a, const cgmath::Vector2f &b,
    const cgmath::Vector2f &c)
{
    return orient2d(
        const_cast<float *>(a.asFloatPtr()), 
        const_cast<float *>(b.asFloatPtr()), 
        const_cast<float *>(c.asFloatPtr())) == 0.0;
}

bool
TestLineSegmentsIntersect2f(const cgmath::Vector2f &a1, const cgmath::Vector2f &a2,
    const cgmath::Vector2f &b1, const cgmath::Vector2f &b2)
{
    // Return true if the endpoints of either line segment lie on the
    // other line segment.
    if (TestLineSegmentIntersectsPoint2f(a1, a2, b1)
        || TestLineSegmentIntersectsPoint2f(a1, a2, b2)
        || TestLineSegmentIntersectsPoint2f(b1, b2, a1)
        || TestLineSegmentIntersectsPoint2f(b1, b2, a2)) {
        return true;
    }

    float orientation1 = TestOrientation2d(b1, b2, a1);
    float orientation2 = TestOrientation2d(b1, b2, a2);
    float orientation3 = TestOrientation2d(a1, a2, b1);
    float orientation4 = TestOrientation2d(a1, a2, b2);

    return ((orientation1 < 0.0 && orientation2 > 0.0)
        || (orientation1 > 0.0 && orientation2 < 0.0))
        && ((orientation3 < 0.0 && orientation4 > 0.0)
            || (orientation3 > 0.0 && orientation4 < 0.0));
}

bool
TestLineSegmentsAreColinear2f(const cgmath::Vector2f &a1, const cgmath::Vector2f &a2,
    const cgmath::Vector2f &b1, const cgmath::Vector2f &b2)
{
    return TestLineIntersectsPoint2f(a1, a2, b1)
        && TestLineIntersectsPoint2f(a1, a2, b2)
        && TestLineIntersectsPoint2f(b1, b2, a1)
        && TestLineIntersectsPoint2f(b1, b2, a2);
}

} // namespace exact
