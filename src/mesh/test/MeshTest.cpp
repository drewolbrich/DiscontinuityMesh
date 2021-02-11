// Copyright 2010 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/Mesh.h>

using mesh::Mesh;

class MeshTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MeshTest);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testAssignment);
    CPPUNIT_TEST(testClear);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCopyConstructor() {
        Mesh mesh1;
        mesh1.setFilename("foo");
        mesh1.createVertex();
        mesh1.createVertex();
        mesh1.createVertex();
        mesh1.createFace();
        Mesh mesh2(mesh1);

        CPPUNIT_ASSERT(mesh2.filename() == "foo");
        CPPUNIT_ASSERT(mesh2.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh2.faceCount() == 1);
    }

    void testAssignment() {
        Mesh mesh1;
        mesh1.setFilename("foo");
        mesh1.createVertex();
        mesh1.createVertex();
        mesh1.createVertex();
        mesh1.createFace();
        Mesh mesh2;
        mesh2 = mesh1;

        CPPUNIT_ASSERT(mesh2.filename() == "foo");
        CPPUNIT_ASSERT(mesh2.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh2.faceCount() == 1);
    }

    void testClear() {
        Mesh mesh1;
        mesh1.setFilename("foo");
        mesh1.createVertex();
        mesh1.createVertex();
        mesh1.createVertex();
        mesh1.createFace();
        mesh1.clear();

        CPPUNIT_ASSERT(mesh1.filename() == "");
        CPPUNIT_ASSERT(mesh1.vertexCount() == 0);
        CPPUNIT_ASSERT(mesh1.faceCount() == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MeshTest);
