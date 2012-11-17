// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <meshobj/WriteObjFile.h>
#include <meshobj/ReadObjFile.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/MeshOperations.h>
#include <cgmath/BoundingBox3f.h>

using meshobj::WriteObjFile;

class WriteObjFileTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(WriteObjFileTest);
    CPPUNIT_TEST(testWriteObjFile);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testWriteObjFile() {
        mesh::Mesh mesh;

        // First, read the file and make sure it's what we expect.
        CPPUNIT_ASSERT_NO_THROW(meshobj::ReadObjFile(&mesh, "box.obj"));

        testMeshSize(mesh);
    
        // Write the file out to a temporary file,
        // and read it back in. Then make sure it's exactly we had before.
        CPPUNIT_ASSERT_NO_THROW(meshobj::WriteObjFile(mesh, "temp.obj"));
        CPPUNIT_ASSERT_NO_THROW(meshobj::ReadObjFile(&mesh, "temp.obj"));

        testMeshSize(mesh);
    }

    void testMeshSize(const mesh::Mesh &mesh) {
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

CPPUNIT_TEST_SUITE_REGISTRATION(WriteObjFileTest);
