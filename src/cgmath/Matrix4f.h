// Copyright 2008 Drew Olbrich

#ifndef CGMATH__MATRIX4F__INCLUDED
#define CGMATH__MATRIX4F__INCLUDED

#include <ostream>

#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix3f.h"

namespace cgmath {

// Matix4f
//
// This 4x4 matrix class treats vectors as columns. This is the convention used by Foley
// and Van Dam (second edition), the OpenGL documentation, and linear algebra texts. The
// Graphics Gems series of books and Direct3D use the opposite convention.
//
// Vectors are transformed by premultiplying them by a matrix. For example:
//
//     transformedVector = matrix*vector;
//
// A sequence of transformation matrices are composed by premultiplying them. For example,
// the three matrices m1, m2, and m3 would be composed to form matrix m4 as follows:
//
//     m4 = m3*m2*m1;
//
// If a vector was transformed by matrix m4, this would be the same as transforming the
// vector by m1, then m2, then m3.
//
// Matrices are stored in memory in column-major order. This provides compatibility with
// the OpenGL function glLoadMatrix.
//
// Note that this is the opposite of the C/C++ convention for arrays, which are stored in
// row-major order.
//
// A matrix that would be expressed mathematically (with column vectors) as
//
//     | a11 a12 a13 a14 |
//     | a21 a22 a23 a24 |
//     | a31 a32 a33 a34 |
//     | a41 a42 a43 a44 |
//
// would be represented in memory in column-major order as
//
//     float matrixElements[16] = {
//         a11, a21, a31, a41,
//         a12, a22, a32, a42,
//         a13, a23, a33, a43,
//         a14, a24, a34, a44
//     };
//
// In particular, in a translation matrix, the x, y, and z translation values would be
// stored in elements a14, a24, and a34 above.
//
// Note also that this memory layout is compatible with systems like Direct3D that use a
// row vector convention in conjunction with a row-major order memory layout. However, the
// order of the arguments for some of the member functions in this class will appear to
// be reversed.
//
// Rotations are right-handed.

class Matrix4f
{
public:
    // For efficiency, the constructor does not initialize the elements of the matrix.
    Matrix4f();
    ~Matrix4f();

    // Copy constructor.
    Matrix4f(const Matrix4f &rhs);

    // Constructor from explicit elements.
    Matrix4f(
        float a11, float a12, float a13, float a14,
        float a21, float a22, float a23, float a24,
        float a31, float a32, float a33, float a34,
        float a41, float a42, float a43, float a44);

    // Constructor from four column vectors.
    Matrix4f(const Vector3f &v1, const Vector3f &v2, const Vector3f &v3, const Vector3f &v4);
    Matrix4f(const Vector4f &v1, const Vector4f &v2, const Vector4f &v3, const Vector4f &v4);

    // Constructor from a 3x3 matrix.
    explicit Matrix4f(const Matrix3f &rhs);
    
    // Constructors from column vector matrices stored as sixteen elements in column-major
    // order. The first four memory elements in the array represent the first column of
    // the matrix. Note that with respect to the arguments passed to the constructor from
    // an explicit list of sixteen elements, defined above, the array elements are
    // transposed. These constructors are also compatible with row vector matrices stored
    // in row-major order.
    explicit Matrix4f(const float *elements);
    explicit Matrix4f(const float elements[4][4]);
    
    // Transformation matrices. (Rotations are right-handed. That is, positive-valued
    // rotations are counterclockwise when looking down the specified axis toward the
    // origin.)
    static Matrix4f fromTranslation(const Vector3f &vector);
    static Matrix4f fromRotation(Vector3f axis, float angleInRadians);
    static Matrix4f fromScale(const Vector3f &scale);

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
    bool operator==(const Matrix4f &rhs) const;
    bool operator!=(const Matrix4f &rhs) const;
    
    // Assignment.
    Matrix4f &operator=(const Matrix4f &rhs);
    
    // Assignment of all elements.
    void set(
        float a11, float a12, float a13, float a14,
        float a21, float a22, float a23, float a24,
        float a31, float a32, float a33, float a34,
        float a41, float a42, float a43, float a44);

    // Matrix addition.
    Matrix4f operator+(const Matrix4f &rhs) const;
    Matrix4f &operator+=(const Matrix4f &rhs);
    
    // Matrix subtraction.
    Matrix4f operator-(const Matrix4f &rhs) const;
    Matrix4f &operator-=(const Matrix4f &rhs);
    
    // Unary minus.
    Matrix4f operator-() const;

    // Scalar multiplication.
    Matrix4f operator*(float c) const;
    Matrix4f &operator*=(float c);
    friend Matrix4f operator*(float c, const Matrix4f &rhs);

    // Premultiply a Vector3f by a matrix. Vectors are treated as columns.
    // This operator should be used to transform a vector. 
    // Vector3f's represent homogeneous coordinates. After the multiplication,
    // the X, Y, and Z components are divided by the homogeneous component W.
    Vector3f operator*(const Vector3f &rhs) const;

    // Premultiply a Vector4f by a matrix. Vectors are treated as columns.
    Vector4f operator*(const Vector4f &rhs) const;

    // Postmultiply a Vector3f by a matrix. Vectors are treated as rows.
    // Vector3f's are treated as homogeneous coordinates. After the multiplication,
    // the X, Y, and Z components are divided by the homogeneous component W.
    friend Vector3f operator*(const Vector3f &lhs, const Matrix4f &rhs);

    // Postmultiply Vector4f by a matrix. Vectors are treated as rows.
    friend Vector4f operator*(const Vector4f &lhs, const Matrix4f &rhs);

    // Matrix multiplication. Matrices are composed by premultiplying.
    Matrix4f operator*(const Matrix4f &rhs) const;

    // Scalar division.
    Matrix4f operator/(float c) const;
    Matrix4f &operator/=(float c);

    // Transpose.
    Matrix4f transpose() const;

    // Inverse.
    Matrix4f inverse() const;

    // Faster calculation of the inverse, assuming the matrix is affine. (The bottom row
    // must be 0 0 0 1.)
    Matrix4f affineInverse() const;
    
    // Adjugate. (Also known as adjoint, but that term is supposedly ambiguous).
    Matrix4f adjugate() const;

    // Determinant.
    float determinant() const;
    
    // Output to a stream.
    friend std::ostream &operator<<(std::ostream &ostr, const Matrix4f &rhs);

    // The zero matrix.
    static const Matrix4f ZERO;

    // The identity matrix.
    static const Matrix4f IDENTITY;

private:
    // Determinant of a 3x3 matrix.
    float determinant3x3(
        float a11, float a12, float a13, 
        float a21, float a22, float a23, 
        float a31, float a32, float a33) const;

    // The elements of the matrix in column-major order, accessed as mData[column][row].
    float mData[4][4];
};

} // namespace cgmath

#endif // CGMATH__MATRIX4F__INCLUDED
