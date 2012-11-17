// Copyright 2009 Retroactive Fiasco.

#ifndef CGMATH__HEMISPHERICAL_POINT_DISTRIBUTOR__INCLUDED
#define CGMATH__HEMISPHERICAL_POINT_DISTRIBUTOR__INCLUDED

#include <vector>

#include "Vector3f.h"

namespace cgmath {

// HemisphericalPointDistributor
//
// Class that generates a jittered set of points on a hemisphere
// centered around the +Z axis.

class HemisphericalPointDistributor
{
public:
    HemisphericalPointDistributor();
    ~HemisphericalPointDistributor();

    // The number of points to create. This must be a perfect square.
    void setPointCount(unsigned pointCount);
    unsigned pointCount() const;

    // The distribution of the points on the hemisphere.
    enum Distribution {
        // Density is proportional to the Z coordinates.
        COSINE,
        // Uniform distribution.
        UNIFORM
    };
    void setDistribution(Distribution distribution);
    Distribution distribution() const;

    // Create the vector of points.
    void initialize();

    // Return one of the points.
    const cgmath::Vector3f &getPoint(unsigned index);

private:
    unsigned mPointCount;
    Distribution mDistribution;

    std::vector<Vector3f> mPointVector;
};

} // namespace cgmath

#endif // CGMATH__HEMISPHERICAL_POINT_DISTRIBUTOR__INCLUDED
