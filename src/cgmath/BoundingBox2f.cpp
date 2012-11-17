// Copyright 2008 Retroactive Fiasco.

#include "BoundingBox2f.h"

#include <algorithm>
#include <limits>
#include <cassert>

namespace cgmath {

BoundingBox2f::BoundingBox2f()
    : mMin(),
      mMax()
{
}

BoundingBox2f::~BoundingBox2f()
{
}

BoundingBox2f::BoundingBox2f(float minX, float maxX, float minY, float maxY)
    : mMin(Vector2f(minX, minY)),
      mMax(Vector2f(maxX, maxY))
{
}

BoundingBox2f::BoundingBox2f(const Vector2f &min, const Vector2f &max)
    : mMin(min),
      mMax(max)
{
}

float 
BoundingBox2f::operator()(int minMax, int axis) const
{
    assert(minMax >= 0 && minMax <= 1);
    assert(axis >= 0 && axis <= 1);

    if (minMax == 0) {
        return mMin[axis];
    } else {
        return mMax[axis];
    }
}

float &
BoundingBox2f::operator()(int minMax, int axis)
{
    assert(minMax >= 0 && minMax <= 1);
    assert(axis >= 0 && axis <= 1);

    if (minMax == 0) {
        return mMin[axis];
    } else {
        return mMax[axis];
    }
}

bool 
BoundingBox2f::operator==(const BoundingBox2f &rhs) const
{
    return mMin == rhs.mMin
        && mMax == rhs.mMax;
}

bool 
BoundingBox2f::operator!=(const BoundingBox2f &rhs) const
{
    return mMin != rhs.mMin
        || mMax != rhs.mMax;
}

float 
BoundingBox2f::minAxis(int axis) const
{
    assert(axis >= 0 && axis <= 1);

    return mMin[axis];
}

float 
BoundingBox2f::maxAxis(int axis) const
{
    assert(axis >= 0 && axis <= 1);

    return mMax[axis];
}

void 
BoundingBox2f::setMinX(float minX)
{
    mMin[0] = minX;
}

float 
BoundingBox2f::minX() const
{
    return mMin[0];
}

void 
BoundingBox2f::setMaxX(float maxX)
{
    mMax[0] = maxX;
}

float 
BoundingBox2f::maxX() const
{
    return mMax[0];
}

void 
BoundingBox2f::setMinY(float minY)
{
    mMin[1] = minY;
}

float 
BoundingBox2f::minY() const
{
    return mMin[1];
}

void 
BoundingBox2f::setMaxY(float maxY)
{
    mMax[1] = maxY;
}

float 
BoundingBox2f::maxY() const
{
    return mMax[1];
}

void 
BoundingBox2f::setMin(const Vector2f &min)
{
    mMin = min;
}

const Vector2f &
BoundingBox2f::min() const
{
    return mMin;
}

void 
BoundingBox2f::setMax(const Vector2f &max)
{
    mMax = max;
}

const Vector2f &
BoundingBox2f::max() const
{
    return mMax;
}

void 
BoundingBox2f::reset()
{
    *this = EMPTY_SET;
}

void 
BoundingBox2f::extendByVector2f(const Vector2f &point)
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
}

void
BoundingBox2f::extendByBoundingBox2f(const BoundingBox2f &rhs)
{
    extendByVector2f(rhs.mMin);
    extendByVector2f(rhs.mMax);
}

bool 
BoundingBox2f::empty() const
{
    return mMin[0] > mMax[0]
        && mMin[1] > mMax[1];
}

float
BoundingBox2f::sizeX() const
{
    return mMax[0] - mMin[0];
}

float
BoundingBox2f::sizeY() const
{
    return mMax[1] - mMin[1];
}

Vector2f
BoundingBox2f::size() const
{
    return mMax - mMin;
}

Vector2f
BoundingBox2f::center() const
{
    return (mMax + mMin)*0.5;
}

const BoundingBox2f BoundingBox2f::EMPTY_SET(
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max());

} // namespace cgmath
