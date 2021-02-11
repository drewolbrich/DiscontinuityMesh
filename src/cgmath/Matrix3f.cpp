// Copyright 2008 Drew Olbrich

#include "Matrix3f.h"

#include <cmath>
#include <cassert>

#include "WriteMatrix.h"
#include "Matrix4f.h"

namespace cgmath {

Matrix3f::Matrix3f()
{
}

Matrix3f::~Matrix3f()
{
}

Matrix3f::Matrix3f(const Matrix3f &rhs) 
{
    mData[0][0] = rhs.mData[0][0];
    mData[0][1] = rhs.mData[0][1];
    mData[0][2] = rhs.mData[0][2];

    mData[1][0] = rhs.mData[1][0];
    mData[1][1] = rhs.mData[1][1];
    mData[1][2] = rhs.mData[1][2];

    mData[2][0] = rhs.mData[2][0];
    mData[2][1] = rhs.mData[2][1];
    mData[2][2] = rhs.mData[2][2];
}

Matrix3f::Matrix3f(
    float a11, float a12, float a13,
    float a21, float a22, float a23,
    float a31, float a32, float a33)
{
    mData[0][0] = a11;     // First column.
    mData[0][1] = a21;
    mData[0][2] = a31;

    mData[1][0] = a12;     // Second column.
    mData[1][1] = a22;
    mData[1][2] = a32;

    mData[2][0] = a13;     // Third column.
    mData[2][1] = a23;
    mData[2][2] = a33;
}

Matrix3f::Matrix3f(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3)
{
    mData[0][0] = v1[0];   // First column.
    mData[0][1] = v1[1];
    mData[0][2] = v1[2];

    mData[1][0] = v2[0];   // Second column.
    mData[1][1] = v2[1];
    mData[1][2] = v2[2];

    mData[2][0] = v3[0];   // Third column.
    mData[2][1] = v3[1];
    mData[2][2] = v3[2];
}

Matrix3f::Matrix3f(const float *elements)
{
    mData[0][0] = elements[0];     // First column.
    mData[0][1] = elements[1];
    mData[0][2] = elements[2];

    mData[1][0] = elements[3];     // Second column.
    mData[1][1] = elements[4];
    mData[1][2] = elements[5];

    mData[2][0] = elements[6];     // Third column.
    mData[2][1] = elements[7];
    mData[2][2] = elements[8];
}

Matrix3f::Matrix3f(const float elements[3][3])
{
    mData[0][0] = elements[0][0];      // First column.
    mData[0][1] = elements[0][1];
    mData[0][2] = elements[0][2];

    mData[1][0] = elements[1][0];      // Second column.
    mData[1][1] = elements[1][1];
    mData[1][2] = elements[1][2];

    mData[2][0] = elements[2][0];      // Third column.
    mData[2][1] = elements[2][1];
    mData[2][2] = elements[2][2];
}

Matrix3f::Matrix3f(const Matrix4f &matrix4f)
{
    (*this)(0, 0) = matrix4f(0, 0);
    (*this)(0, 1) = matrix4f(0, 1);
    (*this)(0, 2) = matrix4f(0, 2);

    (*this)(1, 0) = matrix4f(1, 0);
    (*this)(1, 1) = matrix4f(1, 1);
    (*this)(1, 2) = matrix4f(1, 2);

    (*this)(2, 0) = matrix4f(2, 0);
    (*this)(2, 1) = matrix4f(2, 1);
    (*this)(2, 2) = matrix4f(2, 2);
}

bool 
Matrix3f::operator==(const Matrix3f &rhs) const 
{
    return mData[0][0] == rhs.mData[0][0]
        && mData[0][1] == rhs.mData[0][1]
        && mData[0][2] == rhs.mData[0][2]
        
        && mData[1][0] == rhs.mData[1][0]
        && mData[1][1] == rhs.mData[1][1]
        && mData[1][2] == rhs.mData[1][2]
        
        && mData[2][0] == rhs.mData[2][0]
        && mData[2][1] == rhs.mData[2][1]
        && mData[2][2] == rhs.mData[2][2];
}

bool 
Matrix3f::operator!=(const Matrix3f &rhs) const 
{
    return mData[0][0] != rhs.mData[0][0]
        || mData[0][1] != rhs.mData[0][1]
        || mData[0][2] != rhs.mData[0][2]

        || mData[1][0] != rhs.mData[1][0]
        || mData[1][1] != rhs.mData[1][1]
        || mData[1][2] != rhs.mData[1][2]

        || mData[2][0] != rhs.mData[2][0]
        || mData[2][1] != rhs.mData[2][1]
        || mData[2][2] != rhs.mData[2][2];
}

Matrix3f &
Matrix3f::operator=(const Matrix3f &rhs) 
{
    mData[0][0] = rhs.mData[0][0];
    mData[0][1] = rhs.mData[0][1];
    mData[0][2] = rhs.mData[0][2];

    mData[1][0] = rhs.mData[1][0];
    mData[1][1] = rhs.mData[1][1];
    mData[1][2] = rhs.mData[1][2];

    mData[2][0] = rhs.mData[2][0];
    mData[2][1] = rhs.mData[2][1];
    mData[2][2] = rhs.mData[2][2];

    return *this;
}

void 
Matrix3f::set(
    float a11, float a12, float a13,
    float a21, float a22, float a23,
    float a31, float a32, float a33)
{
    mData[0][0] = a11;     // First column.
    mData[0][1] = a21;
    mData[0][2] = a31;

    mData[1][0] = a12;     // Second column.
    mData[1][1] = a22;
    mData[1][2] = a32;

    mData[2][0] = a13;     // Third column.
    mData[2][1] = a23;
    mData[2][2] = a33;
}

Matrix3f 
Matrix3f::fromRotation(Vector3f axis, float angleInRadians)
{
    axis.normalize();
    
    float c = cosf(angleInRadians);
    float s = sinf(angleInRadians);
    float t = 1.0F - c;

    return Matrix3f(
        t*axis[0]*axis[0] + c,              // First row.
        t*axis[0]*axis[1] - s*axis[2],
        t*axis[0]*axis[2] + s*axis[1],

        t*axis[0]*axis[1] + s*axis[2],      // Second row.
        t*axis[1]*axis[1] + c,
        t*axis[1]*axis[2] - s*axis[0],

        t*axis[0]*axis[2] - s*axis[1],      // Third row.
        t*axis[1]*axis[2] + s*axis[0],
        t*axis[2]*axis[2] + c
        );
}

Matrix3f 
Matrix3f::fromScale(const Vector3f &scale)
{
    return Matrix3f(
        scale[0], 0, 0,
        0, scale[1], 0,
        0, 0, scale[2]);
}

const float *
Matrix3f::asFloatPtr() const
{
    return static_cast<const float *>(&mData[0][0]);
}

float 
Matrix3f::operator()(int row, int column) const 
{
    assert(row >= 0);
    assert(row < 3);
    assert(column >= 0);
    assert(column < 3);

    return mData[column][row];
}

float &
Matrix3f::operator()(int row, int column)
{
    assert(row >= 0);
    assert(row < 3);
    assert(column >= 0);
    assert(column < 3);

    return mData[column][row];
}
    
Matrix3f 
Matrix3f::operator+(const Matrix3f &rhs) const 
{
    return Matrix3f(
        mData[0][0] + rhs.mData[0][0],
        mData[1][0] + rhs.mData[1][0],
        mData[2][0] + rhs.mData[2][0],

        mData[0][1] + rhs.mData[0][1],
        mData[1][1] + rhs.mData[1][1],
        mData[2][1] + rhs.mData[2][1],

        mData[0][2] + rhs.mData[0][2],
        mData[1][2] + rhs.mData[1][2],
        mData[2][2] + rhs.mData[2][2]);
}

Matrix3f &
Matrix3f::operator+=(const Matrix3f &rhs) 
{
    mData[0][0] += rhs.mData[0][0];
    mData[0][1] += rhs.mData[0][1];
    mData[0][2] += rhs.mData[0][2];

    mData[1][0] += rhs.mData[1][0];
    mData[1][1] += rhs.mData[1][1];
    mData[1][2] += rhs.mData[1][2];

    mData[2][0] += rhs.mData[2][0];
    mData[2][1] += rhs.mData[2][1];
    mData[2][2] += rhs.mData[2][2];

    return *this;
}

Matrix3f 
Matrix3f::operator-(const Matrix3f &rhs) const 
{
    return Matrix3f(
        mData[0][0] - rhs.mData[0][0],
        mData[1][0] - rhs.mData[1][0],
        mData[2][0] - rhs.mData[2][0],
        
        mData[0][1] - rhs.mData[0][1],
        mData[1][1] - rhs.mData[1][1],
        mData[2][1] - rhs.mData[2][1],
        
        mData[0][2] - rhs.mData[0][2],
        mData[1][2] - rhs.mData[1][2],
        mData[2][2] - rhs.mData[2][2]);
}

Matrix3f &
Matrix3f::operator-=(const Matrix3f &rhs) 
{
    mData[0][0] -= rhs.mData[0][0];
    mData[0][1] -= rhs.mData[0][1];
    mData[0][2] -= rhs.mData[0][2];

    mData[1][0] -= rhs.mData[1][0];
    mData[1][1] -= rhs.mData[1][1];
    mData[1][2] -= rhs.mData[1][2];

    mData[2][0] -= rhs.mData[2][0];
    mData[2][1] -= rhs.mData[2][1];
    mData[2][2] -= rhs.mData[2][2];

    return *this;
}

Matrix3f 
Matrix3f::operator-() const 
{
    return Matrix3f(
        -mData[0][0], -mData[1][0], -mData[2][0],
        -mData[0][1], -mData[1][1], -mData[2][1],
        -mData[0][2], -mData[1][2], -mData[2][2]);
}

Matrix3f 
Matrix3f::operator*(float c) const 
{
    return Matrix3f(
        mData[0][0]*c, mData[1][0]*c, mData[2][0]*c,
        mData[0][1]*c, mData[1][1]*c, mData[2][1]*c,
        mData[0][2]*c, mData[1][2]*c, mData[2][2]*c);
}

Matrix3f &
Matrix3f::operator*=(float c) 
{
    mData[0][0] *= c;
    mData[0][1] *= c;
    mData[0][2] *= c;

    mData[1][0] *= c;
    mData[1][1] *= c;
    mData[1][2] *= c;

    mData[2][0] *= c;
    mData[2][1] *= c;
    mData[2][2] *= c;

    return *this;
}

Matrix3f 
operator*(float c, const Matrix3f &rhs) 
{
    return Matrix3f(
        c*rhs.mData[0][0], c*rhs.mData[1][0], c*rhs.mData[2][0],
        c*rhs.mData[0][1], c*rhs.mData[1][1], c*rhs.mData[2][1],
        c*rhs.mData[0][2], c*rhs.mData[1][2], c*rhs.mData[2][2]);
}

Vector3f
Matrix3f::operator*(const Vector3f &rhs) const
{
    return Vector3f(
        mData[0][0]*rhs[0]
        + mData[1][0]*rhs[1]
        + mData[2][0]*rhs[2],

        mData[0][1]*rhs[0]
        + mData[1][1]*rhs[1]
        + mData[2][1]*rhs[2],

        mData[0][2]*rhs[0]
        + mData[1][2]*rhs[1]
        + mData[2][2]*rhs[2]);
}

Vector3f 
operator*(const Vector3f &lhs, const Matrix3f &rhs)
{
    return Vector3f(
        lhs[0]*rhs.mData[0][0]
        + lhs[1]*rhs.mData[0][1]
        + lhs[2]*rhs.mData[0][2],

        lhs[0]*rhs.mData[1][0]
        + lhs[1]*rhs.mData[1][1]
        + lhs[2]*rhs.mData[1][2],

        lhs[0]*rhs.mData[2][0]
        + lhs[1]*rhs.mData[2][1]
        + lhs[2]*rhs.mData[2][2]);
}

Matrix3f 
Matrix3f::operator*(const Matrix3f &rhs) const
{
    return Matrix3f(
        mData[0][0]*rhs.mData[0][0]
        + mData[1][0]*rhs.mData[0][1]
        + mData[2][0]*rhs.mData[0][2],
        mData[0][0]*rhs.mData[1][0]
        + mData[1][0]*rhs.mData[1][1]
        + mData[2][0]*rhs.mData[1][2],
        mData[0][0]*rhs.mData[2][0]
        + mData[1][0]*rhs.mData[2][1]
        + mData[2][0]*rhs.mData[2][2],

        mData[0][1]*rhs.mData[0][0]
        + mData[1][1]*rhs.mData[0][1]
        + mData[2][1]*rhs.mData[0][2],
        mData[0][1]*rhs.mData[1][0]
        + mData[1][1]*rhs.mData[1][1]
        + mData[2][1]*rhs.mData[1][2],
        mData[0][1]*rhs.mData[2][0]
        + mData[1][1]*rhs.mData[2][1]
        + mData[2][1]*rhs.mData[2][2],

        mData[0][2]*rhs.mData[0][0]
        + mData[1][2]*rhs.mData[0][1]
        + mData[2][2]*rhs.mData[0][2],
        mData[0][2]*rhs.mData[1][0]
        + mData[1][2]*rhs.mData[1][1]
        + mData[2][2]*rhs.mData[1][2],
        mData[0][2]*rhs.mData[2][0]
        + mData[1][2]*rhs.mData[2][1]
        + mData[2][2]*rhs.mData[2][2]);
}

Matrix3f 
Matrix3f::operator/(float c) const
{
    return Matrix3f(
        mData[0][0]/c, mData[1][0]/c, mData[2][0]/c,
        mData[0][1]/c, mData[1][1]/c, mData[2][1]/c,
        mData[0][2]/c, mData[1][2]/c, mData[2][2]/c);
}

Matrix3f &
Matrix3f::operator/=(float c) 
{
    mData[0][0] /= c;
    mData[0][1] /= c;
    mData[0][2] /= c;

    mData[1][0] /= c;
    mData[1][1] /= c;
    mData[1][2] /= c;

    mData[2][0] /= c;
    mData[2][1] /= c;
    mData[2][2] /= c;

    return *this;
}

Matrix3f 
Matrix3f::transpose() const
{
    return Matrix3f(
        mData[0][0], mData[0][1], mData[0][2],
        mData[1][0], mData[1][1], mData[1][2],
        mData[2][0], mData[2][1], mData[2][2]);
}

Matrix3f 
Matrix3f::inverse() const
{
    return adjugate()/determinant();
}

Matrix3f 
Matrix3f::adjugate() const
{
    return Matrix3f(
        determinant2x2(mData[1][1], mData[2][1], mData[1][2], mData[2][2]),
        -determinant2x2(mData[1][0], mData[2][0], mData[1][2], mData[2][2]),
        determinant2x2(mData[1][0], mData[2][0], mData[1][1], mData[2][1]),
        
        -determinant2x2(mData[0][1], mData[2][1], mData[0][2], mData[2][2]),
        determinant2x2(mData[0][0], mData[2][0], mData[0][2], mData[2][2]),
        -determinant2x2(mData[0][0], mData[2][0], mData[0][1], mData[2][1]),
        
        determinant2x2(mData[0][1], mData[1][1], mData[0][2], mData[1][2]),
        -determinant2x2(mData[0][0], mData[1][0], mData[0][2], mData[1][2]),
        determinant2x2(mData[0][0], mData[1][0], mData[0][1], mData[1][1])
        );
}

float 
Matrix3f::determinant() const
{
    return mData[0][0]*(mData[1][1]*mData[2][2] - mData[2][1]*mData[1][2])
        + mData[1][0]*(mData[2][1]*mData[0][2] - mData[0][1]*mData[2][2])
        + mData[2][0]*(mData[0][1]*mData[1][2] - mData[1][1]*mData[0][2]);
}

std::ostream &
operator<<(std::ostream &ostr, const Matrix3f &rhs) 
{
    cgmath::WriteMatrix(ostr, 3, 3, &rhs.mData[0][0]);

    return ostr;
}

const Matrix3f Matrix3f::ZERO(
    0, 0, 0,
    0, 0, 0,
    0, 0, 0);

const Matrix3f Matrix3f::IDENTITY(
    1, 0, 0,
    0, 1, 0,
    0, 0, 1);

float 
Matrix3f::determinant2x2(
    float a11, float a12, 
    float a21, float a22) const
{
    return a11*a22 - a12*a21;
}

} // namespace cgmath
