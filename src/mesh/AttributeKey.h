// Copyright 2008 Retroactive Fiasco.

#ifndef MESH__ATTRIBUTE_KEY__INCLUDED
#define MESH__ATTRIBUTE_KEY__INCLUDED

#include <string>

#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/BoundingBox2f.h>
#include <cgmath/BoundingBox3f.h>

namespace mesh {

// AttributeKey
//
// This class encapsulates a key that is used to access attributes
// associated with an object that inherits from AttributePossessor.

class AttributeKey 
{
public:
    // These values are written to the .rfm files, so should
    // never change.
    enum Type {
        UNDEFINED       = 0,
        BOOL            = 1,
        INT             = 2,
        FLOAT           = 3,
        VECTOR2F        = 4,
        VECTOR3F        = 5,
        VECTOR4F        = 6,
        MATRIX3F        = 7,
        MATRIX4F        = 8,
        STRING          = 9,
        UNIT_VECTOR3F   = 10,  // Always normalized, even when interpolated.
        BOUNDINGBOX2F   = 11,
        BOUNDINGBOX3F   = 12
    };

    typedef unsigned short Handle;

    enum Flags {
        STANDARD    = (1 << 0), // Standard mesh library attributes.
        TEMPORARY   = (1 << 1)  // Attributes not written to disk.
    };
    
    AttributeKey();
    ~AttributeKey();

    AttributeKey(Handle handle, Type type, unsigned flags = 0);

    bool operator==(const AttributeKey &rhs) const {
        return mHandle == rhs.mHandle;
    }
    
    Handle handle() const { return mHandle; };
    Type type() const { return mType; };
    unsigned flags() const { return mFlags; };

    // If true, the attribute key references a standard attribute that
    // is defined by the mesh library, as opposed to by some other
    // library or application.
    bool isStandard() const;

    // If true, the attribute key and attributes of this type
    // should never be written to disk.
    bool isTemporary() const;

    // Returns true if the values of the AttributeKey have been defined.
    // Returns false if the AttributeKey was created using the default constructor.
    bool isDefined() const;
    
    // These values are returned by member functions of
    // AttributePossessor and Face (for face vertices) when attributes
    // that aren't defined yet are queried.
    // The vector and matrix values below are implemented as references,
    // because they are initialized to static values defined in the vector 
    // and matrix classes, and if we copied those values, there's a 50%
    // chance that they wouldn't have been initialized yet, depending
    // on the whim of the compiler.
    static const bool DEFAULT_BOOL;
    static const int DEFAULT_INT;
    static const float DEFAULT_FLOAT;
    static const cgmath::Vector2f &DEFAULT_VECTOR2F;
    static const cgmath::Vector3f &DEFAULT_VECTOR3F;
    static const cgmath::Vector4f &DEFAULT_VECTOR4F;
    static const cgmath::Matrix3f &DEFAULT_MATRIX3F;
    static const cgmath::Matrix4f &DEFAULT_MATRIX4F;
    static const std::string DEFAULT_STRING;
    static const cgmath::Vector3f &DEFAULT_UNIT_VECTOR3F;
    static const cgmath::BoundingBox2f &DEFAULT_BOUNDINGBOX2F;
    static const cgmath::BoundingBox3f &DEFAULT_BOUNDINGBOX3F;
    
private:
    Handle mHandle;
    Type mType;
    unsigned mFlags;
};

} // namespace mesh

#endif // MESH__ATTRIBUTE_KEY__INCLUDED
