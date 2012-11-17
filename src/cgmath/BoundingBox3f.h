// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__BOUNDING_BOX3F__INCLUDED
#define CGMATH__BOUNDING_BOX3F__INCLUDED

#include "Vector3f.h"

namespace cgmath {

// BoundingBox3f
//
// A 3D bounding box.

class BoundingBox3f
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    BoundingBox3f();
    ~BoundingBox3f();

    BoundingBox3f(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
    BoundingBox3f(const Vector3f &min, const Vector3f &max);

    float operator()(int minMax, int axis) const;
    float &operator()(int minMax, int axis);

    // Equality.
    bool operator==(const BoundingBox3f &rhs) const;
    bool operator!=(const BoundingBox3f &rhs) const;

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

    void setMinZ(float minZ);
    float minZ() const;

    void setMaxZ(float maxZ);
    float maxZ() const;

    void setMin(const Vector3f &min);
    const Vector3f &min() const;

    void setMax(const Vector3f &max);
    const Vector3f &max() const;

    // Reset the bounding box to the empty set.
    void reset();

    // Extend the bounding box to include the specified point.
    void extendByVector3f(const Vector3f &point);

    // Extend the bounding box to include the specified bounding box.
    void extendByBoundingBox3f(const BoundingBox3f &rhs);

    // Returns true if the bounding box is empty.
    bool empty() const;

    // Returns the size of the bounding box.
    float sizeX() const;
    float sizeY() const;
    float sizeZ() const;
    Vector3f size() const;

    // Returns the center of the bounding box.
    Vector3f center() const;
    
    // The empty set.
    static const BoundingBox3f EMPTY_SET;
    
private:
    Vector3f mMin;
    Vector3f mMax;
};

} // namespace cgmath

#endif // CGMATH__BOUNDING_BOX3F__INCLUDED
