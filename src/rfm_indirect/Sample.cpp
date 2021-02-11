// Copyright 2010 Drew Olbrich

#include "Sample.h"

Sample::Sample()
    : mPosition(),
      mNormal(),
      mIllumination(),
      mFaceArea(0.0)
{
}

Sample::~Sample()
{
}

void
Sample::setPosition(const cgmath::Vector3f &position)
{
    mPosition = position;
}

const cgmath::Vector3f &
Sample::position() const
{
    return mPosition;
}

void
Sample::setNormal(const cgmath::Vector3f &normal)
{
    mNormal = normal;
}

const cgmath::Vector3f &
Sample::normal() const
{
    return mNormal;
}

void
Sample::setIllumination(const cgmath::Vector3f &illumination)
{
    mIllumination = illumination;
}

const cgmath::Vector3f &
Sample::illumination() const
{
    return mIllumination;
}

void
Sample::setFaceArea(float faceArea)
{
    mFaceArea = faceArea;
}

float
Sample::faceArea() const
{
    return mFaceArea;
}

cgmath::BoundingBox3f
Sample::boundingBox() const
{
    return cgmath::BoundingBox3f(mPosition, mPosition);
}

