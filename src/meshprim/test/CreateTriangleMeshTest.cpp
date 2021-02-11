// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <meshprim/CreateTriangleMesh.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>

using meshprim::CreateTriangleMesh;

class CreateTriangleMeshTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CreateTriangleMeshTest);
    CPPUNIT_TEST(testCreateTriangleMesh);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCreateTriangleMesh() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0,
            0, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            0, 2, 3
        };
        std::vector<mesh::VertexPtr> vertexPtrArray;
        std::vector<mesh::EdgePtr> edgePtrArray;
        std::vector<mesh::FacePtr> facePtrArray;
        CreateTriangleMesh(&mesh, 
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrArray, &edgePtrArray, &facePtrArray);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(vertexPtrArray.size() == 4);
        CPPUNIT_ASSERT(edgePtrArray.size() == 5);
        CPPUNIT_ASSERT(facePtrArray.size() == 2);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CreateTriangleMeshTest);
