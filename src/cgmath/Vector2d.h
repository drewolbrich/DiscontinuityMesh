// Copyright 2008 Drew Olbrich

#ifndef CGMATH__VECTOR2D__INCLUDED
#define CGMATH__VECTOR2D__INCLUDED

#include <ostream>

namespace cgmath {

#if 0
class Vector3f;
#endif

class Vector2f;

// Vector2d
//
// A 2D vector implemented in terms of double precision floating point values.

class Vector2d
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    Vector2d();
    ~Vector2d();

    // Constructors.
    Vector2d(const Vector2d &rhs);
    explicit Vector2d(const Vector2f &rhs);
    Vector2d(double c1, double c2);
#if 0
    Vector2d(const Vector3f &rhs, int axis1, int axis2);
#endif

    // Constructor from a two component array.
    explicit Vector2d(const double *components);

    // Return the vector as a double pointer.
    const double *asDoublePtr() const;

    // Assignment.
    Vector2d &operator=(const Vector2d &rhs);
    void swap(Vector2d &rhs);
    
    // Assignment of all components.
    void set(double c1, double c2);

    // Component access (zero-indexed).
    double operator[](int index) const;
    double &operator[](int index);

    // Equality.
    bool operator==(const Vector2d &rhs) const;
    bool operator!=(const Vector2d &rhs) const;

    // Returns true if the two vectors are equivalent to within
    // the specified tolerance.
    bool equivalent(const Vector2d &rhs, float epsilon) const;

    // Comparison. Required for STL containers.
    bool operator<(const Vector2d &rhs) const;

    // Vector addition.
    Vector2d operator+(const Vector2d &rhs) const;
    Vector2d &operator+=(const Vector2d &rhs);

    // Vector subtraction.
    Vector2d operator-(const Vector2d &rhs) const;
    Vector2d &operator-=(const Vector2d &rhs);

    // Unary minus.
    Vector2d operator-() const;

    // Scalar multiplication.
    Vector2d operator*(double c) const;
    Vector2d &operator*=(double c);
    friend Vector2d operator*(double c, const Vector2d &rhs);

    // Vector multiplication.
    Vector2d operator*(const Vector2d &rhs) const;
    Vector2d &operator*=(const Vector2d &rhs);

    // Scalar division.
    Vector2d operator/(double c) const;
    Vector2d &operator/=(double c);

    // Dot product.
    double dot(const Vector2d &rhs) const;

    // Length.
    double length() const;
    double lengthSquared() const;

    // Normalization (in place).
    Vector2d &normalize();

    // Normalization (returned).
    Vector2d normalized() const;

    // The maximum absolute value of the vector's components.
    double maxAbs() const;

    // Stream output.
    friend std::ostream &operator<<(std::ostream &ostr, const Vector2d &rhs);

    // The zero vector.
    static const Vector2d ZERO;

    // The unit vector.
    static const Vector2d UNIT;

private:
    // The components of the vector.
    double mData[2];
};

} // namespace cgmath

#endif // CGMATH__VECTOR2D__INCLUDED
