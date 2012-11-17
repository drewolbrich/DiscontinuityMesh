// Copyright 2009 Retroactive Fiasco.

#include "HemisphericalPointDistributor.h"

#include <cassert>
#include <cmath>

#include "Vector2f.h"
#include "Vector3f.h"
#include "CircleOperations.h"

namespace cgmath {

HemisphericalPointDistributor::HemisphericalPointDistributor()
    : mPointCount(0),
      mDistribution(COSINE),
      mPointVector()
{
}

HemisphericalPointDistributor::~HemisphericalPointDistributor()
{
}

void
HemisphericalPointDistributor::setPointCount(unsigned pointCount)
{
    // The number of points must be a perfect square.
    assert(rint(sqrt(pointCount)) == sqrt(pointCount));

    mPointCount = pointCount;
}

unsigned
HemisphericalPointDistributor::pointCount() const
{
    return mPointCount;
}

void
HemisphericalPointDistributor::setDistribution(Distribution distribution)
{
    mDistribution = distribution;
}

HemisphericalPointDistributor::Distribution
HemisphericalPointDistributor::distribution() const
{
    return mDistribution;
}

void
HemisphericalPointDistributor::initialize()
{
    int n = sqrt(mPointCount);

    mPointVector.clear();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {

            cgmath::Vector2f pointOnCircle = MapConcentricSquareToConcentricCircle(
                cgmath::Vector2f((i + drand48())/n, (j + drand48())/n));

            float u = pointOnCircle[0];
            float v = pointOnCircle[1];

            float r = sqrtf(u*u + v*v);

            cgmath::Vector3f pointOnHemisphere;
            switch (mDistribution) {
            case COSINE:
                pointOnHemisphere = cgmath::Vector3f(
                    u,
                    v,
                    sqrtf(1.0 - r*r));
                break;
            case UNIFORM:
                pointOnHemisphere = cgmath::Vector3f(
                    u*sqrtf(2.0 - r*r),
                    v*sqrtf(2.0 - r*r),
                    1.0 - r*r);
                break;
            }

            mPointVector.push_back(pointOnHemisphere);
        }
    }
}

const cgmath::Vector3f &
HemisphericalPointDistributor::getPoint(unsigned index)
{
    assert(index < mPointVector.size());

    return mPointVector[index];
}

} // namespace cgmath
