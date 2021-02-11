// Copyright 2009 Drew Olbrich

#ifndef CGMATH__VECTOR2F_OPERATIONS__INCLUDED
#define CGMATH__VECTOR2F_OPERATIONS__INCLUDED

namespace cgmath {

class Vector2f;

// Interpolate between two Vector2f's.
Vector2f InterpolateVector2f(const Vector2f &p0, const Vector2f &p1, float t);

} // namespace cgmath

#endif // CGMATH__VECTOR2F_OPERATIONS__INCLUDED
