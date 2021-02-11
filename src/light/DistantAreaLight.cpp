// Copyright 2008 Drew Olbrich

#include "DistantAreaLight.h"

#include <cmath>
#include <cassert>

#include <cgmath/Constants.h>

namespace light {

const float DistantAreaLight::DEFAULT_ANGULAR_DIAMETER = 32.25;
const int DistantAreaLight::DEFAULT_SIDES = 6;

DistantAreaLight::DistantAreaLight()
    : mPosition(0.57735026919, 0.57735026919, 0.57735026919),
      mUp(0.0, 1.0, 0.0),
      mRoll(0.0),
      mAngularDiameter(DEFAULT_ANGULAR_DIAMETER),
      mSides(DEFAULT_SIDES),
      mIntensity(1.0),
      mColor(1.0, 1.0, 1.0),
      mSceneDiameter(0.0),
      mVertexOffsetVector(),
      mCenter()
{
}

DistantAreaLight::~DistantAreaLight()
{
}

void
DistantAreaLight::setPosition(const cgmath::Vector3f &position)
{
    mPosition = position;
}

const cgmath::Vector3f &
DistantAreaLight::position() const
{
    return mPosition;
}

void
DistantAreaLight::setPositionFromAzimuthAndElevation(float azimuth, float elevation)
{
    // Convert the angles from degrees to radians.
    elevation *= cgmath::PI/180.0;
    azimuth *= cgmath::PI/180.0;

    // From http://en.wikipedia.org/wiki/Solar_azimuth_angle
    // "The solar azimuth angle is the azimuth angle of the sun. It is most often defined
    // as the angle between the line from the observer to the sun projected on the ground
    // and the line from the observer due south. A positive azimuth angle generally
    // indicates the sun is east of south, and a negative azimuth angle generally
    // indicates the sun is west of south."

    mPosition[0] = cos(elevation)*sin(azimuth);
    mPosition[1] = sin(elevation);
    mPosition[2] = cos(elevation)*-cos(azimuth);
}

void
DistantAreaLight::setUp(const cgmath::Vector3f &up)
{
    mUp = up;
}

const cgmath::Vector3f &
DistantAreaLight::up() const
{
    return mUp;
}

void
DistantAreaLight::setRoll(float roll)
{
    mRoll = roll;
}

float
DistantAreaLight::roll() const
{
    return mRoll;
}

void
DistantAreaLight::setAngularDiameter(float angularDiameter)
{
    mAngularDiameter = angularDiameter;
}

void
DistantAreaLight::setSides(int sides)
{
    mSides = sides;
}

int
DistantAreaLight::sides() const
{
    return mSides;
}

float
DistantAreaLight::angularDiameter() const
{
    return mAngularDiameter;
}

void
DistantAreaLight::setIntensity(float intensity)
{
    mIntensity = intensity;
}

float
DistantAreaLight::intensity() const
{
    return mIntensity;
}

void
DistantAreaLight::setColor(const cgmath::Vector3f &color)
{
    mColor = color;
}

const cgmath::Vector3f &
DistantAreaLight::color() const
{
    return mColor;
}

void
DistantAreaLight::setSceneDiameter(float sceneDiameter)
{
    mSceneDiameter = sceneDiameter;
}

float
DistantAreaLight::sceneDiameter() const
{
    return mSceneDiameter;
}

void
DistantAreaLight::prepareForVertexCalculation()
{
    assert(mSides > 2);

    float degrees = mAngularDiameter/60.0;
    float radians = degrees/180.0*cgmath::PI;

    // Compute the radius of a circle with the specified angular diameter in arc minutes.
    float r = mSceneDiameter*tan(radians/2.0);

    // Compute the radius of an isogonal polygon (measured from the center to
    // each vertex) having the same area as a circle with the radius calculated above.
    r = sqrt(2.0*cgmath::PI*r*r/sin(2.0*cgmath::PI/mSides)/mSides);

    cgmath::Vector3f x = mUp.cross(-mPosition).normalized();
    cgmath::Vector3f y = -mPosition.cross(x).normalized();
    cgmath::Vector3f z = x.cross(y).normalized();

    mVertexOffsetVector.resize(mSides);
    for (int index = 0; index < mSides; ++index) {
        float angle = float(index)/mSides*2.0*cgmath::PI + mRoll/180.0*cgmath::PI;
        mVertexOffsetVector[index] = -z*mSceneDiameter + y*cos(angle)*r + x*sin(angle)*r;
    }

    mCenter = -z*mSceneDiameter;
}

cgmath::Vector3f
DistantAreaLight::calculateVertex(const cgmath::Vector3f &focalPoint, int index) const
{
    assert(mVertexOffsetVector.size() > 2);
    assert(index >= 0);
    assert(index < int(mVertexOffsetVector.size()));

    return focalPoint + mVertexOffsetVector[index];
}

cgmath::Vector3f
DistantAreaLight::getCenter(const cgmath::Vector3f &focalPoint) const
{
    return focalPoint + mCenter;
}

} // namespace light
