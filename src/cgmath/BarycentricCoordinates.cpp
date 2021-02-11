// Copyright 2010 Drew Olbrich

#include "BarycentricCoordinates.h"

#include "Vector3fOperations.h"

namespace cgmath {

Vector3f
GetBarycentricCoordinatesOfPointOnTriangle2f(const Vector2f &p,
    const Vector2f &r0, const Vector2f &r1, const Vector2f &r2)
{
    // The following was taken from the Wikipedia page on barycentric coordinates.
    // http://en.wikipedia.org/wiki/Barycentric_coordinates_(mathematics)

    // Convert to the notation on the web page so we don't make any mistakes
    // in the implementation.
    float x = p[0];
    float y = p[1];
    float x1 = r0[0];
    float y1 = r0[1];
    float x2 = r1[0];
    float y2 = r1[1];
    float x3 = r2[0];
    float y3 = r2[1];

    // Matrix T and its determinant.
    float t11 = x1 - x3;
    float t12 = x2 - x3;
    float t21 = y1 - y3;
    float t22 = y2 - y3;
    float detT = t11*t22 - t12*t21;

    float lambda1 = ((y2 - y3)*(x - x3) - (x2 - x3)*(y - y3))/detT;
    float lambda2 = (-(y1 - y3)*(x - x3) + (x1 - x3)*(y - y3))/detT;
    float lambda3 = 1.0 - lambda1 - lambda2;

    return Vector3f(lambda1, lambda2, lambda3);
}

Vector3f
GetBarycentricCoordinatesOfPointOnTriangle3f(const Vector3f &p,
    const Vector3f &r0, const Vector3f &r1, const Vector3f &r2)
{
    // Find the two axes most perpendicular to the triangle's geometric normal.
    // We'll use these axes to convert from 3D to 2D, under the assumption
    // that computing the barycentric coordinates in this 2D space will
    // generate the least floating point error.
    Vector3f normal = (r2 - r0).cross(r1 - r0);
    unsigned longestAxis = GetLongestAxis(normal);
    unsigned axis0 = (longestAxis + 1) % 3;
    unsigned axis1 = (axis0 + 1) % 3;

    // Convert to 2D.
    Vector2f q(p, axis0, axis1);
    Vector2f s0(r0, axis0, axis1);
    Vector2f s1(r1, axis0, axis1);
    Vector2f s2(r2, axis0, axis1);

    return GetBarycentricCoordinatesOfPointOnTriangle2f(q, s0, s1, s2);
}

} // namespace cgmath
