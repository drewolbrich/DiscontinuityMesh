// Copyright 2009 Retroactive Fiasco.

#include "MeshShaderFaceListener.h"

MeshShaderFaceListener::MeshShaderFaceListener()
    : mFacePtrToIgnore()
{
}

MeshShaderFaceListener::~MeshShaderFaceListener()
{
}

void
MeshShaderFaceListener::setFacePtrToIgnore(mesh::FacePtr facePtrToIgnore)
{
    mFacePtrToIgnore = facePtrToIgnore;
}

mesh::FacePtr
MeshShaderFaceListener::facePtrToIgnore() const
{
    return mFacePtrToIgnore;
}

bool
MeshShaderFaceListener::allowFaceIntersectionTest(mesh::ConstFacePtr facePtr, float t)
{
    if (facePtr == mFacePtrToIgnore) {
        return false;
    }

    // Don't intersect anything close to the ray origin.
    // TODO: See src/rfm_discmesh, MeshShaderFaceListener:initialize
    // for a better way of computing this epsilon.
    if (t < 0.001) {
        return false;
    }

    return true;
}

