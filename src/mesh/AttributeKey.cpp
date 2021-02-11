// Copyright 2008 Drew Olbrich

#include "AttributeKey.h"

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using cgmath::BoundingBox2f;
using cgmath::BoundingBox3f;

namespace mesh {

AttributeKey::AttributeKey()
    : mHandle(0),
      mType(UNDEFINED),
      mFlags(0)
{
}

AttributeKey::~AttributeKey()
{
#ifdef DEBUG
    mHandle = 0;
    mType = UNDEFINED;
    mFlags = 0;
#endif
}
    
AttributeKey::AttributeKey(Handle handle, Type type, unsigned flags)
    : mHandle(handle),
      mType(type),
      mFlags(flags)
{
}

bool
AttributeKey::isStandard() const
{
    return mFlags & STANDARD;
}

bool
AttributeKey::isTemporary() const
{
    return mFlags & TEMPORARY;
}

bool
AttributeKey::isDefined() const
{
    return mType != UNDEFINED;
}

const bool AttributeKey::DEFAULT_BOOL(false);
const int AttributeKey::DEFAULT_INT(0);
const float AttributeKey::DEFAULT_FLOAT(0.0);
const Vector2f &AttributeKey::DEFAULT_VECTOR2F(Vector2f::ZERO);
const Vector3f &AttributeKey::DEFAULT_VECTOR3F(Vector3f::ZERO);
const Vector4f &AttributeKey::DEFAULT_VECTOR4F(Vector4f::ZERO);
const Matrix3f &AttributeKey::DEFAULT_MATRIX3F(Matrix3f::IDENTITY);
const Matrix4f &AttributeKey::DEFAULT_MATRIX4F(Matrix4f::IDENTITY);
const std::string AttributeKey::DEFAULT_STRING;
const Vector3f &AttributeKey::DEFAULT_UNIT_VECTOR3F(Vector3f::ZERO);
const BoundingBox2f &AttributeKey::DEFAULT_BOUNDINGBOX2F(BoundingBox2f::EMPTY_SET);
const BoundingBox3f &AttributeKey::DEFAULT_BOUNDINGBOX3F(BoundingBox3f::EMPTY_SET);

} // namespace mesh
