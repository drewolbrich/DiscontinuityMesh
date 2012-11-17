// Copyright 2008 Retroactive Fiasco.

#include "CircleOperations.h"

#include <cmath>
#include <cassert>

#include "Vector2f.h"
#include "Constants.h"

namespace cgmath {

bool
CalculateCircleFromThreePoints(const Vector2f &a, const Vector2f &b, const Vector2f &c,
    Vector2f &center, float &radius)
{
    //  a     .     c
    //
    //
    //        b

    // Check if the three points are coincident.
    if (a == b && b == c) {
        center = a;
        radius = 0;

        return true;
    }

    Vector2f ba = b - a;
    Vector2f cb = c - b;
    Vector2f baMidpoint = a + 0.5*ba;
    Vector2f cbMidpoint = b + 0.5*cb;
    Vector2f baLeft = Vector2f(-ba[1], ba[0]);
    Vector2f cbLeft = Vector2f(-cb[1], cb[0]);

    double d = cbLeft[1]*baLeft[0] - cbLeft[0]*baLeft[1];
    if (d == 0.0) {
        // The points are colinear.
        return false;
    }
    double t = (cbLeft[0]*(baMidpoint[1] - cbMidpoint[1]) 
        - cbLeft[1]*(baMidpoint[0] - cbMidpoint[0]))/d;

    center = baMidpoint + t*baLeft;
    radius = (center - a).length();

    return true;
}

Vector2f
MapConcentricSquareToConcentricCircle(const Vector2f &pointOnSquare)
{
    // This function is from the paper "A Low Distortion Map Between Disk and Square",
    // by Peter Shirley and Kenneth Chiu in The Journal of Graphics Tools,
    // Volume 2, Number 3.

    assert(pointOnSquare[0] >= 0.0);
    assert(pointOnSquare[0] <= 1.0);
    assert(pointOnSquare[1] >= 0.0);
    assert(pointOnSquare[1] <= 1.0);

    // (a,b) is now on [-1,1]^2
    float a = 2.0*pointOnSquare[0] - 1.0;
    float b = 2.0*pointOnSquare[1] - 1.0;

    float phi;
    float r;

    if (a > -b) {
        // Region 1 or 2.
        if (a > b) {
            // Region 1, also |a| > |b|.
            r = a;
            phi = (PI/4.0)*(b/a);
        } else {
            // Region 2, also |b| > |a|.
            r = b;
            phi = (PI/4.0)*(2.0 - a/b);
        }
    } else {
        // Region 3 or 4.
        if (a < b) {
            // Region 3, also |a| >= |b|, a != 0.
            r = -a;
            phi = (PI/4.0)*(4.0 + b/a);
        } else {
            // Region 4, |b| >= |a|, but a == 0 and b == 0 could occur.
            r = -b;
            if (b != 0.0) {
                phi = (PI/4.0)*(6.0 - a/b);
            } else {
                phi = 0.0;
            }
        }
    }

    float u = r*cosf(phi);
    float v = r*sinf(phi);

    assert(sqrtf(u*u + v*v) < 1.001);

    return Vector2f(u, v);
}

} // namespace cgmath
