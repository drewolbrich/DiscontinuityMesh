// Copyright 2008 Retroactive Fiasco.

#include "Vector3d.h"

#include <cassert>
#include <cmath>
#include <algorithm>

#include "Vector3f.h"
#if 0
#include "Vector4f.h"
#endif

namespace cgmath {

Vector3d::Vector3d()
{
}

Vector3d::~Vector3d()
{
}
    
Vector3d::Vector3d(const Vector3d &rhs)
{
    mData[0] = rhs[0];
    mData[1] = rhs[1];
    mData[2] = rhs[2];
}

Vector3d::Vector3d(const Vector3f &rhs) 
{
    mData[0] = double(rhs[0]);
    mData[1] = double(rhs[1]);
    mData[2] = double(rhs[2]);
}

#if 0
Vector3d::Vector3d(const Vector4d &rhs) 
{
    mData[0] = rhs[0];
    mData[1] = rhs[1];
    mData[2] = rhs[2];
}
#endif

Vector3d::Vector3d(double c1, double c2, double c3) 
{
    mData[0] = c1;
    mData[1] = c2;
    mData[2] = c3;
}

Vector3d::Vector3d(const double *components)
{
    mData[0] = components[0];
    mData[1] = components[1];
    mData[2] = components[2];
}

const double *
Vector3d::asDoublePtr() const
{
    return static_cast<const double *>(&mData[0]);
}

Vector3d &
Vector3d::operator=(const Vector3d &rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];
    mData[2] = rhs.mData[2];

    return *this;
}

void 
Vector3d::swap(Vector3d &rhs)
{
    Vector3d temp(*this);
    *this = rhs;
    rhs = temp;
}

void 
Vector3d::set(double c1, double c2, double c3) 
{
    mData[0] = c1;
    mData[1] = c2;
    mData[2] = c3;
}

double 
Vector3d::operator[](int index) const 
{
    assert(index >= 0);
    assert(index < 3);

    return mData[index];
}

double &
Vector3d::operator[](int index) 
{
    assert(index >= 0);
    assert(index < 3);

    return mData[index];
}

bool 
Vector3d::operator==(const Vector3d &rhs) const 
{
    return mData[0] == rhs.mData[0] 
        && mData[1] == rhs.mData[1]
        && mData[2] == rhs.mData[2];
}

bool 
Vector3d::operator!=(const Vector3d &rhs) const
{
    return mData[0] != rhs.mData[0] 
        || mData[1] != rhs.mData[1]
        || mData[2] != rhs.mData[2];
}

bool 
Vector3d::equivalent(const Vector3d &rhs, float epsilon) const
{
    return std::abs(mData[0] - rhs.mData[0]) <= epsilon
        && std::abs(mData[1] - rhs.mData[1]) <= epsilon
        && std::abs(mData[2] - rhs.mData[2]) <= epsilon;
}

bool 
Vector3d::operator<(const Vector3d &rhs) const
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

Vector3d 
Vector3d::operator+(const Vector3d &rhs) const 
{
    return Vector3d(
        mData[0] + rhs.mData[0], 
        mData[1] + rhs.mData[1], 
        mData[2] + rhs.mData[2]);
}

Vector3d &
Vector3d::operator+=(const Vector3d &rhs) 
{
    mData[0] += rhs.mData[0];
    mData[1] += rhs.mData[1];
    mData[2] += rhs.mData[2];

    return *this;
}

Vector3d 
Vector3d::operator-(const Vector3d &rhs) const 
{
    return Vector3d(
        mData[0] - rhs.mData[0], 
        mData[1] - rhs.mData[1], 
        mData[2] - rhs.mData[2]);
}

Vector3d &
Vector3d::operator-=(const Vector3d &rhs) 
{
    mData[0] -= rhs.mData[0];
    mData[1] -= rhs.mData[1];
    mData[2] -= rhs.mData[2];

    return *this;
}

Vector3d 
Vector3d::operator-() const 
{
    return Vector3d(-mData[0], -mData[1], -mData[2]);
}

Vector3d 
Vector3d::operator*(double c) const 
{
    return Vector3d(mData[0]*c, mData[1]*c, mData[2]*c);
}

Vector3d &
Vector3d::operator*=(double c) 
{
    mData[0] *= c;
    mData[1] *= c;
    mData[2] *= c;

    return *this;
}

Vector3d 
operator*(double c, const Vector3d &rhs) 
{
    return Vector3d(c*rhs.mData[0], c*rhs.mData[1], c*rhs.mData[2]);
}

Vector3d 
Vector3d::operator*(const Vector3d &rhs) const
{
    return Vector3d(mData[0]*rhs[0], mData[1]*rhs[1], mData[2]*rhs[2]);
}

Vector3d &
Vector3d::operator*=(const Vector3d &rhs)
{
    mData[0] *= rhs.mData[0];
    mData[1] *= rhs.mData[1];
    mData[2] *= rhs.mData[2];

    return *this;
}

Vector3d 
Vector3d::operator/(double c) const
{
    return Vector3d(mData[0]/c, mData[1]/c, mData[2]/c);
}

Vector3d &
Vector3d::operator/=(double c) 
{
    mData[0] /= c;
    mData[1] /= c;
    mData[2] /= c;

    return *this;
}

double
Vector3d::dot(const Vector3d &rhs) const 
{
    return mData[0]*rhs.mData[0] 
        + mData[1]*rhs.mData[1] 
        + mData[2]*rhs.mData[2];
}

Vector3d 
Vector3d::cross(const Vector3d &rhs) const 
{
    return Vector3d(
        mData[1]*rhs.mData[2] - mData[2]*rhs.mData[1],
        mData[2]*rhs.mData[0] - mData[0]*rhs.mData[2],
        mData[0]*rhs.mData[1] - mData[1]*rhs.mData[0]);
}

double 
Vector3d::length() const 
{
    return sqrt(
        mData[0]*mData[0] 
        + mData[1]*mData[1] 
        + mData[2]*mData[2]);
}

double 
Vector3d::lengthSquared() const
{
    return mData[0]*mData[0] 
        + mData[1]*mData[1] 
        + mData[2]*mData[2];
}

Vector3d &
Vector3d::normalize() 
{
    *this /= length();

    return *this;
}

Vector3d 
Vector3d::normalized() const
{
    return *this/length();
}

double 
Vector3d::maxAbs() const
{
    return std::max(fabs(mData[0]), std::max(fabs(mData[1]), fabs(mData[2])));
}

std::ostream &
operator<<(std::ostream &ostr, const Vector3d &rhs) 
{
    ostr << rhs.mData[0] 
        << " " << rhs.mData[1] 
        << " " << rhs.mData[2];

    return ostr;
}

const Vector3d Vector3d::ZERO(0, 0, 0);

const Vector3d Vector3d::UNIT(1, 1, 1);

} // namespace cgmath
