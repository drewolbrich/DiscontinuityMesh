// Copyright 2010 Drew Olbrich

#include "MeshRelighter.h"

#include <cstdlib>
#include <cassert>

#include <cgmath/Vector3f.h>
#include <cgmath/ColorOperations.h>
#include <mesh/Mesh.h>
#include <mesh/StandardAttributes.h>

MeshRelighter::MeshRelighter()
    : mMesh(NULL),
      mIntensity(1.0),
      mSaturation(1.0),
      mDirectIntensity(1.0),
      mDirectSaturation(1.0),
      mIndirectIntensity(1.0),
      mIndirectSaturation(1.0),
      mIlluminatedColor3fAttributeKey(),
      mColor3fAttributeKey()
{
}

MeshRelighter::~MeshRelighter()
{
}

void
MeshRelighter::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
MeshRelighter::mesh() const
{
    return mMesh;
}

void
MeshRelighter::setIntensity(float intensity)
{
    mIntensity = intensity;
}

float
MeshRelighter::intensity() const
{
    return mIntensity;
}

void
MeshRelighter::setSaturation(float saturation)
{
    mSaturation = saturation;
}

float
MeshRelighter::saturation() const
{
    return mSaturation;
}

void
MeshRelighter::setDirectIntensity(float directIntensity)
{
    mDirectIntensity = directIntensity;
}

float
MeshRelighter::directIntensity() const
{
    return mDirectIntensity;
}

void
MeshRelighter::setDirectSaturation(float directSaturation)
{
    mDirectSaturation = directSaturation;
}

float
MeshRelighter::directSaturation() const
{
    return mDirectSaturation;
}

void
MeshRelighter::setIndirectIntensity(float indirectIntensity)
{
    mIndirectIntensity = indirectIntensity;
}

float
MeshRelighter::indirectIntensity() const
{
    return mIndirectIntensity;
}

void
MeshRelighter::setIndirectSaturation(float indirectSaturation)
{
    mIndirectSaturation = indirectSaturation;
}

float
MeshRelighter::indirectSaturation() const
{
    return mIndirectSaturation;
}

void
MeshRelighter::relightMesh()
{
    assert(mMesh != NULL);

    mIlluminatedColor3fAttributeKey = mMesh->getAttributeKey("illuminatedColor3f",
        mesh::AttributeKey::VECTOR3F);
    mIndirectIllumination3fAttributeKey = mMesh->getAttributeKey(
        "indirectIllumination3f", mesh::AttributeKey::VECTOR3F);
    mColor3fAttributeKey = mesh::GetColor3fAttributeKey(*mMesh);

    for (mesh::FacePtr facePtr = mMesh->faceBegin(); 
         facePtr != mMesh->faceEnd(); ++facePtr) {
        relightFace(facePtr);
    }
}

void
MeshRelighter::relightFace(mesh::FacePtr facePtr)
{
    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;

        cgmath::Vector3f directIllumination 
            = facePtr->getVertexVector3f(vertexPtr, mIlluminatedColor3fAttributeKey);
        cgmath::Vector3f indirectIllumination
            = facePtr->getVertexVector3f(vertexPtr, mIndirectIllumination3fAttributeKey);

        directIllumination = scaleSaturation(directIllumination, mDirectSaturation)
            *mDirectIntensity;

        indirectIllumination = scaleSaturation(indirectIllumination, mIndirectSaturation)
            *mIndirectIntensity;

        cgmath::Vector3f result = directIllumination + indirectIllumination;

        result = scaleSaturation(result, mSaturation)*mIntensity;

        facePtr->setVertexVector3f(vertexPtr, mColor3fAttributeKey, result);
    }
}

cgmath::Vector3f
MeshRelighter::scaleSaturation(const cgmath::Vector3f &color, float scale)
{
    return cgmath::HsvToRgb(cgmath::RgbToHsv(color)*cgmath::Vector3f(1, scale, 1));
}
