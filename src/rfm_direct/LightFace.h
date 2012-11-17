// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_DISCMESH__LIGHT_FACE__INCLUDED
#define RFM_DISCMESH__LIGHT_FACE__INCLUDED

#include <mesh/Types.h>
#include <cgmath/Vector3f.h>
#include <meshretri/TriangleVector.h>

// LightFace
//
// Interface class for light sources used by MeshShader.

class LightFace
{
public:
    virtual ~LightFace() {}

    virtual void setFacePtr(mesh::FacePtr facePtr) = 0;
    virtual mesh::FacePtr facePtr() const = 0;

    // Compute the intensity contributed by the light source polygon
    // described by 'triangle' at the vertex with the specified position and normal.
    virtual cgmath::Vector3f computeIntensityAtPoint(const cgmath::Vector3f &position,
        const cgmath::Vector3f &normal, const meshretri::Triangle &triangle) const = 0;
};

#endif // RFM_DISCMESH__LIGHT_FACE__INCLUDED
