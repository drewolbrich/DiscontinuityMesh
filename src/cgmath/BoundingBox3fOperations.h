// Copyright 2009 Retroactive Fiasco.

#ifndef CGMATH__BOUNDING_BOX3F_OPERATIONS__INCLUDED
#define CGMATH__BOUNDING_BOX3F_OPERATIONS__INCLUDED

#include <cstdlib>

namespace cgmath {

class Vector3f;
class BoundingBox3f;

// Returns true if the bounding box intersects the specified bounding box.
bool BoundingBox3fIntersectsBoundingBox3f(const BoundingBox3f &lhs, const BoundingBox3f &rhs);

// Returns true if the bounding box intersects a ray segment.
// If 't' is not NULL, the parameter of the nearest intersection point along the
// ray is returned (t=0 at the ray origin, t=1 at the ray endpoint).
// 't' is set to 0 if the ray origin lies inside the bounding box.
bool BoundingBox3fIntersectsRaySegment(const BoundingBox3f &lhs,
    const Vector3f &origin, const Vector3f &endpoint, float *t = NULL);

// Returns true if a bounding box intersects a plane, defined by a point
// on the plane and its normal. The normal vector must have length 1.
bool BoundingBox3fIntersectsPlane(const BoundingBox3f &bbox, 
    const Vector3f &point, const Vector3f &normal);

// Returns true if a bounding box intersects a triangle.
bool BoundingBox3fIntersectsTriangle(const BoundingBox3f &bbox,
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2);

// Returns true if a bounding box intersects a tetrahedron.
// Note: One of the three cases covered by this test is incomplete,
// so in the current implementation, this is a conservative test.
// Some bounding boxes near the edges of the tetrahedron that do not
// intersect the tetrahedron will cause true to be returned.
bool BoundingBox3fIntersectsTetrahedron(const BoundingBox3f &bbox,
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3);

// Grow a bounding box by the specified absolute and relative tolerance.
BoundingBox3f GrowBoundingBox3fByAbsoluteAndRelativeTolerance(const BoundingBox3f &bbox,
    float absoluteTolerance, float relativeTolerance);

} // namespace cgmath

#endif // CGMATH__BOUNDING_BOX3F_OPERATIONS__INCLUDED
