// Copyright 2009 Retroactive Fiasco.

#ifndef CGMATH__MATRIX4F_OPERATIONS__INCLUDED
#define CGMATH__MATRIX4F_OPERATIONS__INCLUDED

#include <cgmath/Matrix4f.h>
#include <cgmath/Vector3f.h>

namespace cgmath {

// Return a matrix that orients and scales the Y axis of a unix cube centered
// around the origin (from -0.5 to 0.5) from point p0 to point p1.
cgmath::Matrix4f GetPointConnectionMatrix(const cgmath::Vector3f &p0,
    const cgmath::Vector3f &p1);

} // namespace cgmath

#endif // CGMATH__MATRIX4F_OPERATIONS__INCLUDED
