// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__VECTOR3F__INCLUDED
#define CGMATH__VECTOR3F__INCLUDED

#include <ostream>

namespace cgmath {

class Vector3d;
class Vector4f;

// Vector3f
//
// A 3D vector.

class Vector3f
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    Vector3f();
    ~Vector3f();

    // Constructors.
    Vector3f(const Vector3f &rhs);
    explicit Vector3f(const Vector3d &rhs);
    explicit Vector3f(const Vector4f &rhs);
    Vector3f(float c1, float c2, float c3);

    // Constructor from a three component array.
    explicit Vector3f(const float *components);

    // Return the vector as a float pointer.
    const float *asFloatPtr() const;
    
    // Assignment.
    Vector3f &operator=(const Vector3f &rhs);
    void swap(Vector3f &rhs);
    
    // Assignment of all components.
    void set(float c1, float c2, float c3);

    // Component access (zero-indexed).
    float operator[](int index) const;
    float &operator[](int index);

    // Equality.
    bool operator==(const Vector3f &rhs) const;
    bool operator!=(const Vector3f &rhs) const;

    // Returns true if the two vectors are equivalent to within
    // the specified tolerance.
    bool equivalent(const Vector3f &rhs, float epsilon) const;

    // Comparison. Required for STL containers.
    bool operator<(const Vector3f &rhs) const;

    // Vector addition.
    Vector3f operator+(const Vector3f &rhs) const;
    Vector3f &operator+=(const Vector3f &rhs);

    // Vector subtraction.
    Vector3f operator-(const Vector3f &rhs) const;
    Vector3f &operator-=(const Vector3f &rhs);

    // Unary minus.
    Vector3f operator-() const;

    // Scalar multiplication.
    Vector3f operator*(float c) const;
    Vector3f &operator*=(float c);
    friend Vector3f operator*(float c, const Vector3f &rhs);

    // Vector multiplication.
    Vector3f operator*(const Vector3f &rhs) const;
    Vector3f &operator*=(const Vector3f &rhs);
    
    // Scalar division.
    Vector3f operator/(float c) const;
    Vector3f &operator/=(float c);

    // Dot product.
    float dot(const Vector3f &rhs) const;

    // Cross product.
    Vector3f cross(const Vector3f &rhs) const;

    // Length.
    float length() const;
    float lengthSquared() const;

    // Normalization (in place).
    Vector3f &normalize();

    // Normalization (returned).
    Vector3f normalized() const;

    // The maximum absolute value of the vector's components.
    float maxAbs() const;

    // Stream output.
    friend std::ostream &operator<<(std::ostream &ostr, const Vector3f &rhs);

    // The zero vector.
    static const Vector3f ZERO;

    // The unit vector.
    static const Vector3f UNIT;

private:
    // The components of the vector.
    float mData[3];
};

} // namespace cgmath

#endif // CGMATH__VECTOR3F__INCLUDED
