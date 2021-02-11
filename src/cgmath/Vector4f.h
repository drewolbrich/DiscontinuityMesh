// Copyright 2008 Drew Olbrich

#ifndef CGMATH__VECTOR4F__INCLUDED
#define CGMATH__VECTOR4F__INCLUDED

#include <ostream>

namespace cgmath {

class Vector3f;

// Vector4f
//
// A 4D vector.

class Vector4f
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    Vector4f();
    ~Vector4f();

    // Constructors.
    Vector4f(const Vector4f &rhs);
    explicit Vector4f(const Vector3f &rhs);
    Vector4f(float c1, float c2, float c3, float c4);

    // Constructor from a four component array.
    explicit Vector4f(const float *components);

    // Return the vector as a float pointer.
    const float *asFloatPtr() const;

    // Assignment.
    Vector4f &operator=(const Vector4f &rhs);
    void swap(Vector4f &rhs);

    // Assignment of all components.
    void set(float c1, float c2, float c3, float c4);

    // Component access (zero-indexed).
    float operator[](int index) const;
    float &operator[](int index);

    // Equality.
    bool operator==(const Vector4f &rhs) const;
    bool operator!=(const Vector4f &rhs) const;

    // Returns true if the two vectors are equivalent to within
    // the specified tolerance.
    bool equivalent(const Vector4f &rhs, float epsilon) const;

    // Comparison. Required for STL containers.
    bool operator<(const Vector4f &rhs) const;

    // Vector addition.
    Vector4f operator+(const Vector4f &rhs) const;
    Vector4f &operator+=(const Vector4f &rhs);

    // Vector subtraction.
    Vector4f operator-(const Vector4f &rhs) const;
    Vector4f &operator-=(const Vector4f &rhs);

    // Unary minus.
    Vector4f operator-() const;

    // Scalar multiplication.
    Vector4f operator*(float c) const;
    Vector4f &operator*=(float c);
    friend Vector4f operator*(float c, const Vector4f &rhs);

    // Vector multiplication.
    Vector4f operator*(const Vector4f &rhs) const;
    Vector4f &operator*=(const Vector4f &rhs);

    // Scalar division.
    Vector4f operator/(float c) const;
    Vector4f &operator/=(float c);

    // Dot product.
    float dot(const Vector4f &rhs) const;

    // Length.
    float length() const;
    float lengthSquared() const;

    // Normalization (in place).
    Vector4f &normalize();

    // Normalization (returned).
    Vector4f normalized() const;

    // The maximum absolute value of the vector's components.
    float maxAbs() const;

    // Stream output.
    friend std::ostream &operator<<(std::ostream &ostr, const Vector4f &rhs);

    // The zero vector.
    static const Vector4f ZERO;

    // The unit vector.
    static const Vector4f UNIT;

private:
    // The components of the vector.
    float mData[4];
};

} // namespace cgmath

#endif // CGMATH__VECTOR4F__INCLUDED
