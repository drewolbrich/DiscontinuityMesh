// Copyright 2009 Drew Olbrich

#include "LocalLightFace.h"

#include <cmath>
#include <algorithm>

#include <cgmath/Vector3d.h>
#include <cgmath/Constants.h>

LocalLightFace::LocalLightFace()
    : mFacePtr(),
      mIntensity(1.0, 1.0, 1.0)
{
}

LocalLightFace::~LocalLightFace()
{
}

void
LocalLightFace::setFacePtr(mesh::FacePtr facePtr)
{
    mFacePtr = facePtr;
}

mesh::FacePtr
LocalLightFace::facePtr() const
{
    return mFacePtr;
}

cgmath::Vector3f
LocalLightFace::computeIntensityAtPoint(const cgmath::Vector3f &position,
    const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const
{
    double formFactor = computePointToPolygonFormFactor(position, normal, triangle);

    return cgmath::Vector3f(formFactor*cgmath::Vector3d(mIntensity));
}

void
LocalLightFace::setIntensity(const cgmath::Vector3f &intensity)
{
    mIntensity = intensity;
}

const cgmath::Vector3f &
LocalLightFace::intensity() const
{
    return mIntensity;
}

double
LocalLightFace::computePointToPolygonFormFactor(const cgmath::Vector3f &point,
    const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const
{
    // The following is based on the book "Radiosity and Global Illumination",
    // chapter 3, p. 43, equation 3.53.

    // We slather double precision all over this function because
    // often it will return very small numbers, which, when
    // multiplied by large emissive face intensities, will
    // result in a lot of error.

    double sum = 0.0;
    for (size_t index = 0; index < 3; ++index) {
        cgmath::Vector3d r1 = cgmath::Vector3d(triangle.mPointArray[index]) 
            - cgmath::Vector3d(point);
        cgmath::Vector3d r0 = cgmath::Vector3d(triangle.mPointArray[(index + 1) % 3])
            - cgmath::Vector3d(point);
        double cosineOfGamma = r0.normalized().dot(r1.normalized());
        cosineOfGamma = std::min(1.0, std::max(-1.0, cosineOfGamma));
        double gamma = acos(cosineOfGamma);
        sum += cgmath::Vector3d(normal).dot((r0.cross(r1)).normalized()*gamma);
    }

    return std::max(0.0, sum/(2.0*cgmath::PI));
}
