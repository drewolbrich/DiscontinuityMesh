// Copyright 2008 Drew Olbrich

#include "BoundingBox3f.h"

#include <algorithm>
#include <limits>
#include <cassert>

namespace cgmath {

BoundingBox3f::BoundingBox3f()
    : mMin(),
      mMax()
{
}

BoundingBox3f::~BoundingBox3f()
{
}

BoundingBox3f::BoundingBox3f(float minX, float maxX, float minY, float maxY,
    float minZ, float maxZ)
    : mMin(Vector3f(minX, minY, minZ)),
      mMax(Vector3f(maxX, maxY, maxZ))
{
}

BoundingBox3f::BoundingBox3f(const Vector3f &min, const Vector3f &max)
    : mMin(min),
      mMax(max)
{
}

float 
BoundingBox3f::operator()(int minMax, int axis) const
{
    assert(minMax >= 0 && minMax <= 1);
    assert(axis >= 0 && axis <= 2);

    if (minMax == 0) {
        return mMin[axis];
    } else {
        return mMax[axis];
    }
}

float &
BoundingBox3f::operator()(int minMax, int axis)
{
    assert(minMax >= 0 && minMax <= 1);
    assert(axis >= 0 && axis <= 2);

    if (minMax == 0) {
        return mMin[axis];
    } else {
        return mMax[axis];
    }
}

bool 
BoundingBox3f::operator==(const BoundingBox3f &rhs) const
{
    return mMin == rhs.mMin
        && mMax == rhs.mMax;
}

bool 
BoundingBox3f::operator!=(const BoundingBox3f &rhs) const
{
    return mMin != rhs.mMin
        || mMax != rhs.mMax;
}

float 
BoundingBox3f::minAxis(int axis) const
{
    assert(axis >= 0 && axis <= 2);

    return mMin[axis];
}

float 
BoundingBox3f::maxAxis(int axis) const
{
    assert(axis >= 0 && axis <= 2);

    return mMax[axis];
}

void 
BoundingBox3f::setMinX(float minX)
{
    mMin[0] = minX;
}

float 
BoundingBox3f::minX() const
{
    return mMin[0];
}

void 
BoundingBox3f::setMaxX(float maxX)
{
    mMax[0] = maxX;
}

float 
BoundingBox3f::maxX() const
{
    return mMax[0];
}

void 
BoundingBox3f::setMinY(float minY)
{
    mMin[1] = minY;
}

float 
BoundingBox3f::minY() const
{
    return mMin[1];
}

void 
BoundingBox3f::setMaxY(float maxY)
{
    mMax[1] = maxY;
}

float 
BoundingBox3f::maxY() const
{
    return mMax[1];
}

void 
BoundingBox3f::setMinZ(float minZ)
{
    mMin[2] = minZ;
}

float 
BoundingBox3f::minZ() const
{
    return mMin[2];
}

void 
BoundingBox3f::setMaxZ(float maxZ)
{
    mMax[2] = maxZ;
}

float 
BoundingBox3f::maxZ() const
{
    return mMax[2];
}

void 
BoundingBox3f::setMin(const Vector3f &min)
{
    mMin = min;
}

const Vector3f &
BoundingBox3f::min() const
{
    return mMin;
}

void 
BoundingBox3f::setMax(const Vector3f &max)
{
    mMax = max;
}

const Vector3f &
BoundingBox3f::max() const
{
    return mMax;
}

void 
BoundingBox3f::reset()
{
    *this = EMPTY_SET;
}

void 
BoundingBox3f::extendByVector3f(const Vector3f &point)
{
    if (point[0] < mMin[0]) {
        mMin[0] = point[0];
    }
    if (point[0] > mMax[0]) {
        mMax[0] = point[0];
    }

    if (point[1] < mMin[1]) {
        mMin[1] = point[1];
    }
    if (point[1] > mMax[1]) {
        mMax[1] = point[1];
    }

    if (point[2] < mMin[2]) {
        mMin[2] = point[2];
    }
    if (point[2] > mMax[2]) {
        mMax[2] = point[2];
    }
}

void 
BoundingBox3f::extendByBoundingBox3f(const BoundingBox3f &rhs)
{
    extendByVector3f(rhs.mMin);
    extendByVector3f(rhs.mMax);
}

bool 
BoundingBox3f::empty() const
{
    return mMin[0] > mMax[0]
        && mMin[1] > mMax[1]
        && mMin[2] > mMax[2];
}

float
BoundingBox3f::sizeX() const
{
    return mMax[0] - mMin[0];
}

float
BoundingBox3f::sizeY() const
{
    return mMax[1] - mMin[1];
}

float
BoundingBox3f::sizeZ() const
{
    return mMax[2] - mMin[2];
}

Vector3f
BoundingBox3f::size() const
{
    return mMax - mMin;
}

Vector3f
BoundingBox3f::center() const
{
    return (mMax + mMin)*0.5;
}

const BoundingBox3f BoundingBox3f::EMPTY_SET(
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max());

} // namespace cgmath
