// Copyright 2010 Drew Olbrich

#include <cassert>

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/SplitEdgeTriangulator.h>
#include <mesh/Mesh.h>
#include <mesh/EdgeOperations.h>
#include <meshprim/CreateTriangleMesh.h>

class SplitEdgeTriangulatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SplitEdgeTriangulatorTest);
    CPPUNIT_TEST(testTriangulateNoEdgesSplit);
    CPPUNIT_TEST(testTriangulateOneEdgeSplit);
    CPPUNIT_TEST(testTriangulateTwoEdgesSplit);
    CPPUNIT_TEST(testTriangulateThreeEdgesSplit);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testTriangulateNoEdgesSplit() {
        mesh::Mesh mesh;
        createSplitTriangle(&mesh, 0);
        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 3);
        CPPUNIT_ASSERT(mesh.faceCount() == 1);
    }

    void testTriangulateOneEdgeSplit() {
        mesh::Mesh mesh;
        createSplitTriangle(&mesh, 1);
        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);
    }

    void testTriangulateTwoEdgesSplit() {
        mesh::Mesh mesh;
        createSplitTriangle(&mesh, 2);
        CPPUNIT_ASSERT(mesh.vertexCount() == 5);
        CPPUNIT_ASSERT(mesh.edgeCount() == 7);
        CPPUNIT_ASSERT(mesh.faceCount() == 3);
    }

    void testTriangulateThreeEdgesSplit() {
        mesh::Mesh mesh;
        createSplitTriangle(&mesh, 3);
        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 9);
        CPPUNIT_ASSERT(mesh.faceCount() == 4);
    }

    // Add a triangle to the mesh, and then split the specified number
    // of edges of the triangle using SplitEdgeTriangulator.
    void createSplitTriangle(mesh::Mesh *mesh, unsigned edgesToSplit) {
        assert(edgesToSplit <= 3);

        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        mesh::SplitEdgeTriangulator splitEdgeTriangulator;
        splitEdgeTriangulator.setMesh(mesh);
        splitEdgeTriangulator.initialize();

        for (unsigned index = 0; index < edgesToSplit; ++index) {
            assert(index < edgePtrVector.size());
            mesh::EdgePtr edgeToSplit = edgePtrVector[index];
            cgmath::Vector3f p0;
            cgmath::Vector3f p1;
            mesh::GetEdgeVertexPositions(edgeToSplit, &p0, &p1);
            cgmath::Vector3f newVertexPosition = (p0 + p1)*0.5;
            mesh::VertexPtr newVertexPtr;
            mesh::SplitEdge(mesh, edgeToSplit, newVertexPosition, &newVertexPtr, NULL);
            newVertexPtr->setBool(splitEdgeTriangulator.splitEdgeVertexAttributeKey(), true);
        }

        splitEdgeTriangulator.triangulate();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(SplitEdgeTriangulatorTest);
