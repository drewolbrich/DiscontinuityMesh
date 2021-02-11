// Copyright 2009 Drew Olbrich

#include "Matrix4fOperations.h"

#include <cmath>
#include <iostream>

namespace cgmath {

cgmath::Matrix4f
GetPointConnectionMatrix(const cgmath::Vector3f &p0, const cgmath::Vector3f &p1)
{
    cgmath::Vector3f xAxis;
    cgmath::Vector3f yAxis;
    cgmath::Vector3f zAxis;

    yAxis = p1 - p0;

    static const cgmath::Vector3f UNIT_X(1.0, 0.0, 0.0);
    static const cgmath::Vector3f UNIT_Z(0.0, 0.0, 1.0);

    if (fabs(yAxis.dot(UNIT_X)) < fabs(yAxis.dot(UNIT_Z))) {
        xAxis = yAxis.cross(UNIT_X).normalized();
    } else {
        xAxis = yAxis.cross(UNIT_Z).normalized();
    }

    zAxis = xAxis.cross(yAxis).normalized();

    return cgmath::Matrix4f(xAxis, yAxis, zAxis, p0)
        *cgmath::Matrix4f::fromTranslation(cgmath::Vector3f(0.0, 0.5, 0.0));
}

} // namespace cgmath
