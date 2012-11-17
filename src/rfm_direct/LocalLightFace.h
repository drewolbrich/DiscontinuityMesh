// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_DISCMESH__LOCAL_LIGHT_FACE__INCLUDED
#define RFM_DISCMESH__LOCAL_LIGHT_FACE__INCLUDED

#include <cgmath/Vector3f.h>

#include "LightFace.h"

// LocalLightFace
//
// Local light source face (an emissive polygon in the scene).

class LocalLightFace : public LightFace
{
public:
    LocalLightFace();
    virtual ~LocalLightFace();

    // For LightFace:
    virtual void setFacePtr(mesh::FacePtr facePtr);
    virtual mesh::FacePtr facePtr() const;
    virtual cgmath::Vector3f computeIntensityAtPoint(const cgmath::Vector3f &position,
        const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const;

    // The intensity of the light source face.
    void setIntensity(const cgmath::Vector3f &intensity);
    const cgmath::Vector3f &intensity() const;

private:
    double computePointToPolygonFormFactor(const cgmath::Vector3f &point,
        const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const;

    mesh::FacePtr mFacePtr;
    cgmath::Vector3f mIntensity;
};

#endif // RFM_DISCMESH__LOCAL_LIGHT_FACE__INCLUDED
