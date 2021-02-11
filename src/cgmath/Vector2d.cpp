// Copyright 2008 Drew Olbrich

#include "Vector2d.h"

#include <cassert>
#include <cmath>
#include <algorithm>

#include "Vector2f.h"
#include "Vector3f.h"

namespace cgmath {

Vector2d::Vector2d()
{
}

Vector2d::~Vector2d()
{
}
    
Vector2d::Vector2d(const Vector2d& rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];
}

Vector2d::Vector2d(const Vector2f &rhs)
{
    mData[0] = rhs[0];
    mData[1] = rhs[1];
}

Vector2d::Vector2d(double c1, double c2) 
{
    mData[0] = c1;
    mData[1] = c2;
}

#if 0
Vector2d::Vector2d(const Vector3f &rhs, int axis1, int axis2)
{
    assert(axis1 >= 0);
    assert(axis1 < 3);
    assert(axis2 >= 0);
    assert(axis2 < 3);

    mData[0] = rhs[axis1];
    mData[1] = rhs[axis2];
}
#endif

Vector2d::Vector2d(const double *components)
{
    mData[0] = components[0];
    mData[1] = components[1];
}

const double *
Vector2d::asDoublePtr() const
{
    return static_cast<const double *>(&mData[0]);
}

Vector2d &
Vector2d::operator=(const Vector2d &rhs) 
{
    mData[0] = rhs.mData[0];
    mData[1] = rhs.mData[1];

    return *this;
}

void 
Vector2d::swap(Vector2d &rhs)
{
    Vector2d temp(*this);
    *this = rhs;
    rhs = temp;
}

void 
Vector2d::set(double c1, double c2) 
{
    mData[0] = c1;
    mData[1] = c2;
}

double 
Vector2d::operator[](int index) const 
{
    assert(index >= 0);
    assert(index < 2);

    return mData[index];
}

double &
Vector2d::operator[](int index) 
{
    assert(index >= 0);
    assert(index < 2);

    return mData[index];
}

bool 
Vector2d::operator==(const Vector2d &rhs) const 
{
    return mData[0] == rhs.mData[0] 
        && mData[1] == rhs.mData[1];
}

bool 
Vector2d::operator!=(const Vector2d &rhs) const
{
    return mData[0] != rhs.mData[0] 
        || mData[1] != rhs.mData[1];
}

bool 
Vector2d::equivalent(const Vector2d &rhs, float epsilon) const
{
    return std::abs(mData[0] - rhs.mData[0]) <= epsilon
        && std::abs(mData[1] - rhs.mData[1]) <= epsilon;
}

bool 
Vector2d::operator<(const Vector2d &rhs) const
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

Vector2d 
Vector2d::operator+(const Vector2d &rhs) const 
{
    return Vector2d(
        mData[0] + rhs.mData[0], 
        mData[1] + rhs.mData[1]);
}

Vector2d &
Vector2d::operator+=(const Vector2d &rhs) 
{
    mData[0] += rhs.mData[0];
    mData[1] += rhs.mData[1];

    return *this;
}

Vector2d 
Vector2d::operator-(const Vector2d &rhs) const 
{
    return Vector2d(
        mData[0] - rhs.mData[0], 
        mData[1] - rhs.mData[1]);
}

Vector2d &
Vector2d::operator-=(const Vector2d &rhs) 
{
    mData[0] -= rhs.mData[0];
    mData[1] -= rhs.mData[1];

    return *this;
}

Vector2d 
Vector2d::operator-() const 
{
    return Vector2d(-mData[0], -mData[1]);
}

Vector2d 
Vector2d::operator*(double c) const 
{
    return Vector2d(mData[0]*c, mData[1]*c);
}

Vector2d &
Vector2d::operator*=(double c) 
{
    mData[0] *= c;
    mData[1] *= c;

    return *this;
}

Vector2d 
operator*(double c, const Vector2d &rhs) 
{
    return Vector2d(c*rhs.mData[0], c*rhs.mData[1]);
}

Vector2d 
Vector2d::operator*(const Vector2d &rhs) const
{
    return Vector2d(mData[0]*rhs[0], mData[1]*rhs[1]);
}

Vector2d &
Vector2d::operator*=(const Vector2d &rhs)
{
    mData[0] *= rhs.mData[0];
    mData[1] *= rhs.mData[1];

    return *this;
}

Vector2d 
Vector2d::operator/(double c) const
{
    return Vector2d(mData[0]/c, mData[1]/c);
}

Vector2d &
Vector2d::operator/=(double c) 
{
    mData[0] /= c;
    mData[1] /= c;

    return *this;
}

double
Vector2d::dot(const Vector2d &rhs) const 
{
    return mData[0]*rhs.mData[0] 
        + mData[1]*rhs.mData[1];
}

double 
Vector2d::length() const 
{
    return sqrt(mData[0]*mData[0] 
        + mData[1]*mData[1]);
}

double 
Vector2d::lengthSquared() const
{
    return mData[0]*mData[0] 
        + mData[1]*mData[1];
}

Vector2d &
Vector2d::normalize() 
{
    *this /= length();

    return *this;
}

Vector2d 
Vector2d::normalized() const
{
    return *this/length();
}

double 
Vector2d::maxAbs() const
{
    return std::max(fabs(mData[0]), fabs(mData[1]));
}

std::ostream &
operator<<(std::ostream &ostr, const Vector2d &rhs) 
{
    ostr << rhs.mData[0] 
        << " " << rhs.mData[1];

    return ostr;
}

const Vector2d Vector2d::ZERO(0, 0);

const Vector2d Vector2d::UNIT(1, 1);

} // namespace cgmath
