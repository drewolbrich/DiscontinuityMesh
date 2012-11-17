// Copyright 2009 Retroactive Fiasco.

#include "MaterialTable.h"

#include <cassert>
#include <vector>

#include <mesh/Mesh.h>
#include <mesh/StandardAttributes.h>

namespace mesh {

MaterialTable::MaterialTable()
    : mMaterialIndexAttributeKey(),
      mColor3fAttributeKey(),
      mMaterialVector()
{
}

MaterialTable::~MaterialTable()
{
}

void
MaterialTable::initialize(const Mesh &mesh)
{
    mMaterialIndexAttributeKey = GetMaterialIndexAttributeKey(mesh);
    mColor3fAttributeKey = GetColor3fAttributeKey(mesh);

    std::vector<int> materialIndexVector = GetMaterialIndexVector(mesh);
    if (materialIndexVector.empty()) {
        return;
    }

    // It's assumed the vector is already sorted.
    size_t maxIndex = materialIndexVector.back();

    mMaterialVector.resize(maxIndex + 1);

    for (size_t index = 0; index < mMaterialVector.size(); ++index) {
        mMaterialVector[index].mName = mesh.getString(
            GetMaterialNameAttributeKey(mesh, index));
        mMaterialVector[index].mAmbient = mesh.getVector4f(
            GetMaterialAmbient4fAttributeKey(mesh, index));
        mMaterialVector[index].mDiffuse = mesh.getVector4f(
            GetMaterialDiffuse4fAttributeKey(mesh, index));
        mMaterialVector[index].mSpecular = mesh.getVector4f(
            GetMaterialSpecular4fAttributeKey(mesh, index));
        mMaterialVector[index].mEmission = mesh.getVector4f(
            GetMaterialEmission4fAttributeKey(mesh, index));
        mMaterialVector[index].mShininess = mesh.getFloat(
            GetMaterialShininessAttributeKey(mesh, index));
    }
}

bool
MaterialTable::faceHasValidMaterialIndex(FacePtr facePtr) const
{
    if (!facePtr->hasAttribute(mMaterialIndexAttributeKey)) {
        return false;
    }

    size_t materialIndex = facePtr->getInt(mMaterialIndexAttributeKey);

    return materialIndex < mMaterialVector.size();
}

const Material &
MaterialTable::getMaterialFromFace(FacePtr facePtr) const
{
    assert(faceHasValidMaterialIndex(facePtr));

    size_t materialIndex = facePtr->getInt(mMaterialIndexAttributeKey);

    return mMaterialVector[materialIndex];
}

cgmath::Vector3f
MaterialTable::getFaceVertexDiffuseColor(FacePtr facePtr,
    VertexPtr vertexPtr)
{
    cgmath::Vector3f diffuse(1.0, 1.0, 1.0);

    if (facePtr->hasAttribute(mMaterialIndexAttributeKey)) {
        size_t materialIndex = facePtr->getInt(mMaterialIndexAttributeKey);
        assert(materialIndex < mMaterialVector.size());
        diffuse *= cgmath::Vector3f(mMaterialVector[materialIndex].mDiffuse);
    }

    if (facePtr->hasAttribute(mColor3fAttributeKey)) {
        diffuse *= facePtr->getVector3f(mColor3fAttributeKey);
    }

    if (facePtr->hasVertexAttribute(vertexPtr, mColor3fAttributeKey)) {
        diffuse *= facePtr->getVertexVector3f(vertexPtr, mColor3fAttributeKey);
    }

    return diffuse;
}

} // namespace mesh
