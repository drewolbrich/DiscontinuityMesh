// Copyright 2009 Drew Olbrich

#include "Vector2fOperations.h"

#include "Vector2f.h"

namespace cgmath {

Vector2f
InterpolateVector2f(const Vector2f &p0, const Vector2f &p1, float t)
{
    // The following expression is more numerically stable than p0 + t*(p1 - p0).
    return p0*(1.0 - t) + p1*t;
}

} // namespace cgmath
