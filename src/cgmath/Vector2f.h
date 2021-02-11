// Copyright 2008 Drew Olbrich

#ifndef CGMATH__VECTOR2F__INCLUDED
#define CGMATH__VECTOR2F__INCLUDED

#include <ostream>

namespace cgmath {

class Vector3f;

// Vector2f
//
// A 2D vector.

class Vector2f
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    Vector2f();
    ~Vector2f();

    // Constructors.
    Vector2f(const Vector2f &rhs);
    Vector2f(float c1, float c2);
    Vector2f(const Vector3f &rhs, int axis1, int axis2);

    // Constructor from a two component array.
    explicit Vector2f(const float *components);

    // Return the vector as a float pointer.
    const float *asFloatPtr() const;

    // Assignment.
    Vector2f &operator=(const Vector2f &rhs);
    void swap(Vector2f &rhs);
    
    // Assignment of all components.
    void set(float c1, float c2);

    // Component access (zero-indexed).
    float operator[](int index) const;
    float &operator[](int index);

    // Equality.
    bool operator==(const Vector2f &rhs) const;
    bool operator!=(const Vector2f &rhs) const;

    // Returns true if the two vectors are equivalent to within
    // the specified tolerance.
    bool equivalent(const Vector2f &rhs, float epsilon) const;

    // Comparison. Required for STL containers.
    bool operator<(const Vector2f &rhs) const;

    // Vector addition.
    Vector2f operator+(const Vector2f &rhs) const;
    Vector2f &operator+=(const Vector2f &rhs);

    // Vector subtraction.
    Vector2f operator-(const Vector2f &rhs) const;
    Vector2f &operator-=(const Vector2f &rhs);

    // Unary minus.
    Vector2f operator-() const;

    // Scalar multiplication.
    Vector2f operator*(float c) const;
    Vector2f &operator*=(float c);
    friend Vector2f operator*(float c, const Vector2f &rhs);

    // Vector multiplication.
    Vector2f operator*(const Vector2f &rhs) const;
    Vector2f &operator*=(const Vector2f &rhs);

    // Scalar division.
    Vector2f operator/(float c) const;
    Vector2f &operator/=(float c);

    // Dot product.
    float dot(const Vector2f &rhs) const;

    // Length.
    float length() const;
    float lengthSquared() const;

    // Normalization (in place).
    Vector2f &normalize();

    // Normalization (returned).
    Vector2f normalized() const;

    // The maximum absolute value of the vector's components.
    float maxAbs() const;

    // Stream output.
    friend std::ostream &operator<<(std::ostream &ostr, const Vector2f &rhs);

    // The zero vector.
    static const Vector2f ZERO;

    // The unit vector.
    static const Vector2f UNIT;

private:
    // The components of the vector.
    float mData[2];
};

} // namespace cgmath

#endif // CGMATH__VECTOR2F__INCLUDED
