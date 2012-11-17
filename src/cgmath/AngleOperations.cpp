// Copyright 2009 Retroactive Fiasco.

#include "AngleOperations.h"

#include "Constants.h"

namespace cgmath {

float
DegreesToRadians(float degrees)
{
    return degrees*PI/180.0;
}

float
RadiansToDegrees(float radians)
{
    return radians*180.0/PI;
}

} // namespace cgmath
