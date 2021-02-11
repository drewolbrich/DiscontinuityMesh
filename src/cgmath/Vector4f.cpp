// Copyright 2008 Drew Olbrich

#include "Vector4f.h"

#include <cassert>
#include <cmath>
#include <algorithm>

#include "Vector3f.h"

namespace cgmath {

Vector4f::Vector4f()
{
}
 
Vector4f::~Vector4f()
{
}
 
Vector4f::Vector4f(const Vector4f &rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];
    mData[2] = rhs.mData[2];
    mData[3] = rhs.mData[3];
}

Vector4f::Vector4f(const Vector3f &rhs) 
{
    mData[0] = rhs[0];
    mData[1] = rhs[1];
    mData[2] = rhs[2];
    mData[3] = 1.0;
}

Vector4f::Vector4f(float c1, float c2, float c3, float c4)
{
    mData[0] = c1;
    mData[1] = c2;
    mData[2] = c3;
    mData[3] = c4;
}

Vector4f::Vector4f(const float *components)
{
    mData[0] = components[0];
    mData[1] = components[1];
    mData[2] = components[2];
    mData[3] = components[3];
}

const float *
Vector4f::asFloatPtr() const
{
    return static_cast<const float *>(&mData[0]);
}

Vector4f &
Vector4f::operator=(const Vector4f &rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];
    mData[2] = rhs.mData[2];
    mData[3] = rhs.mData[3];

    return *this;
}

void 
Vector4f::swap(Vector4f &rhs)
{
    Vector4f temp(*this);
    *this = rhs;
    rhs = temp;
}

void 
Vector4f::set(float c1, float c2, float c3, float c4) 
{
    mData[0] = c1;
    mData[1] = c2;
    mData[2] = c3;
    mData[3] = c4;
}

float 
Vector4f::operator[](int index) const 
{
    assert(index >= 0);
    assert(index < 4);

    return mData[index];
}

float &
Vector4f::operator[](int index) 
{
    assert(index >= 0);
    assert(index < 4);

    return mData[index];
}

bool 
Vector4f::operator==(const Vector4f &rhs) const 
{
    return mData[0] == rhs.mData[0] 
        && mData[1] == rhs.mData[1]
        && mData[2] == rhs.mData[2]
        && mData[3] == rhs.mData[3];
}

bool 
Vector4f::operator!=(const Vector4f &rhs) const
{
    return mData[0] != rhs.mData[0] 
        || mData[1] != rhs.mData[1]
        || mData[2] != rhs.mData[2]
        || mData[3] != rhs.mData[3];
}

bool 
Vector4f::equivalent(const Vector4f &rhs, float epsilon) const
{
    return std::abs(mData[0] - rhs.mData[0]) <= epsilon
        && std::abs(mData[1] - rhs.mData[1]) <= epsilon
        && std::abs(mData[2] - rhs.mData[2]) <= epsilon
        && std::abs(mData[3] - rhs.mData[3]) <= epsilon;
}

bool 
Vector4f::operator<(const Vector4f &rhs) const
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

    if (mData[2] < rhs.mData[2]) {
        return true;
    } else if (mData[2] > rhs.mData[2]) {
        return false;
    }

    if (mData[3] < rhs.mData[3]) {
        return true;
    } else if (mData[3] > rhs.mData[3]) {
        return false;
    }

    // The two vectors are equal.
    return false;
}

Vector4f 
Vector4f::operator+(const Vector4f &rhs) const 
{
    return Vector4f(
        mData[0] + rhs.mData[0], 
        mData[1] + rhs.mData[1], 
        mData[2] + rhs.mData[2], 
        mData[3] + rhs.mData[3]);
}

Vector4f &
Vector4f::operator+=(const Vector4f &rhs) 
{
    mData[0] += rhs.mData[0];
    mData[1] += rhs.mData[1];
    mData[2] += rhs.mData[2];
    mData[3] += rhs.mData[3];

    return *this;
}

Vector4f 
Vector4f::operator-(const Vector4f &rhs) const 
{
    return Vector4f(
        mData[0] - rhs.mData[0], 
        mData[1] - rhs.mData[1], 
        mData[2] - rhs.mData[2], 
        mData[3] - rhs.mData[3]);
}

Vector4f &
Vector4f::operator-=(const Vector4f &rhs) 
{
    mData[0] -= rhs.mData[0];
    mData[1] -= rhs.mData[1];
    mData[2] -= rhs.mData[2];
    mData[3] -= rhs.mData[3];

    return *this;
}

Vector4f 
Vector4f::operator-() const 
{
    return Vector4f(-mData[0], -mData[1], -mData[2], -mData[3]);
}

Vector4f 
Vector4f::operator*(float c) const 
{
    return Vector4f(mData[0]*c, mData[1]*c, mData[2]*c, mData[3]*c);
}

Vector4f &
Vector4f::operator*=(float c) 
{
    mData[0] *= c;
    mData[1] *= c;
    mData[2] *= c;
    mData[3] *= c;

    return *this;
}

Vector4f 
operator*(float c, const Vector4f &rhs) 
{
    return Vector4f(c*rhs.mData[0], c*rhs.mData[1], c*rhs.mData[2], c*rhs.mData[3]);
}

Vector4f 
Vector4f::operator*(const Vector4f &rhs) const
{
    return Vector4f(mData[0]*rhs[0], mData[1]*rhs[1], mData[2]*rhs[2], mData[3]*rhs[3]);
}

Vector4f &
Vector4f::operator*=(const Vector4f &rhs)
{
    mData[0] *= rhs.mData[0];
    mData[1] *= rhs.mData[1];
    mData[2] *= rhs.mData[2];
    mData[3] *= rhs.mData[3];

    return *this;
}

Vector4f 
Vector4f::operator/(float c) const
{
    return Vector4f(mData[0]/c, mData[1]/c, mData[2]/c, mData[3]/c);
}

Vector4f &
Vector4f::operator/=(float c) 
{
    mData[0] /= c;
    mData[1] /= c;
    mData[2] /= c;
    mData[3] /= c;

    return *this;
}

float
Vector4f::dot(const Vector4f &rhs) const 
{
    return mData[0]*rhs.mData[0] 
        + mData[1]*rhs.mData[1] 
        + mData[2]*rhs.mData[2] 
        + mData[3]*rhs.mData[3];
}

float 
Vector4f::length() const 
{
    return sqrtf(
        mData[0]*mData[0] 
        + mData[1]*mData[1] 
        + mData[2]*mData[2] 
        + mData[3]*mData[3]);
}

float 
Vector4f::lengthSquared() const
{
    return mData[0]*mData[0] 
        + mData[1]*mData[1] 
        + mData[2]*mData[2] 
        + mData[3]*mData[3];
}

Vector4f &
Vector4f::normalize() 
{
    *this /= length();

    return *this;
}

Vector4f
Vector4f::normalized() const
{
    return *this/length();
}

float 
Vector4f::maxAbs() const
{
    return std::max(fabsf(mData[0]), std::max(fabsf(mData[1]), 
            std::max(fabsf(mData[2]), fabsf(mData[3]))));
}

std::ostream &
operator<<(std::ostream &ostr, const Vector4f &rhs) 
{
    ostr << rhs.mData[0] 
        << " " << rhs.mData[1] 
        << " " << rhs.mData[2] 
        << " " << rhs.mData[3];

    return ostr;
}

const Vector4f Vector4f::ZERO(0, 0, 0, 0);

const Vector4f Vector4f::UNIT(1, 1, 1, 1);

} // namespace cgmath
