// Copyright 2008 Retroactive Fiasco.

#include "AttributePossessor.h"

#include <cassert>

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using cgmath::BoundingBox2f;
using cgmath::BoundingBox3f;

namespace mesh {

AttributePossessor::AttributePossessor()
    : mAttributeDataVector()
{
}

AttributePossessor::~AttributePossessor()
{
}

void 
AttributePossessor::swap(AttributePossessor &rhs)
{
    mAttributeDataVector.swap(rhs.mAttributeDataVector);
}

bool 
AttributePossessor::hasAttribute(const AttributeKey &key) const
{
    return findAttributeData(key) != NULL;
}

void 
AttributePossessor::eraseAttribute(const AttributeKey &key)
{
    for (AttributeDataVector::iterator iterator
             = mAttributeDataVector.begin();
         iterator != mAttributeDataVector.end(); ++iterator) {
        if ((*iterator).handle() == key.handle()) {
            mAttributeDataVector.erase(iterator);
            break;
        }
    }
}

bool 
AttributePossessor::hasNoAttributes() const
{
    return mAttributeDataVector.empty();
}

void 
AttributePossessor::copyAttributes(const AttributePossessor &source)
{
    mAttributeDataVector = source.mAttributeDataVector;
}

bool 
AttributePossessor::getBool(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::BOOL);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_BOOL;
    }
    const void *data = attributeData->data();

    return *static_cast<const bool *>(data);
}

int32_t 
AttributePossessor::getInt(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::INT);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_INT;
    }
    const void *data = attributeData->data();

    return *static_cast<const int32_t *>(data);
}

float 
AttributePossessor::getFloat(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::FLOAT);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_FLOAT;
    }
    const void *data = attributeData->data();

    return *static_cast<const float *>(data);
}

Vector2f
AttributePossessor::getVector2f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::VECTOR2F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_VECTOR2F;
    }
    const void *data = attributeData->data();

    return *static_cast<const Vector2f *>(data);
}

Vector3f
AttributePossessor::getVector3f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::VECTOR3F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_VECTOR3F;
    }
    const void *data = attributeData->data();
    
    return *static_cast<const Vector3f *>(data);
}

Vector4f
AttributePossessor::getVector4f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::VECTOR4F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_VECTOR4F;
    }
    const void *data = attributeData->data();

    return *static_cast<const Vector4f *>(data);
}

Matrix3f
AttributePossessor::getMatrix3f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::MATRIX3F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_MATRIX3F;
    }
    const void *data = attributeData->data();

    return *static_cast<const Matrix3f *>(data);
}

Matrix4f
AttributePossessor::getMatrix4f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::MATRIX4F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_MATRIX4F;
    }
    const void *data = attributeData->data();

    return *static_cast<const Matrix4f *>(data);
}

std::string
AttributePossessor::getString(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::STRING);

    static const std::string emptyString("");
    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_STRING;
    }
    const void *data = attributeData->data();

    return *static_cast<const std::string *>(data);
}

Vector3f
AttributePossessor::getUnitVector3f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::UNIT_VECTOR3F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_UNIT_VECTOR3F;
    }
    const void *data = attributeData->data();

    return *static_cast<const Vector3f *>(data);
}

BoundingBox2f
AttributePossessor::getBoundingBox2f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::BOUNDINGBOX2F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_BOUNDINGBOX2F;
    }
    const void *data = attributeData->data();

    return *static_cast<const BoundingBox2f *>(data);
}

BoundingBox3f
AttributePossessor::getBoundingBox3f(const AttributeKey &key) const
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::BOUNDINGBOX3F);

    const AttributeData *attributeData = findAttributeData(key);
    if (attributeData == NULL) {
        return AttributeKey::DEFAULT_BOUNDINGBOX3F;
    }
    const void *data = attributeData->data();

    return *static_cast<const BoundingBox3f *>(data);
}

void 
AttributePossessor::setBool(const AttributeKey &key, bool value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::BOOL);
    
    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<bool *>(data) = value;
}

void 
AttributePossessor::setInt(const AttributeKey &key, int32_t value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::INT);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<int32_t *>(data) = value;
}

void 
AttributePossessor::setFloat(const AttributeKey &key, float value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::FLOAT);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<float *>(data) = value;
}

void 
AttributePossessor::setVector2f(const AttributeKey &key, 
    const Vector2f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::VECTOR2F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<Vector2f *>(data) = value;
}

void 
AttributePossessor::setVector3f(const AttributeKey &key, 
    const Vector3f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::VECTOR3F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<Vector3f *>(data) = value;
}

void 
AttributePossessor::setVector4f(const AttributeKey &key, 
    const Vector4f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::VECTOR4F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<Vector4f *>(data) = value;
}

void 
AttributePossessor::setMatrix3f(const AttributeKey &key, 
    const Matrix3f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::MATRIX3F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<Matrix3f *>(data) = value;
}

void 
AttributePossessor::setMatrix4f(const AttributeKey &key, 
    const Matrix4f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::MATRIX4F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<Matrix4f *>(data) = value;
}

void 
AttributePossessor::setString(const AttributeKey &key, 
    const std::string &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::STRING);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<std::string *>(data) = value;
}

void 
AttributePossessor::setUnitVector3f(const AttributeKey &key, 
    const Vector3f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::UNIT_VECTOR3F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<Vector3f *>(data) = value;
}

void
AttributePossessor::setBoundingBox2f(const AttributeKey &key, const BoundingBox2f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::BOUNDINGBOX2F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<BoundingBox2f *>(data) = value;
}

void
AttributePossessor::setBoundingBox3f(const AttributeKey &key, const cgmath::BoundingBox3f &value)
{
    // The specified AttributeKey must match this function's type.
    assert(key.type() == AttributeKey::BOUNDINGBOX3F);

    void *data = findOrCreateAttributeData(key)->data();

    *static_cast<BoundingBox3f *>(data) = value;
}

int 
AttributePossessor::attributeDataCount() const
{
    return mAttributeDataVector.size();
}

AttributePossessor::AttributeDataVector::iterator 
AttributePossessor::attributeDataBegin()
{
    return mAttributeDataVector.begin();
}

AttributePossessor::AttributeDataVector::iterator 
AttributePossessor::attributeDataEnd()
{
    return mAttributeDataVector.end();
}

AttributePossessor::AttributeDataVector::const_iterator 
AttributePossessor::attributeDataBegin() const
{
    return mAttributeDataVector.begin();
}

AttributePossessor::AttributeDataVector::const_iterator 
AttributePossessor::attributeDataEnd() const
{
    return mAttributeDataVector.end();
}

const AttributeData *
AttributePossessor::findAttributeData(const AttributeKey &key) const
{
    for (AttributeDataVector::const_iterator iterator
             = mAttributeDataVector.begin();
         iterator != mAttributeDataVector.end(); ++iterator) {

        if ((*iterator).handle() == key.handle()) {

            // The attribute data type must match that of the 
            // specified attribute key.
            assert((*iterator).type() == key.type());

            return &(*iterator);
        }
    }

    // No AttributeData object corresponding to this key was found.
    return NULL;
}

AttributeData *
AttributePossessor::findOrCreateAttributeData(const AttributeKey &key)
{
    for (AttributeDataVector::iterator iterator
             = mAttributeDataVector.begin();
         iterator != mAttributeDataVector.end(); ++iterator) {

        if ((*iterator).handle() == key.handle()) {

            // The attribute data type must match that of the 
            // specified attribute key.
            assert((*iterator).type() == key.type());

            return &(*iterator);
        }
    }

    // Append the AttributeData object to the vector.
    mAttributeDataVector.push_back(AttributeData(key));

    // Return a pointer to the allocated data.
    return &mAttributeDataVector.back();
}

} // namespace mesh
