// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/AttributeKey.h>

using mesh::AttributeKey;

class AttributeKeyTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AttributeKeyTest);
    CPPUNIT_TEST(testAttributeKey);
    CPPUNIT_TEST(testAttributeKeyDefined);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testAttributeKey() {
        AttributeKey attributeKey(100, AttributeKey::VECTOR3F);

        CPPUNIT_ASSERT(attributeKey.handle() == 100);
        CPPUNIT_ASSERT(attributeKey.type() == AttributeKey::VECTOR3F);
    }

    void testAttributeKeyDefined() {
        AttributeKey undefinedAttributeKey;
        CPPUNIT_ASSERT(!undefinedAttributeKey.isDefined());

        AttributeKey definedAttributeKey(100, AttributeKey::VECTOR3F);
        CPPUNIT_ASSERT(definedAttributeKey.isDefined());

        AttributeKey copiedUndefinedAttributeKey = undefinedAttributeKey;
        CPPUNIT_ASSERT(!copiedUndefinedAttributeKey.isDefined());

        AttributeKey copiedDefinedAttributeKey = definedAttributeKey;
        CPPUNIT_ASSERT(copiedDefinedAttributeKey.isDefined());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttributeKeyTest);
