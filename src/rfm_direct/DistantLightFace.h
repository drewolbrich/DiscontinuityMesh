// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_DISCMESH__DISTANT_LIGHT_FACE__INCLUDED
#define RFM_DISCMESH__DISTANT_LIGHT_FACE__INCLUDED

#include "LightFace.h"

// DistantLightFace
//
// Distant light source face.

class DistantLightFace : public LightFace
{
public:
    DistantLightFace();
    virtual ~DistantLightFace();

    // For LightFace:
    virtual void setFacePtr(mesh::FacePtr facePtr);
    virtual mesh::FacePtr facePtr() const;
    virtual cgmath::Vector3f computeIntensityAtPoint(const cgmath::Vector3f &position,
        const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const;

    // The intensity of the light source face.
    void setIntensity(const cgmath::Vector3f &intensity);
    const cgmath::Vector3f &intensity() const;

    // The area of the face when it's not occluded.
    // The area of 'triangle' as passed to computeIntensityAtPoint
    // will be some fraction of this value.
    void setUnoccludedFaceArea(float unoccludedFaceArea);
    float unoccludedFaceArea() const;

    // The center of the light source.
    void setCenter(const cgmath::Vector3f &center);
    const cgmath::Vector3f &center() const;

private:
    mesh::FacePtr mFacePtr;
    cgmath::Vector3f mIntensity;
    float mUnoccludedFaceArea;
    cgmath::Vector3f mCenter;
};

#endif // RFM_DISCMESH__DISTANT_LIGHT_FACE__INCLUDED
