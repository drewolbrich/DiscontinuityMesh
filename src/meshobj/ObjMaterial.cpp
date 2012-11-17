// Copyright 2008 Retroactive Fiasco.

#include "ObjMaterial.h"

using cgmath::Vector3f;

namespace meshobj {

ObjMaterial::ObjMaterial()
    : mAmbientIsSet(false),
      mDiffuseIsSet(false),
      mSpecularIsSet(false),
      mEmissionIsSet(false),
      mOpacityIsSet(false),
      mShininessIsSet(false),
      mDiffuseMapIsSet(false),
      mAmbient(Vector3f::ZERO),
      mDiffuse(Vector3f::UNIT),
      mSpecular(Vector3f::ZERO),
      mEmission(Vector3f::ZERO),
      mOpacity(1.0),
      mShininess(0.0),
      mDiffuseMap("")
{
}

ObjMaterial::~ObjMaterial()
{
}

void 
ObjMaterial::setAmbient(const Vector3f &ambient)
{
    mAmbient = ambient;
    mAmbientIsSet = true;
}

bool 
ObjMaterial::ambientIsSet() const
{
    return mAmbientIsSet;
}

const Vector3f &
ObjMaterial::ambient() const
{
    return mAmbient;
}

void
ObjMaterial::setDiffuse(const Vector3f &diffuse)
{
    mDiffuse = diffuse;
    mDiffuseIsSet = true;
}

bool 
ObjMaterial::diffuseIsSet() const
{
    return mDiffuseIsSet;
}

const Vector3f &
ObjMaterial::diffuse() const
{
    return mDiffuse;
}

void 
ObjMaterial::setSpecular(const Vector3f &specular)
{
    mSpecular = specular;
    mSpecularIsSet = true;
}

bool 
ObjMaterial::specularIsSet() const
{
    return mSpecularIsSet;
}

const Vector3f &
ObjMaterial::specular() const
{
    return mSpecular;
}

void 
ObjMaterial::setEmission(const Vector3f &emission)
{
    mEmission = emission;
    mEmissionIsSet = true;
}

bool 
ObjMaterial::emissionIsSet() const
{
    return mEmissionIsSet;
}

const Vector3f &
ObjMaterial::emission() const
{
    return mEmission;
}

void 
ObjMaterial::setOpacity(float opacity)
{
    mOpacity = opacity;
    mOpacityIsSet = true;
}

bool 
ObjMaterial::opacityIsSet() const
{
    return mOpacityIsSet;
}

float 
ObjMaterial::opacity() const
{
    return mOpacity;
}

void 
ObjMaterial::setShininess(float shininess)
{
    mShininess = shininess;
    mShininessIsSet = true;
}

bool 
ObjMaterial::shininessIsSet() const
{
    return mShininessIsSet;
}

float 
ObjMaterial::shininess() const
{
    return mShininess;
}

void 
ObjMaterial::setDiffuseMap(const std::string &diffuseMap)
{
    mDiffuseMap = diffuseMap;
    mDiffuseMapIsSet = true;
}

bool 
ObjMaterial::diffuseMapIsSet() const
{
    return mDiffuseMapIsSet;
}

const std::string &
ObjMaterial::diffuseMap() const
{
    return mDiffuseMap;
}

} // namespace meshobj
