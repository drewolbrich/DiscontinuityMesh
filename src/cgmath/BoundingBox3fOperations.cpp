// Copyright 2009 Retroactive Fiasco.

#include "BoundingBox3fOperations.h"

#include <cmath>
#include <vector>
#include <algorithm>

#include "Vector3f.h"
#include "BoundingBox3f.h"

#include "tribox2.h"

namespace cgmath {

bool
BoundingBox3fIntersectsBoundingBox3f(const BoundingBox3f &lhs, const BoundingBox3f &rhs)
{
    if (std::max(lhs.min()[0], rhs.min()[0]) > std::min(lhs.max()[0], rhs.max()[0])
        || std::max(lhs.min()[1], rhs.min()[1]) > std::min(lhs.max()[1], rhs.max()[1])
        || std::max(lhs.min()[2], rhs.min()[2]) > std::min(lhs.max()[2], rhs.max()[2])) {
        return false;
    }

    return true;
}

bool
BoundingBox3fIntersectsRaySegment(const BoundingBox3f &lhs,
    const Vector3f &origin, const Vector3f &endpoint, float *t)
{
    // This code is based on the article "Fast Ray-Box Intersection",
    // on page 395 of Graphics Gems I.

    const Vector3f direction = endpoint - origin;
    
    enum Quadrant {
        LEFT,
        RIGHT,
        MIDDLE
    };
    
    // For the three axes, find the candidate planes 
    // for intersection. These are the three planes that are
    // closest to the first endpoint of the line.
    Quadrant quadrant[3];
    float candidatePlane[3];
    bool inside = true;
    for (int axis = 0; axis < 3; ++axis) {
        if (origin[axis] < lhs.min()[axis]) {
            quadrant[axis] = LEFT;
            candidatePlane[axis] = lhs.min()[axis];
            inside = false;
        } else if (origin[axis] > lhs.max()[axis]) {
            quadrant[axis] = RIGHT;
            candidatePlane[axis] = lhs.max()[axis];
            inside = false;
        } else {
            quadrant[axis] = MIDDLE;
        }
    }

    // If the first endpoint is inside the box, we're done.
    if (inside) {
        if (t != NULL) {
            *t = 0.0;
        }
        return true;
    }

    // Calculate the t distances to the candidate planes.
    float maxT[3];
    for (int axis = 0; axis < 3; ++axis) {
        if (quadrant[axis] != MIDDLE 
            && direction[axis] != 0.0) {
            maxT[axis] = (candidatePlane[axis] - origin[axis])/direction[axis];
        } else {
            maxT[axis] = -1.0;
        }
    }

    // Get the largest t distance to determine which face
    // of the box is intersected.
    int plane = 0;
    for (int axis = 1; axis < 3; ++axis) {
        if (maxT[axis] > maxT[plane]) {
            plane = axis;
        }
    }

    if (t != NULL) {
        *t = maxT[plane];
    }

    // If the intersection point is past the far endpoint of the
    // ray segment, it doesn't intersect the box.
    if (maxT[plane] > 1.0) {
        return false;
    }
    
    // Test if the intersection point on the plane is 
    // is within the bounds of the box.
    if (maxT[plane] < 0.0) {
        return false;
    }
    for (int axis = 0; axis < 3; ++axis) {
        if (axis != plane) {
            float x = origin[axis] + maxT[plane]*direction[axis];

            // Note: The following statement reflects a correction
            // to the source code in Graphics Gems, Volume I,
            // as listed in the errata for that book.
            if (x < lhs.min()[axis] 
                || x > lhs.max()[axis]) {
                return false;
            }
        }
    }

    // The ray hits the box.
    return true;
}

bool
BoundingBox3fIntersectsPlane(const BoundingBox3f &bbox,
    const Vector3f &point, const Vector3f &normal)
{
    // This code is based on the book "Real Time Collision Detection", p. 164.

    Vector3f c = (bbox.min() + bbox.max())*0.5f;
    Vector3f e = bbox.max() - c;

    // Compute the projection interval radius of b onto L(t) = b.c + t*p.n
    float r = e[0]*fabsf(normal[0]) + e[1]*fabsf(normal[1]) + e[2]*fabsf(normal[2]);

    // Compute the distance of the box center from the plane.
    float s = normal.dot(c - point);

    return fabs(s) <= r;
}

bool
BoundingBox3fIntersectsTriangle(const BoundingBox3f &bbox,
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2)
{
    float triverts[3][3];
    for (unsigned index = 0; index < 3; ++index) {
        triverts[0][index] = v0[index];
        triverts[1][index] = v1[index];
        triverts[2][index] = v2[index];
    }

    cgmath::Vector3f boxCenter = bbox.center();
    cgmath::Vector3f boxHalfSize = bbox.size()*0.5;

    return triBoxOverlap(
        const_cast<float *>(boxCenter.asFloatPtr()),
        const_cast<float *>(boxHalfSize.asFloatPtr()), 
        triverts) != 0;
}

bool
BoundingBox3fIntersectsTetrahedron(const BoundingBox3f &bbox,
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3)
{
    // We transform the problem of testing the intersection of an AABB with a tetrahedron
    // into the equivalent problem of testing the intersection of a point at the center of
    // the AABB with the convex polyhedron formed by sweeping the AABB over the surface of
    // the tetrahedron. This object can be represented as a collection of 3D half-spaces,
    // represented as planes. The point is then known to be inside the volume if it is
    // on the back side of each of the planes.

    const Vector3f vertexArray[4] = { v0, v1, v2, v3 };

    // First, check if the bounding box intersects the bounding box
    // of the tetrahedron, since that is a less expensive test and will usually fail.
    BoundingBox3f tetrahedronBbox = BoundingBox3f::EMPTY_SET;
    for (unsigned vertexIndex = 0; vertexIndex < 4; ++vertexIndex) {
        tetrahedronBbox.extendByVector3f(vertexArray[vertexIndex]);
    }
    if (!BoundingBox3fIntersectsBoundingBox3f(bbox, tetrahedronBbox)) {
        return false;
    }

    const Vector3f bboxCenter = bbox.center();
    const Vector3f halfSize = bbox.size()*0.5f;

    // Test the planes formed by the vertices of the AABB swept across the faces 
    // of the tetrahedron.

    for (unsigned vertexIndex = 0; vertexIndex < 4; ++vertexIndex) {
        // The other three vertices form the plane parallel 
        // to the plane we're interested in.
        const unsigned otherVertexIndex[3] = {
            (vertexIndex + 1) % 4,
            (vertexIndex + 2) % 4,
            (vertexIndex + 3) % 4
        };
        Vector3f normal = (vertexArray[otherVertexIndex[1]]
            - vertexArray[otherVertexIndex[0]]).cross(
                vertexArray[otherVertexIndex[2]]
                - vertexArray[otherVertexIndex[0]]).normalized();
        if (normal.dot(vertexArray[vertexIndex] - vertexArray[otherVertexIndex[0]]) > 0.0f) {
            normal *= -1.0f;
        }
        float maxDistance = 0.0f;
        Vector3f maxDistancePoint;
        for (int x = -1; x <= 1; x += 2) {
            for (int y = -1; y <= 1; y += 2) {
                for (int z = -1; z <= 1; z += 2) {
                    Vector3f point = vertexArray[otherVertexIndex[0]]
                        + halfSize*Vector3f(x, y, z);
                    float distance = (point - vertexArray[vertexIndex]).dot(normal);
                    if (distance > maxDistance) {
                        maxDistance = distance;
                        maxDistancePoint = point;
                    }
                }
            }
        }
        if ((vertexArray[vertexIndex] - maxDistancePoint).dot(normal) <= 0.0f) {
            if ((bboxCenter - maxDistancePoint).dot(normal) > 0.0f) {
                return false;
            }
        }
    }

    // Test the planes formed by the faces of the AABB translated to each
    // vertex of the tetrahedron.

    for (unsigned vertexIndex = 0; vertexIndex < 4; ++vertexIndex) {
        for (unsigned axis = 0; axis < 3; ++axis) {
            for (int side = -1; side <= 1; side += 2) {
                Vector3f normal = Vector3f::ZERO;
                normal[axis] = side;
                Vector3f point = vertexArray[vertexIndex] + normal*halfSize[axis];
                bool isExteriorPlane = true;
                for (unsigned otherVertexIndex = 0; otherVertexIndex < 4; ++otherVertexIndex) {
                    if (otherVertexIndex != vertexIndex
                        && (vertexArray[otherVertexIndex] 
                            - vertexArray[vertexIndex]).dot(normal) > 0.0f) {
                        isExteriorPlane = false;
                        break;
                    }
                }
                if (isExteriorPlane) {
                    if ((bboxCenter - point).dot(normal) > 0.0f) {
                        return false;
                    }
                }
            }
        }
    }

    // Test the planes formed by the edges of the AABB swept along
    // each edge of the tetrahedron.

// The following code is broken. When tested with src/sandbox/box_tetrahedron_test,
// the "return false" statement below was never executed.
#if 0
    // Loop over all pairs of vertices of the tetrahedron.
    for (unsigned tetrahedronVertex0 = 0; tetrahedronVertex0 < 3; ++tetrahedronVertex0) {
        for (unsigned tetrahedronVertex1 = tetrahedronVertex0 + 1; 
             tetrahedronVertex1 < 4; ++tetrahedronVertex1) {

            // The edge of the tetrahedron.
            Vector3f tetrahedronEdge[2] = {
                vertexArray[tetrahedronVertex0],
                vertexArray[tetrahedronVertex1],
            };

            // Get the other two vertices of the tetrahedron that aren't
            // on the edge we're considering.
            unsigned tetrahedronVertex2 = tetrahedronVertex1;
            do {
                tetrahedronVertex2 = (tetrahedronVertex2 + 1) % 4;
            } while (tetrahedronVertex2 == tetrahedronVertex0
                || tetrahedronVertex2 == tetrahedronVertex1);
            unsigned tetrahedronVertex3 = tetrahedronVertex2;
            do {
                tetrahedronVertex3 = (tetrahedronVertex3 + 1) % 4;
            } while (tetrahedronVertex3 == tetrahedronVertex0
                || tetrahedronVertex3 == tetrahedronVertex1);

            Vector3f otherTetrahedronEdge[2] = {
                vertexArray[tetrahedronVertex2],
                vertexArray[tetrahedronVertex3],
            };

            // At this point, we have:
            // tetrahedronEdge - The two endpoints of the line segment on the tetrahedron
            //    we're extruding the AABB along.
            // otherTetrahedronEdge - The two endpoints of the opposing edge
            //    of the tetrahedron.

            // Now we loop over all of the edges of the AABB,
            // and extrude them along the tetrahedron edge, forming planes.
            
            for (unsigned bboxAxis0 = 0; bboxAxis0 < 3; ++bboxAxis0) {
                unsigned bboxAxis1 = (bboxAxis0 + 1) % 3;
                unsigned bboxAxis2 = (bboxAxis0 + 2) % 3;

                // bboxAxis0 is the axis that the AABB edges we're
                // interested in extend across.
                // There are four AABB edges in this direction, for this axis,
                // and we now loop over them.

                for (int a = -1; a <= 1; a += 2) {
                    for (int b = -1; b <= 1; b += 2) {

                        // The edge of the AABB.
                        Vector3f bboxEdge[2] = { halfSize, halfSize };
                        bboxEdge[0][bboxAxis0] *= -1.0f;
                        bboxEdge[0][bboxAxis1] *= a;
                        bboxEdge[0][bboxAxis2] *= b;
                        bboxEdge[1][bboxAxis0] *= 1.0f;
                        bboxEdge[1][bboxAxis1] *= a;
                        bboxEdge[1][bboxAxis2] *= b;

                        // We want a point and normal defining the plane.

                        Vector3f point = tetrahedronEdge[0] + bboxEdge[0];
                        Vector3f normal = (tetrahedronEdge[1] - tetrahedronEdge[0]).cross(
                            (bboxEdge[1] - bboxEdge[0])).normalized();

                        if (normal*Vector3f::ZERO != Vector3f::ZERO) {
                            // The normal is NaN, probably because the
                            // tetrahedron edge and the AABB edge are colinear.
                            continue;
                        }

                        // Flip the normal, if necessary.

                        unsigned frontCount = 0;
                        unsigned backCount = 0;
                        for (unsigned index = 0; index < 2; ++index) {
                            const cgmath::Vector3f p = otherTetrahedronEdge[index] + bboxEdge[0];
                            if ((p - point).dot(normal) >= 0.0f) {
                                ++frontCount;
                            } else {
                                ++backCount;
                            }
                        }

                        if (frontCount == 1) {
                            // Points lie on both sides of the plane,
                            // so the plane cannot lie on the exterior of the volume.
                            continue;
                        } else if (frontCount == 2) {
                            normal *= -1.0f;
                        }

                        frontCount = 0;
                        for (int i = -1; i <= 1; i += 2) {
                            for (int j = -1; j <= 1; j += 2) {
                                for (int k = -1; k <= 1; k += 2) {
                                    const Vector3f p = tetrahedronEdge[0] 
                                        + halfSize*Vector3f(i, j, k);
                                    if (p == bboxEdge[0]
                                        || p == bboxEdge[1]) {
                                        // We don't want to do a comparison against
                                        // the AABB edge endpoints, because we already
                                        // know that they lie on the plane.
                                        continue;
                                    }
                                    if ((p - point).dot(normal) >= 0.0f) {
                                        ++frontCount;
                                        goto skip;
                                    }
                                }
                            }
                        }
                    skip:
                        if (frontCount > 0) {
                            // If any points of the AABB lie in front of the plane,
                            // the plane cannot lie on the exterior of the volume.
                            continue;
                        }

                        if ((bboxCenter - point).dot(normal) > 0.0f) {
                            return false;
                        }
                    }
                }
            }
        }
    }
#endif

    return true;
}

BoundingBox3f
GrowBoundingBox3fByAbsoluteAndRelativeTolerance(const BoundingBox3f &bbox,
    float absoluteTolerance, float relativeTolerance)
{
    Vector3f min = bbox.min();
    Vector3f max = bbox.max();
    for (unsigned axis = 0; axis < 3; ++axis) {
        min[axis] -= std::max(absoluteTolerance, fabsf(min[axis])*relativeTolerance);
        max[axis] += std::max(absoluteTolerance, fabsf(max[axis])*relativeTolerance);
    }

    return BoundingBox3f(min, max);
}

} // namespace cgmath
