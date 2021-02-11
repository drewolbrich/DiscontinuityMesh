// Copyright 2008 Drew Olbrich

#include "AttributeData.h"

#include <cstdlib>
#include <cassert>

#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/BoundingBox2f.h>
#include <cgmath/BoundingBox3f.h>

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using cgmath::BoundingBox2f;
using cgmath::BoundingBox3f;

namespace mesh {

AttributeData::AttributeData(AttributeKey attributeKey)
    : mHandle(attributeKey.handle()),
      mType(attributeKey.type()),
      mData(NULL)
{
    allocateData();
}

AttributeData::~AttributeData()
{
    if (mData != NULL) {
        deallocateData();
    }
    
#ifdef DEBUG
    mHandle = 0;
    mType = AttributeKey::UNDEFINED;
#endif
}

AttributeData::AttributeData(const AttributeData &rhs)
{
    mData = NULL;
    
    *this = rhs;
}

AttributeData &
AttributeData::operator=(const AttributeData &rhs)
{
    // If either AttributeData object has no data pointers, or the
    // types don't match up, then consider reallocating the memory.
    if (mData == NULL
        || rhs.mData == NULL
        || mType != rhs.mType) {

        if (mData != NULL) {
            deallocateData();
        }
        
        mHandle = rhs.mHandle;
        mType = rhs.mType;
        
        if (rhs.mData == NULL) {
            return *this;
        }

        allocateData();

    } else {

        // Make sure the handle and type are the same.
        mHandle = rhs.mHandle;
        mType = rhs.mType;
    }

    copyData(rhs);

    return *this;
}

void
AttributeData::copyData(const AttributeData &rhs)
{
    assert(mType == rhs.mType);

    switch (mType) {
    case AttributeKey::UNDEFINED:
        // This should never happen.
        abort();
        break;
    case AttributeKey::BOOL:
        *static_cast<bool *>(mData) = *static_cast<bool *>(rhs.mData);
        break;
    case AttributeKey::INT:
        *static_cast<int32_t *>(mData) = *static_cast<int32_t *>(rhs.mData);
        break;
    case AttributeKey::FLOAT:
        *static_cast<float *>(mData) = *static_cast<float *>(rhs.mData);
        break;
    case AttributeKey::VECTOR2F:
        *static_cast<Vector2f *>(mData) = *static_cast<Vector2f *>(rhs.mData);
        break;
    case AttributeKey::VECTOR3F:
        *static_cast<Vector3f *>(mData) = *static_cast<Vector3f *>(rhs.mData);
        break;
    case AttributeKey::VECTOR4F:
        *static_cast<Vector4f *>(mData) = *static_cast<Vector4f *>(rhs.mData);
        break;
    case AttributeKey::MATRIX3F:
        *static_cast<Matrix3f *>(mData) = *static_cast<Matrix3f *>(rhs.mData);
         break;
    case AttributeKey::MATRIX4F:
        *static_cast<Matrix4f *>(mData) = *static_cast<Matrix4f *>(rhs.mData);
        break;
    case AttributeKey::STRING:
        *static_cast<std::string *>(mData) 
            = *static_cast<std::string *>(rhs.mData);
        break;
    case AttributeKey::UNIT_VECTOR3F:
        *static_cast<Vector3f *>(mData) = *static_cast<Vector3f *>(rhs.mData);
        break;
    case AttributeKey::BOUNDINGBOX2F:
        *static_cast<BoundingBox2f *>(mData) = *static_cast<BoundingBox2f *>(rhs.mData);
        break;
    case AttributeKey::BOUNDINGBOX3F:
        *static_cast<BoundingBox3f *>(mData) = *static_cast<BoundingBox3f *>(rhs.mData);
        break;
    default:
        // This should never happen.
        abort();
        break;
    }
}

void 
AttributeData::allocateData()
{
    assert(mData == NULL);
    
    switch (mType) {
    case AttributeKey::UNDEFINED:
        // This should never happen.
        abort();
        break;
    case AttributeKey::BOOL:
        mData = static_cast<void *>(new bool);
        break;
    case AttributeKey::INT:
        mData = static_cast<void *>(new int32_t);
        break;
    case AttributeKey::FLOAT:
        mData = static_cast<void *>(new float);
        break;
    case AttributeKey::VECTOR2F:
        mData = static_cast<void *>(new Vector2f);
        break;
    case AttributeKey::VECTOR3F:
        mData = static_cast<void *>(new Vector3f);
        break;
    case AttributeKey::VECTOR4F:
        mData = static_cast<void *>(new Vector4f);
        break;
    case AttributeKey::MATRIX3F:
        mData = static_cast<void *>(new Matrix3f);
        break;
    case AttributeKey::MATRIX4F:
        mData = static_cast<void *>(new Matrix4f);
        break;
    case AttributeKey::STRING:
        mData = static_cast<void *>(new std::string);
        break;
    case AttributeKey::UNIT_VECTOR3F:
        mData = static_cast<void *>(new Vector3f);
        break;
    case AttributeKey::BOUNDINGBOX2F:
        mData = static_cast<void *>(new BoundingBox2f);
        break;
    case AttributeKey::BOUNDINGBOX3F:
        mData = static_cast<void *>(new BoundingBox3f);
        break;
    default:
        // This should never happen.
        abort();
        break;
    }
}

void 
AttributeData::deallocateData()
{
    assert(mData != NULL);

    switch (mType) {
    case AttributeKey::UNDEFINED:
        // This should never happen.
        abort();
        break;
    case AttributeKey::BOOL:
        delete static_cast<bool *>(mData);
        break;
    case AttributeKey::INT:
        delete static_cast<int32_t *>(mData);
        break;
    case AttributeKey::FLOAT:
        delete static_cast<float *>(mData);
        break;
    case AttributeKey::VECTOR2F:
        delete static_cast<Vector2f *>(mData);
        break;
    case AttributeKey::VECTOR3F:
        delete static_cast<Vector3f *>(mData);
        break;
    case AttributeKey::VECTOR4F:
        delete static_cast<Vector4f *>(mData);
        break;
    case AttributeKey::MATRIX3F:
        delete static_cast<Matrix3f *>(mData);
        break;
    case AttributeKey::MATRIX4F:
        delete static_cast<Matrix4f *>(mData);
        break;
    case AttributeKey::STRING:
        delete static_cast<std::string *>(mData);
        break;
    case AttributeKey::UNIT_VECTOR3F:
        delete static_cast<Vector3f *>(mData);
        break;
    case AttributeKey::BOUNDINGBOX2F:
        delete static_cast<BoundingBox2f *>(mData);
        break;
    case AttributeKey::BOUNDINGBOX3F:
        delete static_cast<BoundingBox3f *>(mData);
        break;
    default:
        // This should never happen.
        abort();
        break;
    }

    mData = NULL;
}

} // namespace mesh
