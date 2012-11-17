// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/AttributePossessorOperations.h>
#include <mesh/Mesh.h>
#include <mesh/Vertex.h>
#include <mesh/Types.h>

class AttributePossessorOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(AttributePossessorOperationsTest);
    CPPUNIT_TEST(testMergeAttributePossessorAttributes);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testMergeAttributePossessorAttributes() {
        mesh::Mesh mesh;

        mesh::VertexPtr sourceVertexPtr = mesh.createVertex();
        mesh::VertexPtr targetVertexPtr = mesh.createVertex();

        mesh::AttributeKey key1 = mesh.getAttributeKey("testKey1", 
            mesh::AttributeKey::FLOAT);
        sourceVertexPtr->setFloat(key1, 1.0);
        targetVertexPtr->setFloat(key1, 2.0);

        mesh::AttributeKey key2 = mesh.getAttributeKey("testKey2",
            mesh::AttributeKey::FLOAT);
        sourceVertexPtr->setFloat(key2, 3.0);

        CPPUNIT_ASSERT(sourceVertexPtr->getFloat(key1) == 1.0);
        CPPUNIT_ASSERT(sourceVertexPtr->getFloat(key2) == 3.0);

        CPPUNIT_ASSERT(targetVertexPtr->getFloat(key1) == 2.0);
        CPPUNIT_ASSERT(!targetVertexPtr->hasAttribute(key2));

        MergeAttributePossessorAttributes(mesh, *sourceVertexPtr, &*targetVertexPtr);

        CPPUNIT_ASSERT(sourceVertexPtr->getFloat(key1) == 1.0);
        CPPUNIT_ASSERT(sourceVertexPtr->getFloat(key2) == 3.0);

        // This already should not have changed, because it already existed.
        CPPUNIT_ASSERT(targetVertexPtr->getFloat(key1) == 2.0);
        // This attribute should have been copied, because it did not already exist.
        CPPUNIT_ASSERT(targetVertexPtr->getFloat(key2) == 3.0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttributePossessorOperationsTest);
