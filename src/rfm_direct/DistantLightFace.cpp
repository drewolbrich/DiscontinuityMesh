// Copyright 2009 Drew Olbrich

#include "DistantLightFace.h"

#include <cgmath/TriangleOperations.h>

DistantLightFace::DistantLightFace()
    : mFacePtr(),
      mIntensity(),
      mUnoccludedFaceArea(0.0),
      mCenter()
{
}

DistantLightFace::~DistantLightFace()
{
}

void
DistantLightFace::setFacePtr(mesh::FacePtr facePtr)
{
    mFacePtr = facePtr;
}

mesh::FacePtr
DistantLightFace::facePtr() const
{
    return mFacePtr;
}

cgmath::Vector3f
DistantLightFace::computeIntensityAtPoint(const cgmath::Vector3f &position,
    const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const
{
    return mIntensity*cgmath::GetTriangleArea(
        triangle.mPointArray[0], triangle.mPointArray[1], triangle.mPointArray[2])
        /mUnoccludedFaceArea
        *normal.dot((mCenter - position).normalized());
}

void
DistantLightFace::setIntensity(const cgmath::Vector3f &intensity)
{
    mIntensity = intensity;
}

const cgmath::Vector3f &
DistantLightFace::intensity() const
{
    return mIntensity;
}

void
DistantLightFace::setUnoccludedFaceArea(float unoccludedFaceArea)
{
    mUnoccludedFaceArea = unoccludedFaceArea;
}

float
DistantLightFace::unoccludedFaceArea() const
{
    return mUnoccludedFaceArea;
}

void
DistantLightFace::setCenter(const cgmath::Vector3f &center)
{
    mCenter = center;
}

const cgmath::Vector3f &
DistantLightFace::center() const
{
    return mCenter;
}

