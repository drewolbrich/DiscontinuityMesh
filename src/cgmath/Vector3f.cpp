// Copyright 2008 Drew Olbrich

#include "Vector3f.h"

#include <cassert>
#include <cmath>
#include <algorithm>

#include "Vector3d.h"
#include "Vector4f.h"

namespace cgmath {

Vector3f::Vector3f()
{
}

Vector3f::~Vector3f()
{
}
    
Vector3f::Vector3f(const Vector3f &rhs)
{
    mData[0] = rhs[0];
    mData[1] = rhs[1];
    mData[2] = rhs[2];
}

Vector3f::Vector3f(const Vector3d &rhs) 
{
    mData[0] = float(rhs[0]);
    mData[1] = float(rhs[1]);
    mData[2] = float(rhs[2]);
}

Vector3f::Vector3f(const Vector4f &rhs) 
{
    mData[0] = rhs[0];
    mData[1] = rhs[1];
    mData[2] = rhs[2];
}

Vector3f::Vector3f(float c1, float c2, float c3) 
{
    mData[0] = c1;
    mData[1] = c2;
    mData[2] = c3;
}

Vector3f::Vector3f(const float *components)
{
    mData[0] = components[0];
    mData[1] = components[1];
    mData[2] = components[2];
}

const float *
Vector3f::asFloatPtr() const
{
    return static_cast<const float *>(&mData[0]);
}

Vector3f &
Vector3f::operator=(const Vector3f &rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];
    mData[2] = rhs.mData[2];

    return *this;
}

void 
Vector3f::swap(Vector3f &rhs)
{
    Vector3f temp(*this);
    *this = rhs;
    rhs = temp;
}

void 
Vector3f::set(float c1, float c2, float c3) 
{
    mData[0] = c1;
    mData[1] = c2;
    mData[2] = c3;
}

float 
Vector3f::operator[](int index) const 
{
    assert(index >= 0);
    assert(index < 3);

    return mData[index];
}

float &
Vector3f::operator[](int index) 
{
    assert(index >= 0);
    assert(index < 3);

    return mData[index];
}

bool 
Vector3f::operator==(const Vector3f &rhs) const 
{
    return mData[0] == rhs.mData[0] 
        && mData[1] == rhs.mData[1]
        && mData[2] == rhs.mData[2];
}

bool 
Vector3f::operator!=(const Vector3f &rhs) const
{
    return mData[0] != rhs.mData[0] 
        || mData[1] != rhs.mData[1]
        || mData[2] != rhs.mData[2];
}

bool 
Vector3f::equivalent(const Vector3f &rhs, float epsilon) const
{
    return std::abs(mData[0] - rhs.mData[0]) <= epsilon
        && std::abs(mData[1] - rhs.mData[1]) <= epsilon
        && std::abs(mData[2] - rhs.mData[2]) <= epsilon;
}

bool 
Vector3f::operator<(const Vector3f &rhs) const
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

    // The two vectors are equal.
    return false;
}

Vector3f 
Vector3f::operator+(const Vector3f &rhs) const 
{
    return Vector3f(
        mData[0] + rhs.mData[0], 
        mData[1] + rhs.mData[1], 
        mData[2] + rhs.mData[2]);
}

Vector3f &
Vector3f::operator+=(const Vector3f &rhs) 
{
    mData[0] += rhs.mData[0];
    mData[1] += rhs.mData[1];
    mData[2] += rhs.mData[2];

    return *this;
}

Vector3f 
Vector3f::operator-(const Vector3f &rhs) const 
{
    return Vector3f(
        mData[0] - rhs.mData[0], 
        mData[1] - rhs.mData[1], 
        mData[2] - rhs.mData[2]);
}

Vector3f &
Vector3f::operator-=(const Vector3f &rhs) 
{
    mData[0] -= rhs.mData[0];
    mData[1] -= rhs.mData[1];
    mData[2] -= rhs.mData[2];

    return *this;
}

Vector3f 
Vector3f::operator-() const 
{
    return Vector3f(-mData[0], -mData[1], -mData[2]);
}

Vector3f 
Vector3f::operator*(float c) const 
{
    return Vector3f(mData[0]*c, mData[1]*c, mData[2]*c);
}

Vector3f &
Vector3f::operator*=(float c) 
{
    mData[0] *= c;
    mData[1] *= c;
    mData[2] *= c;

    return *this;
}

Vector3f 
operator*(float c, const Vector3f &rhs) 
{
    return Vector3f(c*rhs.mData[0], c*rhs.mData[1], c*rhs.mData[2]);
}

Vector3f 
Vector3f::operator*(const Vector3f &rhs) const
{
    return Vector3f(mData[0]*rhs[0], mData[1]*rhs[1], mData[2]*rhs[2]);
}

Vector3f &
Vector3f::operator*=(const Vector3f &rhs)
{
    mData[0] *= rhs.mData[0];
    mData[1] *= rhs.mData[1];
    mData[2] *= rhs.mData[2];

    return *this;
}

Vector3f 
Vector3f::operator/(float c) const
{
    return Vector3f(mData[0]/c, mData[1]/c, mData[2]/c);
}

Vector3f &
Vector3f::operator/=(float c) 
{
    mData[0] /= c;
    mData[1] /= c;
    mData[2] /= c;

    return *this;
}

float
Vector3f::dot(const Vector3f &rhs) const 
{
    return mData[0]*rhs.mData[0] 
        + mData[1]*rhs.mData[1] 
        + mData[2]*rhs.mData[2];
}

Vector3f 
Vector3f::cross(const Vector3f &rhs) const 
{
    return Vector3f(
        mData[1]*rhs.mData[2] - mData[2]*rhs.mData[1],
        mData[2]*rhs.mData[0] - mData[0]*rhs.mData[2],
        mData[0]*rhs.mData[1] - mData[1]*rhs.mData[0]);
}

float 
Vector3f::length() const 
{
    return sqrtf(
        mData[0]*mData[0] 
        + mData[1]*mData[1] 
        + mData[2]*mData[2]);
}

float 
Vector3f::lengthSquared() const
{
    return mData[0]*mData[0] 
        + mData[1]*mData[1] 
        + mData[2]*mData[2];
}

Vector3f &
Vector3f::normalize() 
{
    *this /= length();

    return *this;
}

Vector3f 
Vector3f::normalized() const
{
    return *this/length();
}

float 
Vector3f::maxAbs() const
{
    return std::max(fabsf(mData[0]), std::max(fabsf(mData[1]), fabsf(mData[2])));
}

std::ostream &
operator<<(std::ostream &ostr, const Vector3f &rhs) 
{
    ostr << rhs.mData[0] 
        << " " << rhs.mData[1] 
        << " " << rhs.mData[2];

    return ostr;
}

const Vector3f Vector3f::ZERO(0, 0, 0);

const Vector3f Vector3f::UNIT(1, 1, 1);

} // namespace cgmath
