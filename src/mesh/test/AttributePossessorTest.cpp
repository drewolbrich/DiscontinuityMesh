// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/AttributePossessor.h>
#include <mesh/AttributeKey.h>
#include <mesh/AttributeKeyMap.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/Matrix4f.h>
#include <cgmath/BoundingBox2f.h>
#include <cgmath/BoundingBox3f.h>

using mesh::AttributePossessor;
using mesh::AttributeKey;
using mesh::AttributeKeyMap;
using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;
using cgmath::BoundingBox2f;
using cgmath::BoundingBox3f;

class AttributePossessorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AttributePossessorTest);
    CPPUNIT_TEST(testAttributePossessor);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testAttributePossessor() {
        AttributeKeyMap attributeKeyMap;

        AttributeKey boolKey 
            = attributeKeyMap.getAttributeKey("bool", AttributeKey::BOOL);
        AttributeKey intKey 
            = attributeKeyMap.getAttributeKey("int", AttributeKey::INT);
        AttributeKey floatKey 
            = attributeKeyMap.getAttributeKey("float", AttributeKey::FLOAT);
        AttributeKey vector3fKey 
            = attributeKeyMap.getAttributeKey("vector3f", AttributeKey::VECTOR3F);
        AttributeKey vector4fKey 
            = attributeKeyMap.getAttributeKey("vector4f", AttributeKey::VECTOR4F);
        AttributeKey matrix3fKey 
            = attributeKeyMap.getAttributeKey("matrix3f", AttributeKey::MATRIX3F);
        AttributeKey matrix4fKey 
            = attributeKeyMap.getAttributeKey("matrix4f", AttributeKey::MATRIX4F);
        AttributeKey boundingBox2fKey 
            = attributeKeyMap.getAttributeKey("boundingBox2f", AttributeKey::BOUNDINGBOX2F);
        AttributeKey boundingBox3fKey 
            = attributeKeyMap.getAttributeKey("boundingBox3f", AttributeKey::BOUNDINGBOX3F);
    
        AttributePossessor attributePossessor;

        CPPUNIT_ASSERT(attributePossessor.hasNoAttributes());

        CPPUNIT_ASSERT(!attributePossessor.hasAttribute(boolKey));
    
        // Test default values.
        CPPUNIT_ASSERT(!attributePossessor.getBool(boolKey));
        CPPUNIT_ASSERT(attributePossessor.getInt(intKey) == 0);
        CPPUNIT_ASSERT(attributePossessor.getFloat(floatKey) == 0);
        CPPUNIT_ASSERT(attributePossessor.getVector3f(vector3fKey) == Vector3f::ZERO);
        CPPUNIT_ASSERT(attributePossessor.getVector4f(vector4fKey) == Vector4f::ZERO);
        CPPUNIT_ASSERT(attributePossessor.getMatrix3f(matrix3fKey) == Matrix3f::IDENTITY);
        CPPUNIT_ASSERT(attributePossessor.getMatrix4f(matrix4fKey) == Matrix4f::IDENTITY);
        CPPUNIT_ASSERT(attributePossessor.getBoundingBox2f(boundingBox2fKey) 
            == BoundingBox2f::EMPTY_SET);
        CPPUNIT_ASSERT(attributePossessor.getBoundingBox3f(boundingBox3fKey) 
            == BoundingBox3f::EMPTY_SET);

        // Test value assignment.
        attributePossessor.setBool(boolKey, true);
        CPPUNIT_ASSERT(attributePossessor.getBool(boolKey));

        attributePossessor.setInt(intKey, 100);
        CPPUNIT_ASSERT(attributePossessor.getInt(intKey) == 100);

        attributePossessor.setFloat(floatKey, 100);
        CPPUNIT_ASSERT(attributePossessor.getFloat(floatKey) == 100);

        attributePossessor.setVector3f(vector3fKey, 
            Vector3f(100, 200, 300));
        CPPUNIT_ASSERT(attributePossessor.getVector3f(vector3fKey)
            == Vector3f(100, 200, 300));

        attributePossessor.setVector4f(vector4fKey, 
            Vector4f(100, 200, 300, 400));
        CPPUNIT_ASSERT(attributePossessor.getVector4f(vector4fKey)
            == Vector4f(100, 200, 300, 400));

        attributePossessor.setMatrix3f(matrix3fKey,
            Matrix3f::fromScale(Vector3f(100, 200, 300)));
        CPPUNIT_ASSERT(attributePossessor.getMatrix3f(matrix3fKey)
            == Matrix3f::fromScale(Vector3f(100, 200, 300)));

        attributePossessor.setMatrix4f(matrix4fKey,
            Matrix4f::fromScale(Vector3f(100, 200, 300)));
        CPPUNIT_ASSERT(attributePossessor.getMatrix4f(matrix4fKey)
            == Matrix4f::fromScale(Vector3f(100, 200, 300)));
        
        attributePossessor.setBoundingBox2f(boundingBox2fKey,
            BoundingBox2f(0, 1, 2, 3));
        CPPUNIT_ASSERT(attributePossessor.getBoundingBox2f(boundingBox2fKey)
                == BoundingBox2f(0, 1, 2, 3));

        attributePossessor.setBoundingBox3f(boundingBox3fKey,
            BoundingBox3f(0, 1, 2, 3, 4, 5));
        CPPUNIT_ASSERT(attributePossessor.getBoundingBox3f(boundingBox3fKey)
            == BoundingBox3f(0, 1, 2, 3, 4, 5));

        CPPUNIT_ASSERT(attributePossessor.hasAttribute(boolKey));

        attributePossessor.eraseAttribute(boolKey);

        CPPUNIT_ASSERT(!attributePossessor.hasAttribute(boolKey));

        CPPUNIT_ASSERT(!attributePossessor.hasNoAttributes());

        AttributePossessor source;
        AttributePossessor target;
        source.setBool(boolKey, true);
        target.setInt(intKey, 100);
        target.copyAttributes(source);
        CPPUNIT_ASSERT(source.hasAttribute(boolKey));
        CPPUNIT_ASSERT(target.hasAttribute(boolKey));
        CPPUNIT_ASSERT(!target.hasAttribute(intKey));
        CPPUNIT_ASSERT(target.getBool(boolKey));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttributePossessorTest);
