// Copyright 2008 Drew Olbrich

#include <cassert>
#include <cmath>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3f.h>
#include <mesh/EdgeOperations.h>
#include <mesh/VertexOperations.h>
#include <mesh/IsConsistent.h>
#include <mesh/AttributeKey.h>
#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <mesh/DeletedElementTracker.h>
#include <meshprim/CreateTriangleMesh.h>

using cgmath::Vector3f;
using mesh::Mesh;
using mesh::AttributeKey;
using mesh::VertexPtr;
using mesh::EdgePtr;
using mesh::FacePtr;

class EdgeOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EdgeOperationsTest);
    CPPUNIT_TEST(testGetEdgeAdjacentVertices);
    CPPUNIT_TEST(testGetEdgeVertexPositions);
    CPPUNIT_TEST(testGetEdgeLength);
    CPPUNIT_TEST(testDeleteEdge);
    CPPUNIT_TEST(testSplitEdge);
    CPPUNIT_TEST(testCollapseEdgeTwoTrianglesSideEdge);
    CPPUNIT_TEST(testCollapseEdgeTwoTrianglesShareEdge);
    CPPUNIT_TEST(testCollapseEdgeFourTriangles);
    CPPUNIT_TEST(testCollapseEdgeFourTrianglesInteriorEdge);
    CPPUNIT_TEST(testGetEpsilonFromEdge);
    CPPUNIT_TEST(testMergeEdgeIntoEdge);
    CPPUNIT_TEST(testMergeEdgeIntoEdgeWithSharedVertices);
    CPPUNIT_TEST(testGetEdgeBoundingBox);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testGetEdgeAdjacentVertices() {
        Mesh mesh;
        mesh::EdgePtr edge = mesh.createEdge();
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        edge->addAdjacentVertex(v1);
        edge->addAdjacentVertex(v2);

        mesh::VertexPtr r1;
        mesh::VertexPtr r2;
        mesh::GetEdgeAdjacentVertices(edge, &r1, &r2);

        CPPUNIT_ASSERT(r1 == v1);
        CPPUNIT_ASSERT(r2 == v2);
    }

    void testGetEdgeVertexPositions() {
        Mesh mesh;
        mesh::EdgePtr edge = mesh.createEdge();
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        edge->addAdjacentVertex(v1);
        edge->addAdjacentVertex(v2);

        v1->setPosition(cgmath::Vector3f(1, 2, 3));
        v2->setPosition(cgmath::Vector3f(4, 5, 6));

        cgmath::Vector3f p0;
        cgmath::Vector3f p1;
        mesh::GetEdgeVertexPositions(edge, &p0, &p1);

        CPPUNIT_ASSERT(p0 == cgmath::Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(p1 == cgmath::Vector3f(4, 5, 6));
    }

    void testGetEdgeLength() {
        Mesh mesh;
        mesh::EdgePtr edge = mesh.createEdge();
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        edge->addAdjacentVertex(v1);
        edge->addAdjacentVertex(v2);

        v1->setPosition(cgmath::Vector3f(1, 2, 3));
        v2->setPosition(cgmath::Vector3f(4, 5, 6));

        float computedLength = sqrtf(3*3 + 3*3 + 3*3);

        float edgeLength = mesh::GetEdgeLength(edge);

        CPPUNIT_ASSERT(fabsf(edgeLength - computedLength) < 0.001);
    }

    void testDeleteEdge() {
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
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh::EdgePtr deletedEdge = edgePtrVector[0];
        mesh::DeleteEdge(&mesh, deletedEdge);

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 4);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        for (size_t index = 0; index < vertexPtrVector.size(); ++index) {
            CPPUNIT_ASSERT(!vertexPtrVector[index]->hasAdjacentEdge(deletedEdge));
        }
        for (size_t index = 0; index < facePtrVector.size(); ++index) {
            CPPUNIT_ASSERT(!facePtrVector[index]->hasAdjacentEdge(deletedEdge));
        }
    }

    void testSplitEdge() {
        Mesh mesh;

        FacePtr face = mesh.createFace();
        
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();

        v1->setPosition(Vector3f(0, 0, 0));
        v2->setPosition(Vector3f(2, 0, 0));
        v3->setPosition(Vector3f(0, 2, 0));

        AttributeKey key = mesh.getAttributeKey("testKey", 
            AttributeKey::FLOAT);
        v1->setFloat(key, 1.0);
        v2->setFloat(key, 2.0);
        v3->setFloat(key, 16.0);
        
        face->addAdjacentVertex(v1);
        face->addAdjacentVertex(v2);
        face->addAdjacentVertex(v3);
        
        v1->addAdjacentFace(face);
        v2->addAdjacentFace(face);
        v3->addAdjacentFace(face);

        EdgePtr e12 = mesh.createEdge();
        EdgePtr e23 = mesh.createEdge();
        EdgePtr e31 = mesh.createEdge();

        v1->addAdjacentEdge(e12);
        v1->addAdjacentEdge(e31);
        v2->addAdjacentEdge(e23);
        v2->addAdjacentEdge(e12);
        v3->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e31);

        face->addAdjacentEdge(e12);
        face->addAdjacentEdge(e23);
        face->addAdjacentEdge(e31);
        
        e12->addAdjacentFace(face);
        e23->addAdjacentFace(face);
        e31->addAdjacentFace(face);

        e12->addAdjacentVertex(v1);
        e12->addAdjacentVertex(v2);

        e23->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v3);

        e31->addAdjacentVertex(v3);
        e31->addAdjacentVertex(v1);
        
        VertexPtr newVertexPtr;
        EdgePtr newEdgePtr;
        SplitEdge(&mesh, e12, Vector3f(1, 0, 0), &newVertexPtr, &newEdgePtr);

        CPPUNIT_ASSERT(newVertexPtr->getFloat(key) == 1.5);

        CPPUNIT_ASSERT(face->adjacentVertexCount() == 4);
        CPPUNIT_ASSERT(face->adjacentEdgeCount() == 4);
        
        CPPUNIT_ASSERT(IsConsistent(mesh));
    }

    void testCollapseEdgeTwoTrianglesSideEdge() {
        mesh::Mesh mesh;

        //  3---2
        //  |  /|
        //  | / |
        //  |/  |
        //  0---1
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
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh::DeletedElementTracker deletedElementTracker;
        mesh::VertexPtr remainingVertex = CollapseEdge(&mesh, edgePtrVector[0], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 3);
        CPPUNIT_ASSERT(mesh.faceCount() == 1);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[2]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[0]));

        CPPUNIT_ASSERT(remainingVertex == vertexPtrVector[1]);
    }

    void testCollapseEdgeTwoTrianglesShareEdge() {
        mesh::Mesh mesh;

        //  3---2
        //  |  /|
        //  | / |
        //  |/  |
        //  0---1
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
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh::DeletedElementTracker deletedElementTracker;
        mesh::VertexPtr remainingVertex = CollapseEdge(&mesh, edgePtrVector[2], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 2);
        CPPUNIT_ASSERT(mesh.faceCount() == 0);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[2]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[2]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[3]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[0]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[1]));

        CPPUNIT_ASSERT(remainingVertex == vertexPtrVector[0]);
    }

    void testCollapseEdgeFourTriangles() {
        mesh::Mesh mesh;

        //  4---3---2---5   4---3-------5
        //   \  |  /|  /     \  |\     /
        //    \ | / | /       \ | \   /
        //     \|/  |/         \|  \ /
        //      0---1           0---1
        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0,
             0, 5, 0,
            -5, 5, 0,
            10, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            1, 5, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 9);
        CPPUNIT_ASSERT(mesh.faceCount() == 4);

        mesh::VertexPtr v1;
        mesh::VertexPtr v2;
        mesh::GetEdgeAdjacentVertices(edgePtrVector[3], &v1, &v2);
        CPPUNIT_ASSERT(v1 == vertexPtrVector[2]);
        CPPUNIT_ASSERT(v2 == vertexPtrVector[3]);

        mesh::DeletedElementTracker deletedElementTracker;
        mesh::VertexPtr remainingVertex = mesh::CollapseEdge(&mesh, edgePtrVector[3], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 5);
        CPPUNIT_ASSERT(mesh.edgeCount() == 7);
        CPPUNIT_ASSERT(mesh.faceCount() == 3);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[2]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[3]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[1]));

        CPPUNIT_ASSERT(remainingVertex == vertexPtrVector[3]);
    }

    void testCollapseEdgeFourTrianglesInteriorEdge() {
        mesh::Mesh mesh;

        //  4---3---2---5   4       2---5
        //   \  |  /|  /     \     /|  /
        //    \ | / | /       \   / | /
        //     \|/  |/         \ /  |/
        //      0---1           0---1
        float vertexPositionArray[] = {
            0, 0, 0,
            5, 0, 0,
            5, 5, 0,
             0, 5, 0,
            -5, 5, 0,
            10, 5, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            0, 2, 3,
            0, 3, 4,
            1, 5, 2
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 9);
        CPPUNIT_ASSERT(mesh.faceCount() == 4);

        mesh::VertexPtr v1;
        mesh::VertexPtr v2;
        mesh::GetEdgeAdjacentVertices(edgePtrVector[4], &v1, &v2);
        CPPUNIT_ASSERT(v1 == vertexPtrVector[3]);
        CPPUNIT_ASSERT(v2 == vertexPtrVector[0]);

        mesh::DeletedElementTracker deletedElementTracker;
        mesh::VertexPtr remainingVertex = mesh::CollapseEdge(&mesh, edgePtrVector[4], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 5);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[3]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[3]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[4]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[5]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[2]));

        CPPUNIT_ASSERT(remainingVertex == vertexPtrVector[0]);
    }

    void testGetEpsilonFromEdge() {
        Mesh mesh;
        EdgePtr e1 = mesh.createEdge();
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v2);
        v1->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e1);

        v1->setPosition(Vector3f(1, 2, 3));
        v2->setPosition(Vector3f(4, 5, 6));
        CPPUNIT_ASSERT(mesh::GetEpsilonFromEdge(e1, 1.0, 1.0) == 6.0);

        v1->setPosition(Vector3f(1, 2, 3));
        v2->setPosition(Vector3f(4, 5, -6));
        CPPUNIT_ASSERT(mesh::GetEpsilonFromEdge(e1, 1.0, 1.0) == 6.0);

        v1->setPosition(Vector3f(1, 2, 3));
        v2->setPosition(Vector3f(4, 5, 6));
        CPPUNIT_ASSERT(mesh::GetEpsilonFromEdge(e1, 10.0, 1.0) == 10.0);
    }

    void testMergeEdgeIntoEdge() {
        Mesh mesh;

        // 5---43---2
        //  \  ||  /
        //   \ || /
        //    \||/
        //     01
        float vertexPositionArray[] = {
            1, 0, 0,
            1, 0, 0,
            2, 1, 0,
            1, 1, 0,
            1, 1, 0,
            0, 1, 0
        };
        int faceVertexIndexArray[] = {
            0, 4, 5,
            1, 2, 3
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh::DeletedElementTracker deletedElementTracker;

        // Swap the order of the vertices on the second edge so they'll
        // be merged in the order we want.
        VertexPtr v3;
        VertexPtr v1;
        GetEdgeAdjacentVertices(edgePtrVector[5], &v3, &v1);
        assert(v3 == vertexPtrVector[3]);
        assert(v1 == vertexPtrVector[1]);
        edgePtrVector[5]->removeAdjacentVertex(v3);
        edgePtrVector[5]->removeAdjacentVertex(v1);
        edgePtrVector[5]->addAdjacentVertex(v1);
        edgePtrVector[5]->addAdjacentVertex(v3);

        MergeEdgeIntoEdge(&mesh, edgePtrVector[0], edgePtrVector[5], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);
    }

    void testMergeEdgeIntoEdgeWithSharedVertices() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            0, 0, 0,
            1, 1, 0,
            0, 1, 0,
            1, 0, 0,
            2, 0, 0,
            2, 1, 0,
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            3, 4, 5
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;

        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh::DeletedElementTracker deletedElementTracker;

        // Merge the vertices.
        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[3], vertexPtrVector[0], &deletedElementTracker);
        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[5], vertexPtrVector[1], &deletedElementTracker);

        // Merge the edge.
        MergeEdgeIntoEdgeWithSharedVertices(&mesh,
            edgePtrVector[5], edgePtrVector[0], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentFace(facePtrVector[0]));
        CPPUNIT_ASSERT(vertexPtrVector[1]->hasAdjacentFace(facePtrVector[1]));

        CPPUNIT_ASSERT(edgePtrVector[0]->hasAdjacentFace(facePtrVector[0]));
        CPPUNIT_ASSERT(edgePtrVector[0]->hasAdjacentFace(facePtrVector[1]));
    }

    void testGetEdgeBoundingBox() {
        Mesh mesh;
        EdgePtr e1 = mesh.createEdge();
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v2);
        v1->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e1);

        v1->setPosition(Vector3f(1, 2, 3));
        v2->setPosition(Vector3f(4, 5, 6));
        CPPUNIT_ASSERT(mesh::GetEdgeBoundingBox(e1)
            == cgmath::BoundingBox3f(Vector3f(1, 2, 3), Vector3f(4, 5, 6)));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(EdgeOperationsTest);
