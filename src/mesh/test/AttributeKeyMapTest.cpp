// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/AttributeKeyMap.h>

using mesh::AttributeKeyMap;
using mesh::AttributeKey;

class AttributeKeyMapTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AttributeKeyMapTest);
    CPPUNIT_TEST(testAttributeKeyMap);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testAttributeKeyMap() {
        AttributeKeyMap attributeKeyMap;

        AttributeKey attributeKey 
            = attributeKeyMap.getAttributeKey("name1", AttributeKey::VECTOR3F);
        CPPUNIT_ASSERT(attributeKey.handle() == 1);
        CPPUNIT_ASSERT(attributeKey.type() == AttributeKey::VECTOR3F);

        CPPUNIT_ASSERT(attributeKeyMap.hasAttributeKey("name1"));

        attributeKeyMap.eraseAttributeKey("name1");

        CPPUNIT_ASSERT(!attributeKeyMap.hasAttributeKey("name1"));

        attributeKeyMap.getAttributeKey("name1", AttributeKey::VECTOR3F);
        attributeKeyMap.getAttributeKey("name2", AttributeKey::VECTOR3F);
        attributeKeyMap.getAttributeKey("name3", AttributeKey::VECTOR3F);

        AttributeKeyMap::iterator iterator = attributeKeyMap.begin();
        CPPUNIT_ASSERT((*iterator).first == "name1");
        CPPUNIT_ASSERT((*iterator).second.handle() == 2);
        CPPUNIT_ASSERT((*iterator).second.type() == AttributeKey::VECTOR3F);
        ++iterator;
        CPPUNIT_ASSERT((*iterator).first == "name2");
        CPPUNIT_ASSERT((*iterator).second.handle() == 3);
        CPPUNIT_ASSERT((*iterator).second.type() == AttributeKey::VECTOR3F);
        ++iterator;
        CPPUNIT_ASSERT((*iterator).first == "name3");
        CPPUNIT_ASSERT((*iterator).second.handle() == 4);
        CPPUNIT_ASSERT((*iterator).second.type() == AttributeKey::VECTOR3F);
        ++iterator;
        CPPUNIT_ASSERT(iterator == attributeKeyMap.end());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttributeKeyMapTest);
