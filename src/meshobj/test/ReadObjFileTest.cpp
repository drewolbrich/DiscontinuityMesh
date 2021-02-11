// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <meshobj/ReadObjFile.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/MeshOperations.h>
#include <cgmath/Vector3f.h>

class ReadObjFileTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ReadObjFileTest);
    CPPUNIT_TEST(testReadObjFile);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testReadObjFile() {
        mesh::Mesh mesh;

        CPPUNIT_ASSERT_NO_THROW(meshobj::ReadObjFile(&mesh, "box.obj"));

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        // The box has eight vertices.
        CPPUNIT_ASSERT(mesh.vertexCount() == 8);

        // The box has eighteen edges.
        CPPUNIT_ASSERT(mesh.edgeCount() == 18);

        // The box has twelve faces (six sides, each made from
        // two triangles).
        CPPUNIT_ASSERT(mesh.faceCount() == 12);

        cgmath::BoundingBox3f boundingBox = mesh::ComputeBoundingBox(mesh);
        CPPUNIT_ASSERT(boundingBox.min() == cgmath::Vector3f(-0.5, -0.5, -0.5));
        CPPUNIT_ASSERT(boundingBox.max() == cgmath::Vector3f(0.5, 0.5, 0.5));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ReadObjFileTest);
