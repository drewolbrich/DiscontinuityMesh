// Copyright 2008 Drew Olbrich

#ifndef MESH__ATTRIBUTE_POSSESSSOR__INCLUDED
#define MESH__ATTRIBUTE_POSSESSSOR__INCLUDED

#include <vector>

#include <stdint.h>

#include "AttributeData.h"

namespace cgmath {
class Vector2f;
class Vector3f;
class Vector4f;
class Matrix3f;
class Matrix4f;
class BoundingBox2f;
class BoundingBox3f;
}

namespace mesh {

class AttributeKey;

// AttributePossesssor
//
// This is a base class for classes that can one or more mesh
// attributes. This class is not intended to be used directly,
// only through a subclass.

class AttributePossessor {
public:
    AttributePossessor();
    virtual ~AttributePossessor();

    // Efficiently swap the contents of the AttributePossessor with another.
    void swap(AttributePossessor &rhs);
    
    // Returns true if the specified attribute is defined for 
    // this AttributePossessor.
    bool hasAttribute(const AttributeKey &key) const;

    // Remove the specified attribute, if it exists.
    void eraseAttribute(const AttributeKey &key);

    // Returns true if the AttributePossessor has no
    // attributes defined.
    bool hasNoAttributes() const;

    // Copy all of the attributes from the specified
    // AttributePossessor. All of the existing attributes
    // for the object are lost.
    void copyAttributes(const AttributePossessor &source);
    
    // Returns the value of the specified attribute. If the attribute
    // is not yet defined for the AttributePossessor object,
    // a default value is returned (false, zero, the identity matrix,
    // or the empty string.)
    // The return values are not passed by reference, because there's a small
    // possibility they would be invalidated the next time the AttriutePossessor's
    // AttributeData vector is resized, for example when one of the "set" functions
    // below is called.
    bool getBool(const AttributeKey &key) const;
    int32_t getInt(const AttributeKey &key) const;
    float getFloat(const AttributeKey &key) const;
    cgmath::Vector2f getVector2f(const AttributeKey &key) const;
    cgmath::Vector3f getVector3f(const AttributeKey &key) const;
    cgmath::Vector4f getVector4f(const AttributeKey &key) const;
    cgmath::Matrix3f getMatrix3f(const AttributeKey &key) const;
    cgmath::Matrix4f getMatrix4f(const AttributeKey &key) const;
    std::string getString(const AttributeKey &key) const;
    cgmath::Vector3f getUnitVector3f(const AttributeKey &key) const;
    cgmath::BoundingBox2f getBoundingBox2f(const AttributeKey &key) const;
    cgmath::BoundingBox3f getBoundingBox3f(const AttributeKey &key) const;

    // Set the specified attribute for the AttributePossessor object.
    void setBool(const AttributeKey &key, bool value);
    void setInt(const AttributeKey &key, int32_t value);
    void setFloat(const AttributeKey &key, float value);
    void setVector2f(const AttributeKey &key, const cgmath::Vector2f &value);
    void setVector3f(const AttributeKey &key, const cgmath::Vector3f &value);
    void setVector4f(const AttributeKey &key, const cgmath::Vector4f &value);
    void setMatrix3f(const AttributeKey &key, const cgmath::Matrix3f &value);
    void setMatrix4f(const AttributeKey &key, const cgmath::Matrix4f &value);
    void setString(const AttributeKey &key, const std::string &value);
    void setUnitVector3f(const AttributeKey &key, const cgmath::Vector3f &value);
    void setBoundingBox2f(const AttributeKey &key, const cgmath::BoundingBox2f &value);
    void setBoundingBox3f(const AttributeKey &key, const cgmath::BoundingBox3f &value);

    // Functions for iterating directly over the attributes,
    // without a key.
    typedef std::vector<AttributeData> AttributeDataVector;
    typedef AttributeDataVector::iterator iterator;
    typedef AttributeDataVector::const_iterator const_iterator;
    int attributeDataCount() const;
    AttributeDataVector::iterator attributeDataBegin();
    AttributeDataVector::iterator attributeDataEnd();
    AttributeDataVector::const_iterator attributeDataBegin() const;
    AttributeDataVector::const_iterator attributeDataEnd() const;

    // Returns a pointer to the AttributeData corresponding to the specified AttributeKey,
    // or NULL if none exists.
    const AttributeData *findAttributeData(const AttributeKey &key) const;

    // Just like findAttributeData, except if no corresponding
    // AttributeData object is found, a new one is created,
    // and memory for it is allocated.
    AttributeData *findOrCreateAttributeData(const AttributeKey &key);

private:
    // The memory that the AttributeData objects in this STL vector
    // point to is owned by the AttributePossessor, not by the
    // AttributeData objects themselves.
    AttributeDataVector mAttributeDataVector;
};

} // namespace mesh

#endif // MESH__ATTRIBUTE_POSSESSSOR__INCLUDED
