// Copyright 2009 Retroactive Fiasco.

#include "TriangleOperations.h"

#include <algorithm>
#include <cmath>

#include "Vector3f.h"
#include "Tolerance.h"
#include "LineOperations.h"

namespace cgmath {

float
GetTriangleArea(const Vector3f &p0, const Vector3f &p1, const Vector3f &p2)
{
    return (p1 - p0).cross(p2 - p0).length()*0.5f;
}

bool
GetIntersectionOfTriangleWithCoplanarLineSegment(
    const Vector3f &p0, const Vector3f &p1, const Vector3f &p2,
    const Vector3f &q0, const Vector3f &q1,
    float absoluteTolerance, float relativeTolerance,
    Vector3f *r0, Vector3f *r1,
    int *clippingEdgeIndex0, int *clippingEdgeIndex1)
{
    *r0 = q0;
    *r1 = q1;
    *clippingEdgeIndex0 = -1;
    *clippingEdgeIndex1 = -1;

    float epsilon = absoluteTolerance;
    for (unsigned axis = 0; axis < 3; ++axis) {
        epsilon = std::max(epsilon,
            relativeTolerance*std::max(fabsf(p0[axis]), 
                std::max(fabsf(p1[axis]), fabsf(p2[axis]))));
    }

    // The normal vector of the triangle.
    Vector3f triangleNormal = (p1 - p0).cross(p2 - p0);
    if (triangleNormal.length() < TOLERANCE) {
        // The triangle is degenerate.
        return false;
    }
    triangleNormal.normalize();

    if (fabsf(triangleNormal.dot(q0 - p0)) > epsilon
        || fabsf(triangleNormal.dot(q1 - p0)) > epsilon) {
        // One or more of the endpoints of line segment q0, q1 do not lie sufficiently
        // close to the plane of triangle p0, p1, p2.
        return false;
    }

    // Clip the line segment against the planes passing through
    // the edges of the triangle.
    const Vector3f triangleVertexArray[3] = {
        p0, p1, p2
    };
    for (unsigned pointIndex = 0; pointIndex < 3; ++pointIndex) {
        Vector3f clippingPlaneVertex = triangleVertexArray[pointIndex];
        Vector3f clippingPlaneEdge = triangleVertexArray[(pointIndex + 1) % 3]
            - triangleVertexArray[pointIndex];
        Vector3f clippingPlaneNormal = triangleNormal.cross(clippingPlaneEdge);
        clippingPlaneNormal.normalize();
        cgmath::Vector3f oldPoint0 = *r0;
        cgmath::Vector3f oldPoint1 = *r1;
        if (!cgmath::ClipLineSegmentAgainstPlane(r0, r1, 
                clippingPlaneVertex, clippingPlaneNormal)) {
            // The line segment was completely clipped away,
            // resulting in no line segment.
            return false;
        }
        if (*r0 != oldPoint0) {
            *clippingEdgeIndex0 = pointIndex;
        }
        if (*r1 != oldPoint1) {
            *clippingEdgeIndex1 = pointIndex;
        }
    }

    return true;
}

} // namespace cgmath
