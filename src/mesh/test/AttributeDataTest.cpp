// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/AttributeData.h>
#include <mesh/AttributeKey.h>

using mesh::AttributeKey;
using mesh::AttributeData;

class AttributeDataTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AttributeDataTest);
    CPPUNIT_TEST(testAttributeData);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testAttributeData() {
        AttributeKey attributeKey(100, AttributeKey::VECTOR3F);

        AttributeData attributeData(attributeKey);

        CPPUNIT_ASSERT(attributeData.handle() == 100);
        CPPUNIT_ASSERT(attributeData.type() == AttributeKey::VECTOR3F);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttributeDataTest);
