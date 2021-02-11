// Copyright 2008 Drew Olbrich

#ifndef CGMATH__BOUNDING_BOX2F__INCLUDED
#define CGMATH__BOUNDING_BOX2F__INCLUDED

#include "Vector2f.h"

namespace cgmath {

// BoundingBox2f
//
// A 2D bounding box.

class BoundingBox2f
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    BoundingBox2f();
    ~BoundingBox2f();

    BoundingBox2f(float minX, float maxX, float minY, float maxY);
    BoundingBox2f(const Vector2f &min, const Vector2f &max);

    float operator()(int minMax, int axis) const;
    float &operator()(int minMax, int axis);

    // Equality.
    bool operator==(const BoundingBox2f &rhs) const;
    bool operator!=(const BoundingBox2f &rhs) const;

    float minAxis(int axis) const;
    float maxAxis(int axis) const;
    
    void setMinX(float minX);
    float minX() const;

    void setMaxX(float maxX);
    float maxX() const;

    void setMinY(float minY);
    float minY() const;

    void setMaxY(float maxY);
    float maxY() const;

    void setMin(const Vector2f &min);
    const Vector2f &min() const;

    void setMax(const Vector2f &max);
    const Vector2f &max() const;

    // Reset the bounding box to the empty set.
    void reset();

    // Extend the bounding box to include the specified point.
    void extendByVector2f(const Vector2f &point);

    // Extend the bounding box to include the specified bounding box.
    void extendByBoundingBox2f(const BoundingBox2f &rhs);

    // Returns true if the bounding box is empty.
    bool empty() const;

    // Returns the size of the bounding box.
    float sizeX() const;
    float sizeY() const;
    Vector2f size() const;

    // Returns the center of the bounding box.
    Vector2f center() const;
    
    // The empty set.
    static const BoundingBox2f EMPTY_SET;
    
private:
    Vector2f mMin;
    Vector2f mMax;
};

} // namespace cgmath

#endif // CGMATH__BOUNDING_BOX2F__INCLUDED
