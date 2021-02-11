// Copyright 2008 Drew Olbrich

#include "Matrix4f.h"

#include <cassert>
#include <cmath>

#include "WriteMatrix.h"

namespace cgmath {

Matrix4f::Matrix4f()
{
}

Matrix4f::~Matrix4f()
{
}

Matrix4f::Matrix4f(const Matrix4f &rhs) 
{
    mData[0][0] = rhs.mData[0][0];
    mData[0][1] = rhs.mData[0][1];
    mData[0][2] = rhs.mData[0][2];
    mData[0][3] = rhs.mData[0][3];

    mData[1][0] = rhs.mData[1][0];
    mData[1][1] = rhs.mData[1][1];
    mData[1][2] = rhs.mData[1][2];
    mData[1][3] = rhs.mData[1][3];

    mData[2][0] = rhs.mData[2][0];
    mData[2][1] = rhs.mData[2][1];
    mData[2][2] = rhs.mData[2][2];
    mData[2][3] = rhs.mData[2][3];

    mData[3][0] = rhs.mData[3][0];
    mData[3][1] = rhs.mData[3][1];
    mData[3][2] = rhs.mData[3][2];
    mData[3][3] = rhs.mData[3][3];
}

Matrix4f::Matrix4f(
    float a11, float a12, float a13, float a14,
    float a21, float a22, float a23, float a24,
    float a31, float a32, float a33, float a34,
    float a41, float a42, float a43, float a44)
{
    mData[0][0] = a11;     // First column.
    mData[0][1] = a21;
    mData[0][2] = a31;
    mData[0][3] = a41;

    mData[1][0] = a12;     // Second column.
    mData[1][1] = a22;
    mData[1][2] = a32;
    mData[1][3] = a42;

    mData[2][0] = a13;     // Third column.
    mData[2][1] = a23;
    mData[2][2] = a33;
    mData[2][3] = a43;

    mData[3][0] = a14;     // Fourth column.
    mData[3][1] = a24;
    mData[3][2] = a34;
    mData[3][3] = a44;
}

Matrix4f::Matrix4f(const Vector3f &v1, const Vector3f &v2,  const Vector3f &v3, 
    const Vector3f &v4)
{
    mData[0][0] = v1[0];   // First column.
    mData[0][1] = v1[1];
    mData[0][2] = v1[2];
    mData[0][3] = 0;

    mData[1][0] = v2[0];   // Second column.
    mData[1][1] = v2[1];
    mData[1][2] = v2[2];
    mData[1][3] = 0;

    mData[2][0] = v3[0];   // Third column.
    mData[2][1] = v3[1];
    mData[2][2] = v3[2];
    mData[2][3] = 0;

    mData[3][0] = v4[0];   // Fourth column.
    mData[3][1] = v4[1];
    mData[3][2] = v4[2];
    mData[3][3] = 1;
}

Matrix4f::Matrix4f(const Vector4f &v1, const Vector4f &v2, const Vector4f &v3, 
    const Vector4f &v4)
{
    mData[0][0] = v1[0];   // First column.
    mData[0][1] = v1[1];
    mData[0][2] = v1[2];
    mData[0][3] = v1[3];

    mData[1][0] = v2[0];   // Second column.
    mData[1][1] = v2[1];
    mData[1][2] = v2[2];
    mData[1][3] = v2[3];

    mData[2][0] = v3[0];   // Third column.
    mData[2][1] = v3[1];
    mData[2][2] = v3[2];
    mData[2][3] = v3[3];

    mData[3][0] = v4[0];   // Fourth column.
    mData[3][1] = v4[1];
    mData[3][2] = v4[2];
    mData[3][3] = v4[3];
}

Matrix4f::Matrix4f(const Matrix3f &rhs)
{
    mData[0][0] = rhs(0, 0);
    mData[0][1] = rhs(1, 0);
    mData[0][2] = rhs(2, 0);
    mData[0][3] = 0;

    mData[1][0] = rhs(0, 1);
    mData[1][1] = rhs(1, 1);
    mData[1][2] = rhs(2, 1);
    mData[1][3] = 0;

    mData[2][0] = rhs(0, 2);
    mData[2][1] = rhs(1, 2);
    mData[2][2] = rhs(2, 2);
    mData[2][3] = 0;

    mData[3][0] = 0;
    mData[3][1] = 0;
    mData[3][2] = 0;
    mData[3][3] = 1;
}

Matrix4f::Matrix4f(const float *elements)
{
    mData[0][0] = elements[0];     // First column.
    mData[0][1] = elements[1];
    mData[0][2] = elements[2];
    mData[0][3] = elements[3];

    mData[1][0] = elements[4];     // Second column.
    mData[1][1] = elements[5];
    mData[1][2] = elements[6];
    mData[1][3] = elements[7];

    mData[2][0] = elements[8];     // Third column.
    mData[2][1] = elements[9];
    mData[2][2] = elements[10];
    mData[2][3] = elements[11];

    mData[3][0] = elements[12];    // Fourth column.
    mData[3][1] = elements[13];
    mData[3][2] = elements[14];
    mData[3][3] = elements[15];
}

Matrix4f::Matrix4f(const float elements[4][4])
{
    mData[0][0] = elements[0][0];
    mData[0][1] = elements[0][1];
    mData[0][2] = elements[0][2];
    mData[0][3] = elements[0][3];

    mData[1][0] = elements[1][0];
    mData[1][1] = elements[1][1];
    mData[1][2] = elements[1][2];
    mData[1][3] = elements[1][3];

    mData[2][0] = elements[2][0];
    mData[2][1] = elements[2][1];
    mData[2][2] = elements[2][2];
    mData[2][3] = elements[2][3];

    mData[3][0] = elements[3][0];
    mData[3][1] = elements[3][1];
    mData[3][2] = elements[3][2];
    mData[3][3] = elements[3][3];
}

Matrix4f 
Matrix4f::fromTranslation(const Vector3f &vector)
{
    return Matrix4f(
        1, 0, 0, vector[0],
        0, 1, 0, vector[1],
        0, 0, 1, vector[2],
        0, 0, 0, 1);
}

Matrix4f 
Matrix4f::fromRotation(Vector3f axis, float angleInRadians)
{
    axis.normalize();
    
    float c = cosf(angleInRadians);
    float s = sinf(angleInRadians);
    float t = 1.0F - c;

    return Matrix4f(

        t*axis[0]*axis[0] + c,              // First row.
        t*axis[0]*axis[1] - s*axis[2],
        t*axis[0]*axis[2] + s*axis[1],
        0,

        t*axis[0]*axis[1] + s*axis[2],      // Second row.
        t*axis[1]*axis[1] + c,
        t*axis[1]*axis[2] - s*axis[0],
        0,

        t*axis[0]*axis[2] - s*axis[1],      // Third row.
        t*axis[1]*axis[2] + s*axis[0],
        t*axis[2]*axis[2] + c,
        0,

        0, 0, 0, 1);                        // Fourth row.
}

Matrix4f 
Matrix4f::fromScale(const Vector3f &scale)
{
    return Matrix4f(
        scale[0], 0, 0, 0,
        0, scale[1], 0, 0,
        0, 0, scale[2], 0,
        0, 0, 0, 1);
}

bool 
Matrix4f::operator==(const Matrix4f &rhs) const 
{
    return mData[0][0] == rhs.mData[0][0]
        && mData[0][1] == rhs.mData[0][1]
        && mData[0][2] == rhs.mData[0][2]
        && mData[0][3] == rhs.mData[0][3]
        
        && mData[1][0] == rhs.mData[1][0]
        && mData[1][1] == rhs.mData[1][1]
        && mData[1][2] == rhs.mData[1][2]
        && mData[1][3] == rhs.mData[1][3]
        
        && mData[2][0] == rhs.mData[2][0]
        && mData[2][1] == rhs.mData[2][1]
        && mData[2][2] == rhs.mData[2][2]
        && mData[2][3] == rhs.mData[2][3]

        && mData[3][0] == rhs.mData[3][0]
        && mData[3][1] == rhs.mData[3][1]
        && mData[3][2] == rhs.mData[3][2]
        && mData[3][3] == rhs.mData[3][3];
}

bool 
Matrix4f::operator!=(const Matrix4f &rhs) const 
{
    return mData[0][0] != rhs.mData[0][0]
        || mData[0][1] != rhs.mData[0][1]
        || mData[0][2] != rhs.mData[0][2]
        || mData[0][3] != rhs.mData[0][3]

        || mData[1][0] != rhs.mData[1][0]
        || mData[1][1] != rhs.mData[1][1]
        || mData[1][2] != rhs.mData[1][2]
        || mData[1][3] != rhs.mData[1][3]

        || mData[2][0] != rhs.mData[2][0]
        || mData[2][1] != rhs.mData[2][1]
        || mData[2][2] != rhs.mData[2][2]
        || mData[2][3] != rhs.mData[2][3]

        || mData[3][0] != rhs.mData[3][0]
        || mData[3][1] != rhs.mData[3][1]
        || mData[3][2] != rhs.mData[3][2]
        || mData[3][3] != rhs.mData[3][3];
}

Matrix4f &
Matrix4f::operator=(const Matrix4f &rhs) 
{
    mData[0][0] = rhs.mData[0][0];
    mData[0][1] = rhs.mData[0][1];
    mData[0][2] = rhs.mData[0][2];
    mData[0][3] = rhs.mData[0][3];

    mData[1][0] = rhs.mData[1][0];
    mData[1][1] = rhs.mData[1][1];
    mData[1][2] = rhs.mData[1][2];
    mData[1][3] = rhs.mData[1][3];

    mData[2][0] = rhs.mData[2][0];
    mData[2][1] = rhs.mData[2][1];
    mData[2][2] = rhs.mData[2][2];
    mData[2][3] = rhs.mData[2][3];

    mData[3][0] = rhs.mData[3][0];
    mData[3][1] = rhs.mData[3][1];
    mData[3][2] = rhs.mData[3][2];
    mData[3][3] = rhs.mData[3][3];

    return *this;
}

void 
Matrix4f::set(
    float a11, float a12, float a13, float a14,
    float a21, float a22, float a23, float a24,
    float a31, float a32, float a33, float a34,
    float a41, float a42, float a43, float a44)
{
    mData[0][0] = a11;     // First column.
    mData[0][1] = a21;
    mData[0][2] = a31;
    mData[0][3] = a41;

    mData[1][0] = a12;     // Second column.
    mData[1][1] = a22;
    mData[1][2] = a32;
    mData[1][3] = a42;

    mData[2][0] = a13;     // Third column.
    mData[2][1] = a23;
    mData[2][2] = a33;
    mData[2][3] = a43;

    mData[3][0] = a14;     // Fourth column.
    mData[3][1] = a24;
    mData[3][2] = a34;
    mData[3][3] = a44;
}

const float *
Matrix4f::asFloatPtr() const
{
    return static_cast<const float *>(&mData[0][0]);
}

float 
Matrix4f::operator()(int row, int column) const 
{
    assert(row >= 0);
    assert(row < 4);
    assert(column >= 0);
    assert(column < 4);

    return mData[column][row];
}

float &
Matrix4f::operator()(int row, int column)
{
    assert(row >= 0);
    assert(row < 4);
    assert(column >= 0);
    assert(column < 4);

    return mData[column][row];
}

Matrix4f 
Matrix4f::operator+(const Matrix4f &rhs) const 
{
    return Matrix4f(
        mData[0][0] + rhs.mData[0][0],
        mData[1][0] + rhs.mData[1][0],
        mData[2][0] + rhs.mData[2][0],
        mData[3][0] + rhs.mData[3][0],

        mData[0][1] + rhs.mData[0][1],
        mData[1][1] + rhs.mData[1][1],
        mData[2][1] + rhs.mData[2][1],
        mData[3][1] + rhs.mData[3][1],

        mData[0][2] + rhs.mData[0][2],
        mData[1][2] + rhs.mData[1][2],
        mData[2][2] + rhs.mData[2][2],
        mData[3][2] + rhs.mData[3][2],

        mData[0][3] + rhs.mData[0][3],
        mData[1][3] + rhs.mData[1][3],
        mData[2][3] + rhs.mData[2][3],
        mData[3][3] + rhs.mData[3][3]);
}

Matrix4f &
Matrix4f::operator+=(const Matrix4f &rhs) 
{
    mData[0][0] += rhs.mData[0][0];
    mData[0][1] += rhs.mData[0][1];
    mData[0][2] += rhs.mData[0][2];
    mData[0][3] += rhs.mData[0][3];

    mData[1][0] += rhs.mData[1][0];
    mData[1][1] += rhs.mData[1][1];
    mData[1][2] += rhs.mData[1][2];
    mData[1][3] += rhs.mData[1][3];

    mData[2][0] += rhs.mData[2][0];
    mData[2][1] += rhs.mData[2][1];
    mData[2][2] += rhs.mData[2][2];
    mData[2][3] += rhs.mData[2][3];

    mData[3][0] += rhs.mData[3][0];
    mData[3][1] += rhs.mData[3][1];
    mData[3][2] += rhs.mData[3][2];
    mData[3][3] += rhs.mData[3][3];

    return *this;
}

Matrix4f 
Matrix4f::operator-(const Matrix4f &rhs) const 
{
    return Matrix4f(
        mData[0][0] - rhs.mData[0][0],
        mData[1][0] - rhs.mData[1][0],
        mData[2][0] - rhs.mData[2][0],
        mData[3][0] - rhs.mData[3][0],
        
        mData[0][1] - rhs.mData[0][1],
        mData[1][1] - rhs.mData[1][1],
        mData[2][1] - rhs.mData[2][1],
        mData[3][1] - rhs.mData[3][1],
        
        mData[0][2] - rhs.mData[0][2],
        mData[1][2] - rhs.mData[1][2],
        mData[2][2] - rhs.mData[2][2],
        mData[3][2] - rhs.mData[3][2],
        
        mData[0][3] - rhs.mData[0][3],
        mData[1][3] - rhs.mData[1][3],
        mData[2][3] - rhs.mData[2][3],
        mData[3][3] - rhs.mData[3][3]);
}

Matrix4f &
Matrix4f::operator-=(const Matrix4f &rhs) 
{
    mData[0][0] -= rhs.mData[0][0];
    mData[0][1] -= rhs.mData[0][1];
    mData[0][2] -= rhs.mData[0][2];
    mData[0][3] -= rhs.mData[0][3];

    mData[1][0] -= rhs.mData[1][0];
    mData[1][1] -= rhs.mData[1][1];
    mData[1][2] -= rhs.mData[1][2];
    mData[1][3] -= rhs.mData[1][3];

    mData[2][0] -= rhs.mData[2][0];
    mData[2][1] -= rhs.mData[2][1];
    mData[2][2] -= rhs.mData[2][2];
    mData[2][3] -= rhs.mData[2][3];

    mData[3][0] -= rhs.mData[3][0];
    mData[3][1] -= rhs.mData[3][1];
    mData[3][2] -= rhs.mData[3][2];
    mData[3][3] -= rhs.mData[3][3];

    return *this;
}

Matrix4f 
Matrix4f::operator-() const 
{
    return Matrix4f(
        -mData[0][0], -mData[1][0], -mData[2][0], -mData[3][0],
        -mData[0][1], -mData[1][1], -mData[2][1], -mData[3][1],
        -mData[0][2], -mData[1][2], -mData[2][2], -mData[3][2],
        -mData[0][3], -mData[1][3], -mData[2][3], -mData[3][3]);
}

Matrix4f 
Matrix4f::operator*(float c) const 
{
    return Matrix4f(
        mData[0][0]*c, mData[1][0]*c, mData[2][0]*c, mData[3][0]*c,
        mData[0][1]*c, mData[1][1]*c, mData[2][1]*c, mData[3][1]*c,
        mData[0][2]*c, mData[1][2]*c, mData[2][2]*c, mData[3][2]*c,
        mData[0][3]*c, mData[1][3]*c, mData[2][3]*c, mData[3][3]*c);
}

Matrix4f &
Matrix4f::operator*=(float c) 
{
    mData[0][0] *= c;
    mData[0][1] *= c;
    mData[0][2] *= c;
    mData[0][3] *= c;

    mData[1][0] *= c;
    mData[1][1] *= c;
    mData[1][2] *= c;
    mData[1][3] *= c;

    mData[2][0] *= c;
    mData[2][1] *= c;
    mData[2][2] *= c;
    mData[2][3] *= c;

    mData[3][0] *= c;
    mData[3][1] *= c;
    mData[3][2] *= c;
    mData[3][3] *= c;

    return *this;
}

Matrix4f 
operator*(float c, const Matrix4f &rhs) 
{
    return Matrix4f(
        c*rhs.mData[0][0], c*rhs.mData[1][0], c*rhs.mData[2][0], c*rhs.mData[3][0],
        c*rhs.mData[0][1], c*rhs.mData[1][1], c*rhs.mData[2][1], c*rhs.mData[3][1],
        c*rhs.mData[0][2], c*rhs.mData[1][2], c*rhs.mData[2][2], c*rhs.mData[3][2],
        c*rhs.mData[0][3], c*rhs.mData[1][3], c*rhs.mData[2][3], c*rhs.mData[3][3]);
}

Vector3f
Matrix4f::operator*(const Vector3f &rhs) const
{
    float inverseW = 1.0/(mData[0][3]*rhs[0] + mData[1][3]*rhs[1]
        + mData[2][3]*rhs[2] + mData[3][3]);

    return Vector3f(
        (mData[0][0]*rhs[0]
            + mData[1][0]*rhs[1]
            + mData[2][0]*rhs[2]
            + mData[3][0])*inverseW,

        (mData[0][1]*rhs[0]
            + mData[1][1]*rhs[1]
            + mData[2][1]*rhs[2]
            + mData[3][1])*inverseW,

        (mData[0][2]*rhs[0]
            + mData[1][2]*rhs[1]
            + mData[2][2]*rhs[2]
            + mData[3][2])*inverseW);
}

Vector4f
Matrix4f::operator*(const Vector4f &rhs) const
{
    return Vector4f(
        mData[0][0]*rhs[0]
        + mData[1][0]*rhs[1]
        + mData[2][0]*rhs[2]
        + mData[3][0]*rhs[3],

        mData[0][1]*rhs[0]
        + mData[1][1]*rhs[1]
        + mData[2][1]*rhs[2]
        + mData[3][1]*rhs[3],

        mData[0][2]*rhs[0]
        + mData[1][2]*rhs[1]
        + mData[2][2]*rhs[2]
        + mData[3][2]*rhs[3],

        mData[0][3]*rhs[0]
        + mData[1][3]*rhs[1]
        + mData[2][3]*rhs[2]
        + mData[3][3]*rhs[3]);
}

Vector3f 
operator*(const Vector3f &lhs, const Matrix4f &rhs)
{
    float inverseW = 1.0/(rhs.mData[3][0]*lhs[0] + rhs.mData[3][1]*lhs[1]
        + rhs.mData[3][2]*lhs[2] + rhs.mData[3][3]);

    return Vector3f(
        (lhs[0]*rhs.mData[0][0]
            + lhs[1]*rhs.mData[0][1]
            + lhs[2]*rhs.mData[0][2]
            + rhs.mData[0][3])*inverseW,

        (lhs[0]*rhs.mData[1][0]
            + lhs[1]*rhs.mData[1][1]
            + lhs[2]*rhs.mData[1][2]
            + rhs.mData[1][3])*inverseW,

        (lhs[0]*rhs.mData[2][0]
            + lhs[1]*rhs.mData[2][1]
            + lhs[2]*rhs.mData[2][2]
            + rhs.mData[2][3])*inverseW);
}

Vector4f 
operator*(const Vector4f &lhs, const Matrix4f &rhs)
{
    return Vector4f(
        lhs[0]*rhs.mData[0][0]
        + lhs[1]*rhs.mData[0][1]
        + lhs[2]*rhs.mData[0][2]
        + lhs[3]*rhs.mData[0][3],

        lhs[0]*rhs.mData[1][0]
        + lhs[1]*rhs.mData[1][1]
        + lhs[2]*rhs.mData[1][2]
        + lhs[3]*rhs.mData[1][3],

        lhs[0]*rhs.mData[2][0]
        + lhs[1]*rhs.mData[2][1]
        + lhs[2]*rhs.mData[2][2]
        + lhs[3]*rhs.mData[2][3],

        lhs[0]*rhs.mData[3][0]
        + lhs[1]*rhs.mData[3][1]
        + lhs[2]*rhs.mData[3][2]
        + lhs[3]*rhs.mData[3][3]);
}

Matrix4f 
Matrix4f::operator*(const Matrix4f &rhs) const
{
    return Matrix4f(
        mData[0][0]*rhs.mData[0][0]
        + mData[1][0]*rhs.mData[0][1]
        + mData[2][0]*rhs.mData[0][2]
        + mData[3][0]*rhs.mData[0][3],
        mData[0][0]*rhs.mData[1][0]
        + mData[1][0]*rhs.mData[1][1]
        + mData[2][0]*rhs.mData[1][2]
        + mData[3][0]*rhs.mData[1][3],
        mData[0][0]*rhs.mData[2][0]
        + mData[1][0]*rhs.mData[2][1]
        + mData[2][0]*rhs.mData[2][2]
        + mData[3][0]*rhs.mData[2][3],
        mData[0][0]*rhs.mData[3][0]
        + mData[1][0]*rhs.mData[3][1]
        + mData[2][0]*rhs.mData[3][2]
        + mData[3][0]*rhs.mData[3][3],

        mData[0][1]*rhs.mData[0][0]
        + mData[1][1]*rhs.mData[0][1]
        + mData[2][1]*rhs.mData[0][2]
        + mData[3][1]*rhs.mData[0][3],
        mData[0][1]*rhs.mData[1][0]
        + mData[1][1]*rhs.mData[1][1]
        + mData[2][1]*rhs.mData[1][2]
        + mData[3][1]*rhs.mData[1][3],
        mData[0][1]*rhs.mData[2][0]
        + mData[1][1]*rhs.mData[2][1]
        + mData[2][1]*rhs.mData[2][2]
        + mData[3][1]*rhs.mData[2][3],
        mData[0][1]*rhs.mData[3][0]
        + mData[1][1]*rhs.mData[3][1]
        + mData[2][1]*rhs.mData[3][2]
        + mData[3][1]*rhs.mData[3][3],

        mData[0][2]*rhs.mData[0][0]
        + mData[1][2]*rhs.mData[0][1]
        + mData[2][2]*rhs.mData[0][2]
        + mData[3][2]*rhs.mData[0][3],
        mData[0][2]*rhs.mData[1][0]
        + mData[1][2]*rhs.mData[1][1]
        + mData[2][2]*rhs.mData[1][2]
        + mData[3][2]*rhs.mData[1][3],
        mData[0][2]*rhs.mData[2][0]
        + mData[1][2]*rhs.mData[2][1]
        + mData[2][2]*rhs.mData[2][2]
        + mData[3][2]*rhs.mData[2][3],
        mData[0][2]*rhs.mData[3][0]
        + mData[1][2]*rhs.mData[3][1]
        + mData[2][2]*rhs.mData[3][2]
        + mData[3][2]*rhs.mData[3][3],

        mData[0][3]*rhs.mData[0][0]
        + mData[1][3]*rhs.mData[0][1]
        + mData[2][3]*rhs.mData[0][2]
        + mData[3][3]*rhs.mData[0][3],
        mData[0][3]*rhs.mData[1][0]
        + mData[1][3]*rhs.mData[1][1]
        + mData[2][3]*rhs.mData[1][2]
        + mData[3][3]*rhs.mData[1][3],
        mData[0][3]*rhs.mData[2][0]
        + mData[1][3]*rhs.mData[2][1]
        + mData[2][3]*rhs.mData[2][2]
        + mData[3][3]*rhs.mData[2][3],
        mData[0][3]*rhs.mData[3][0]
        + mData[1][3]*rhs.mData[3][1]
        + mData[2][3]*rhs.mData[3][2]
        + mData[3][3]*rhs.mData[3][3]);
}

Matrix4f 
Matrix4f::operator/(float c) const
{
    return Matrix4f(
        mData[0][0]/c, mData[1][0]/c, mData[2][0]/c, mData[3][0]/c,
        mData[0][1]/c, mData[1][1]/c, mData[2][1]/c, mData[3][1]/c,
        mData[0][2]/c, mData[1][2]/c, mData[2][2]/c, mData[3][2]/c,
        mData[0][3]/c, mData[1][3]/c, mData[2][3]/c, mData[3][3]/c);
}

Matrix4f &
Matrix4f::operator/=(float c) 
{
    mData[0][0] /= c;
    mData[0][1] /= c;
    mData[0][2] /= c;
    mData[0][3] /= c;

    mData[1][0] /= c;
    mData[1][1] /= c;
    mData[1][2] /= c;
    mData[1][3] /= c;

    mData[2][0] /= c;
    mData[2][1] /= c;
    mData[2][2] /= c;
    mData[2][3] /= c;

    mData[3][0] /= c;
    mData[3][1] /= c;
    mData[3][2] /= c;
    mData[3][3] /= c;

    return *this;
}

Matrix4f 
Matrix4f::transpose() const
{
    return Matrix4f(
        mData[0][0], mData[0][1], mData[0][2], mData[0][3],
        mData[1][0], mData[1][1], mData[1][2], mData[1][3],
        mData[2][0], mData[2][1], mData[2][2], mData[2][3],
        mData[3][0], mData[3][1], mData[3][2], mData[3][3]);
}

Matrix4f 
Matrix4f::inverse() const
{
    return adjugate()/determinant();
}

Matrix4f
Matrix4f::affineInverse() const
{
    // The matrix must be affine.
    assert(mData[0][3] == 0.0
        && mData[1][3] == 0.0
        && mData[2][3] == 0.0
        && mData[3][3] == 1.0);
    
    float det3x3 = determinant3x3(
        mData[0][0], mData[1][0], mData[2][0],
        mData[0][1], mData[1][1], mData[2][1],
        mData[0][2], mData[1][2], mData[2][2]);

    float a11 = (mData[1][1]*mData[2][2] - mData[1][2]*mData[2][1])/det3x3;
    float a12 = -(mData[1][0]*mData[2][2] - mData[1][2]*mData[2][0])/det3x3;
    float a13 = (mData[1][0]*mData[2][1] - mData[1][1]*mData[2][0])/det3x3;
    float a21 = -(mData[0][1]*mData[2][2] - mData[0][2]*mData[2][1])/det3x3;
    float a22 = (mData[0][0]*mData[2][2] - mData[0][2]*mData[2][0])/det3x3;
    float a23 = -(mData[0][0]*mData[2][1] - mData[0][1]*mData[2][0])/det3x3;
    float a31 = (mData[0][1]*mData[1][2] - mData[0][2]*mData[1][1])/det3x3;
    float a32 = -(mData[0][0]*mData[1][2] - mData[0][2]*mData[1][0])/det3x3;
    float a33 = (mData[0][0]*mData[1][1] - mData[0][1]*mData[1][0])/det3x3;
        
    return Matrix4f(
        a11, a12, a13, -(mData[3][0]*a11 + mData[3][1]*a12 + mData[3][2]*a13),
        a21, a22, a23, -(mData[3][0]*a21 + mData[3][1]*a22 + mData[3][2]*a23),
        a31, a32, a33, -(mData[3][0]*a31 + mData[3][1]*a32 + mData[3][2]*a33),
        0, 0, 0, 1
        );
}

Matrix4f 
Matrix4f::adjugate() const
{
    return Matrix4f(
        determinant3x3(
            mData[1][1], mData[2][1], mData[3][1], 
            mData[1][2], mData[2][2], mData[3][2], 
            mData[1][3], mData[2][3], mData[3][3]),
        -determinant3x3(
            mData[1][0], mData[2][0], mData[3][0], 
            mData[1][2], mData[2][2], mData[3][2], 
            mData[1][3], mData[2][3], mData[3][3]),
        determinant3x3(
            mData[1][0], mData[2][0], mData[3][0], 
            mData[1][1], mData[2][1], mData[3][1], 
            mData[1][3], mData[2][3], mData[3][3]),
        -determinant3x3(
            mData[1][0], mData[2][0], mData[3][0], 
            mData[1][1], mData[2][1], mData[3][1], 
            mData[1][2], mData[2][2], mData[3][2]),

        -determinant3x3(
            mData[0][1], mData[2][1], mData[3][1], 
            mData[0][2], mData[2][2], mData[3][2], 
            mData[0][3], mData[2][3], mData[3][3]),
        determinant3x3(
            mData[0][0], mData[2][0], mData[3][0], 
            mData[0][2], mData[2][2], mData[3][2], 
            mData[0][3], mData[2][3], mData[3][3]),
        -determinant3x3(
            mData[0][0], mData[2][0], mData[3][0], 
            mData[0][1], mData[2][1], mData[3][1], 
            mData[0][3], mData[2][3], mData[3][3]),
        determinant3x3(
            mData[0][0], mData[2][0], mData[3][0], 
            mData[0][1], mData[2][1], mData[3][1], 
            mData[0][2], mData[2][2], mData[3][2]),

        determinant3x3(
            mData[0][1], mData[1][1], mData[3][1], 
            mData[0][2], mData[1][2], mData[3][2], 
            mData[0][3], mData[1][3], mData[3][3]),
        -determinant3x3(
            mData[0][0], mData[1][0], mData[3][0], 
            mData[0][2], mData[1][2], mData[3][2], 
            mData[0][3], mData[1][3], mData[3][3]),
        determinant3x3(
            mData[0][0], mData[1][0], mData[3][0], 
            mData[0][1], mData[1][1], mData[3][1], 
            mData[0][3], mData[1][3], mData[3][3]),
        -determinant3x3(
            mData[0][0], mData[1][0], mData[3][0], 
            mData[0][1], mData[1][1], mData[3][1], 
            mData[0][2], mData[1][2], mData[3][2]),

        -determinant3x3(
            mData[0][1], mData[1][1], mData[2][1], 
            mData[0][2], mData[1][2], mData[2][2], 
            mData[0][3], mData[1][3], mData[2][3]),
        determinant3x3(
            mData[0][0], mData[1][0], mData[2][0], 
            mData[0][2], mData[1][2], mData[2][2], 
            mData[0][3], mData[1][3], mData[2][3]),
        -determinant3x3(
            mData[0][0], mData[1][0], mData[2][0], 
            mData[0][1], mData[1][1], mData[2][1], 
            mData[0][3], mData[1][3], mData[2][3]),
        determinant3x3(
            mData[0][0], mData[1][0], mData[2][0], 
            mData[0][1], mData[1][1], mData[2][1], 
            mData[0][2], mData[1][2], mData[2][2])
        );
}

float 
Matrix4f::determinant() const
{
    return 
        mData[0][0]*determinant3x3(
            mData[1][1], mData[1][2], mData[1][3], 
            mData[2][1], mData[2][2], mData[2][3], 
            mData[3][1], mData[3][2], mData[3][3])
        - mData[1][0]*determinant3x3(
            mData[0][1], mData[0][2], mData[0][3], 
            mData[2][1], mData[2][2], mData[2][3], 
            mData[3][1], mData[3][2], mData[3][3])
        + mData[2][0]*determinant3x3(
            mData[0][1], mData[0][2], mData[0][3], 
            mData[1][1], mData[1][2], mData[1][3], 
            mData[3][1], mData[3][2], mData[3][3])
        - mData[3][0]*determinant3x3(
            mData[0][1], mData[0][2], mData[0][3], 
            mData[1][1], mData[1][2], mData[1][3], 
            mData[2][1], mData[2][2], mData[2][3]);
}

std::ostream &
operator<<(std::ostream &ostr, const Matrix4f &rhs) 
{
    cgmath::WriteMatrix(ostr, 4, 4, &rhs.mData[0][0]);

    return ostr;
}

const Matrix4f Matrix4f::ZERO(
    0, 0, 0, 0, 
    0, 0, 0, 0, 
    0, 0, 0, 0, 
    0, 0, 0, 0);

const Matrix4f Matrix4f::IDENTITY(
    1, 0, 0, 0, 
    0, 1, 0, 0, 
    0, 0, 1, 0, 
    0, 0, 0, 1);

float 
Matrix4f::determinant3x3(
    float a11, float a12, float a13, 
    float a21, float a22, float a23, 
    float a31, float a32, float a33) const
{
    return a11*(a22*a33 - a32*a23)
        + a21*(a32*a13 - a12*a33)
        + a31*(a12*a23 - a22*a13);
}

} // namespace cgmath
