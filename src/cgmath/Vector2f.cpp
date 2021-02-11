// Copyright 2008 Drew Olbrich

#include "Vector2f.h"

#include <cassert>
#include <cmath>
#include <algorithm>

#include "Vector3f.h"

namespace cgmath {

Vector2f::Vector2f()
{
}

Vector2f::~Vector2f()
{
}
    
Vector2f::Vector2f(const Vector2f& rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];
}

Vector2f::Vector2f(float c1, float c2) 
{
    mData[0] = c1;
    mData[1] = c2;
}

Vector2f::Vector2f(const Vector3f &rhs, int axis1, int axis2)
{
    assert(axis1 >= 0);
    assert(axis1 < 3);
    assert(axis2 >= 0);
    assert(axis2 < 3);

    mData[0] = rhs[axis1];
    mData[1] = rhs[axis2];
}

Vector2f::Vector2f(const float *components)
{
    mData[0] = components[0];
    mData[1] = components[1];
}

const float *
Vector2f::asFloatPtr() const
{
    return static_cast<const float *>(&mData[0]);
}

Vector2f &
Vector2f::operator=(const Vector2f &rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];

    return *this;
}

void 
Vector2f::swap(Vector2f &rhs)
{
    Vector2f temp(*this);
    *this = rhs;
    rhs = temp;
}

void 
Vector2f::set(float c1, float c2) 
{
    mData[0] = c1;
    mData[1] = c2;
}

float 
Vector2f::operator[](int index) const 
{
    assert(index >= 0);
    assert(index < 2);

    return mData[index];
}

float &
Vector2f::operator[](int index) 
{
    assert(index >= 0);
    assert(index < 2);

    return mData[index];
}

bool 
Vector2f::operator==(const Vector2f &rhs) const 
{
    return mData[0] == rhs.mData[0] 
        && mData[1] == rhs.mData[1];
}

bool 
Vector2f::operator!=(const Vector2f &rhs) const
{
    return mData[0] != rhs.mData[0] 
        || mData[1] != rhs.mData[1];
}

bool 
Vector2f::equivalent(const Vector2f &rhs, float epsilon) const
{
    return std::abs(mData[0] - rhs.mData[0]) <= epsilon
        && std::abs(mData[1] - rhs.mData[1]) <= epsilon;
}

bool 
Vector2f::operator<(const Vector2f &rhs) const
{
    if (mData[0] < rhs.mData[0]) {
        return true;
    } else if (mData[0] > rhs.mData[0]) {
        return false;
    }

    if (mData[1] < rhs.mData[1]) {
        return true;
    } else if (mData[1] > rhs.mData[1]) {
        return false;
    }

    // The two vectors are equal.
    return false;
}

Vector2f 
Vector2f::operator+(const Vector2f &rhs) const 
{
    return Vector2f(
        mData[0] + rhs.mData[0], 
        mData[1] + rhs.mData[1]);
}

Vector2f &
Vector2f::operator+=(const Vector2f &rhs) 
{
    mData[0] += rhs.mData[0];
    mData[1] += rhs.mData[1];

    return *this;
}

Vector2f 
Vector2f::operator-(const Vector2f &rhs) const 
{
    return Vector2f(
        mData[0] - rhs.mData[0], 
        mData[1] - rhs.mData[1]);
}

Vector2f &
Vector2f::operator-=(const Vector2f &rhs) 
{
    mData[0] -= rhs.mData[0];
    mData[1] -= rhs.mData[1];

    return *this;
}

Vector2f 
Vector2f::operator-() const 
{
    return Vector2f(-mData[0], -mData[1]);
}

Vector2f 
Vector2f::operator*(float c) const 
{
    return Vector2f(mData[0]*c, mData[1]*c);
}

Vector2f &
Vector2f::operator*=(float c) 
{
    mData[0] *= c;
    mData[1] *= c;

    return *this;
}

Vector2f 
operator*(float c, const Vector2f &rhs) 
{
    return Vector2f(c*rhs.mData[0], c*rhs.mData[1]);
}

Vector2f 
Vector2f::operator*(const Vector2f &rhs) const
{
    return Vector2f(mData[0]*rhs[0], mData[1]*rhs[1]);
}

Vector2f &
Vector2f::operator*=(const Vector2f &rhs)
{
    mData[0] *= rhs.mData[0];
    mData[1] *= rhs.mData[1];

    return *this;
}

Vector2f 
Vector2f::operator/(float c) const
{
    return Vector2f(mData[0]/c, mData[1]/c);
}

Vector2f &
Vector2f::operator/=(float c) 
{
    mData[0] /= c;
    mData[1] /= c;

    return *this;
}

float
Vector2f::dot(const Vector2f &rhs) const 
{
    return mData[0]*rhs.mData[0] 
        + mData[1]*rhs.mData[1];
}

float 
Vector2f::length() const 
{
    return sqrtf(mData[0]*mData[0] 
        + mData[1]*mData[1]);
}

float 
Vector2f::lengthSquared() const
{
    return mData[0]*mData[0] 
        + mData[1]*mData[1];
}

Vector2f &
Vector2f::normalize() 
{
    *this /= length();

    return *this;
}

Vector2f 
Vector2f::normalized() const
{
    return *this/length();
}

float 
Vector2f::maxAbs() const
{
    return std::max(fabsf(mData[0]), fabsf(mData[1]));
}

std::ostream &
operator<<(std::ostream &ostr, const Vector2f &rhs) 
{
    ostr << rhs.mData[0] 
        << " " << rhs.mData[1];

    return ostr;
}

const Vector2f Vector2f::ZERO(0, 0);

const Vector2f Vector2f::UNIT(1, 1);

} // namespace cgmath
