// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__MATRIX3F__INCLUDED
#define CGMATH__MATRIX3F__INCLUDED

#include <ostream>

#include "Vector3f.h"

namespace cgmath {

class Matrix4f;

// Matrix3f
//
// This 3x3 matrix class treats vectors as columns. Matrix elements are stored in memory
// in column-major order. Transformations are applied by premultiplication by a matrix. 
// See the header file Matrix4f.h for a more detailed explanation.

class Matrix3f
{
public:
    // For efficiency, the constructor does not initialize the elements of the matrix.
    Matrix3f();
    ~Matrix3f();

    // Copy constructor.
    Matrix3f(const Matrix3f &rhs);

    // Constructor from explicit elements.
    Matrix3f(
        float a11, float a12, float a13,
        float a21, float a22, float a23,
        float a31, float a32, float a33);

    // Constructor from three column vectors.
    Matrix3f(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3);

    // Constructors from column vector matrices stored as nine elements in column-major
    // order. The first three memory elements in the array represent the first column of
    // the matrix. Note that with respect to the arguments passed to the constructor from
    // an explicit list of nine elements, defined above, the array elements are
    // transposed. These constructors are also compatible with row vector matrices stored
    // in row-major order.
    explicit Matrix3f(const float *elements);
    explicit Matrix3f(const float elements[3][3]);

    // Constructor from the rotation component of a 4x4 matrix.
    explicit Matrix3f(const Matrix4f &matrix4f);

    // Transformation matrices. (Rotations are right-handed. That is, positive-valued
    // rotations are counterclockwise when looking down the specified axis toward the
    // origin.)
    static Matrix3f fromRotation(Vector3f axis, float angleInRadians);
    static Matrix3f fromScale(const Vector3f &scale);

    // Return the matrix as a float pointer.
    const float *asFloatPtr() const;
    
    // Element access (column-major order, zero-indexed). If you see a matrix element
    // a(i,j) appear in mathematical notation, this would be referenced using the
    // following operators as a(i-1,j-1). Note that this is the reverse of the C/C++
    // convention for arrays. Therefore, a(1,2) references what is privately referenced
    // within this class as a.mData[1][0].
    float operator()(int row, int column) const;
    float &operator()(int row, int column);

    // Equality.
    bool operator==(const Matrix3f &rhs) const;
    bool operator!=(const Matrix3f &rhs) const;
    
    // Assignment.
    Matrix3f &operator=(const Matrix3f &rhs);
    
    // Assignment of all elements (column-major order).
    void set(
        float a11, float a12, float a13,
        float a21, float a22, float a23,
        float a31, float a32, float a33);

    // Matrix addition.
    Matrix3f operator+(const Matrix3f &rhs) const;
    Matrix3f &operator+=(const Matrix3f &rhs);
    
    // Matrix subtraction.
    Matrix3f operator-(const Matrix3f &rhs) const;
    Matrix3f &operator-=(const Matrix3f &rhs);
    
    // Unary minus.
    Matrix3f operator-() const;

    // Scalar multiplication.
    Matrix3f operator*(float c) const;
    Matrix3f &operator*=(float c);
    friend Matrix3f operator*(float c, const Matrix3f &rhs);

    // Premultiply a vector by a matrix. Vectors are treated as columns.
    // This operator should be used to transform a vector. 
    Vector3f operator*(const Vector3f &rhs) const;

    // Postmultiply a vector by a matrix. Vectors are treated as rows.
    friend Vector3f operator*(const Vector3f &lhs, const Matrix3f &rhs);

    // Matrix multiplication. Matrices are composed by premultiplying.
    Matrix3f operator*(const Matrix3f &rhs) const;

    // Scalar division.
    Matrix3f operator/(float c) const;
    Matrix3f &operator/=(float c);

    // Transpose.
    Matrix3f transpose() const;

    // Inverse.
    Matrix3f inverse() const;
    
    // Adjugate. (Also known as adjoint, but that term is supposedly ambiguous).
    Matrix3f adjugate() const;

    // Determinant.
    float determinant() const;
    
    // Output to a stream.
    friend std::ostream &operator<<(std::ostream &ostr, const Matrix3f &rhs);

    // The zero matrix.
    static const Matrix3f ZERO;

    // The identity matrix.
    static const Matrix3f IDENTITY;

private:
    // Determinant of a 2x2 matrix.
    float determinant2x2(
        float a11, float a12, 
        float a21, float a22) const;

    // The elements of the matrix in column-major order, accessed as mData[column][row].
    float mData[3][3];
};

} // namespace cgmath

#endif // CGMATH__MATRIX3F__INCLUDED
