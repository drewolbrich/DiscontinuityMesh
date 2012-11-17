// Copyright 2009 Retroactive Fiasco.

#ifndef CGMATH__BOUNDING_BOX2F_OPERATIONS__INCLUDED
#define CGMATH__BOUNDING_BOX2F_OPERATIONS__INCLUDED

namespace cgmath {

class BoundingBox2f;

// Returns true if the bounding box intersects
// the specified bounding box.
bool BoundingBox2fIntersectsBoundingBox2f(const BoundingBox2f &lhs, const BoundingBox2f &rhs);

} // namespace cgmath

#endif // CGMATH__BOUNDING_BOX2F_OPERATIONS__INCLUDED
