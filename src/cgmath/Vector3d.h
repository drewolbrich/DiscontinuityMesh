// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__VECTOR3D__INCLUDED
#define CGMATH__VECTOR3D__INCLUDED

#include <ostream>

namespace cgmath {

class Vector3f;
#if 0
class Vector4d;
#endif

// Vector3d
//
// A 3D vector implemented in terms of double precision floating point values.

class Vector3d
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    Vector3d();
    ~Vector3d();

    // Constructors.
    Vector3d(const Vector3d &rhs);
    explicit Vector3d(const Vector3f &rhs);
#if 0
    explicit Vector3d(const Vector4d &rhs);
#endif
    Vector3d(double c1, double c2, double c3);

    // Constructor from a three component array.
    explicit Vector3d(const double *components);

    // Return the vector as a double pointer.
    const double *asDoublePtr() const;
    
    // Assignment.
    Vector3d &operator=(const Vector3d &rhs);
    void swap(Vector3d &rhs);
    
    // Assignment of all components.
    void set(double c1, double c2, double c3);

    // Component access (zero-indexed).
    double operator[](int index) const;
    double &operator[](int index);

    // Equality.
    bool operator==(const Vector3d &rhs) const;
    bool operator!=(const Vector3d &rhs) const;

    // Returns true if the two vectors are equivalent to within
    // the specified tolerance.
    bool equivalent(const Vector3d &rhs, float epsilon) const;

    // Comparison. Required for STL containers.
    bool operator<(const Vector3d &rhs) const;

    // Vector addition.
    Vector3d operator+(const Vector3d &rhs) const;
    Vector3d &operator+=(const Vector3d &rhs);

    // Vector subtraction.
    Vector3d operator-(const Vector3d &rhs) const;
    Vector3d &operator-=(const Vector3d &rhs);

    // Unary minus.
    Vector3d operator-() const;

    // Scalar multiplication.
    Vector3d operator*(double c) const;
    Vector3d &operator*=(double c);
    friend Vector3d operator*(double c, const Vector3d &rhs);

    // Vector multiplication.
    Vector3d operator*(const Vector3d &rhs) const;
    Vector3d &operator*=(const Vector3d &rhs);
    
    // Scalar division.
    Vector3d operator/(double c) const;
    Vector3d &operator/=(double c);

    // Dot product.
    double dot(const Vector3d &rhs) const;

    // Cross product.
    Vector3d cross(const Vector3d &rhs) const;

    // Length.
    double length() const;
    double lengthSquared() const;

    // Normalization (in place).
    Vector3d &normalize();

    // Normalization (returned).
    Vector3d normalized() const;

    // The maximum absolute value of the vector's components.
    double maxAbs() const;

    // Stream output.
    friend std::ostream &operator<<(std::ostream &ostr, const Vector3d &rhs);

    // The zero vector.
    static const Vector3d ZERO;

    // The unit vector.
    static const Vector3d UNIT;

private:
    // The components of the vector.
    double mData[3];
};

} // namespace cgmath

#endif // CGMATH__VECTOR3D__INCLUDED
