// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__VECTOR3F_OPERATIONS__INCLUDED
#define CGMATH__VECTOR3F_OPERATIONS__INCLUDED

#include <cstdlib>

namespace cgmath {

class Vector3f;

// Given a vector, return the zero-based vector component indices of the two primary axes
// that are most orthogonal to it. For example, given the vector 0.9, 0.1, 0.2, the
// indices 1 and 2 are returned. Given the vector 0.1, 0.9, 0.2, the indices
// 2 and 0 are returned.
// The order of the returned indices is such that if the two axes are used to
// define a 2D coordinate space in which a polygon is created, and the vertices of the
// polygon are specified in counterclockwise order in that 2D space,
// then the vertices of the polygon will appear in counterclockwise order
// in 3D space, when viewed down the normal vector, given that the normal vector
// is the input vector to this function.
// The third axis returned is the axis that the input vector is most parallel to.
// The third axis argument is optional.
void GetPrimaryAxesMostOrthogonalToVector(const Vector3f &vector, 
    unsigned *axis1, unsigned *axis2, unsigned *axis3 = NULL);

// Return the longest axis.
unsigned GetLongestAxis(const Vector3f &vector);

// Interpolate between two Vector3f's.
Vector3f InterpolateVector3f(const Vector3f &p0, const Vector3f &p1, float t);

} // namespace cgmath

#endif // CGMATH__VECTOR3F_OPERATIONS__INCLUDED
