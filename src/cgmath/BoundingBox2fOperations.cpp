// Copyright 2009 Retroactive Fiasco.

#include "BoundingBox2fOperations.h"

#include <algorithm>

#include "BoundingBox2f.h"

namespace cgmath {

bool
BoundingBox2fIntersectsBoundingBox2f(const BoundingBox2f &lhs, const BoundingBox2f &rhs)
{
    if (std::max(lhs.min()[0], rhs.min()[0]) > std::min(lhs.max()[0], rhs.max()[0])
        || std::max(lhs.min()[1], rhs.min()[1]) > std::min(lhs.max()[1], rhs.max()[1])) {
        return false;
    }

    return true;
}

} // namespace cgmath
