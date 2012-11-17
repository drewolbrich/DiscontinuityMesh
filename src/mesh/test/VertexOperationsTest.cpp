// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/VertexOperations.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <mesh/DeletedElementTracker.h>
#include <meshprim/CreateTriangleMesh.h>

class VertexOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VertexOperationsTest);
    CPPUNIT_TEST(testDeleteVertex);
    CPPUNIT_TEST(testMergeVertexIntoVertex);
    CPPUNIT_TEST(testMergeVertexIntoVertexTwoSharedEdges);
    CPPUNIT_TEST(testMergeVertexIntoNonadjacentVertexTwoTrianglesOneVertex);
    CPPUNIT_TEST(testMergeVertexIntoNonadjacentVertexTwoTrianglesTwoVertices);
    CPPUNIT_TEST(testMergeVertexIntoNonadjacentVertexFourTriangles);
    CPPUNIT_TEST(testFindEdgeConnectingVertices);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testDeleteVertex() {
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

        mesh::VertexPtr deletedVertex = vertexPtrVector[1];
        mesh::DeleteVertex(&mesh, deletedVertex);

        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 5);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        for (size_t index = 0; index < edgePtrVector.size(); ++index) {
            CPPUNIT_ASSERT(!edgePtrVector[index]->hasAdjacentVertex(deletedVertex));
        }
        for (size_t index = 0; index < facePtrVector.size(); ++index) {
            CPPUNIT_ASSERT(!facePtrVector[index]->hasAdjacentVertex(deletedVertex));
        }
    }

    void testMergeVertexIntoVertex() {
        mesh::Mesh mesh;

        // 5---4---3    5-------3
        // |\  |  /|    |\     /|    
        // | \ | / |    | \   / |    
        // |  \|/  |    |  \ /  |    
        // 0---1---2    0---1---2    
        float vertexPositionArray[] = {
            0, 0, 0,
            1, 0, 0,
            2, 0, 0,
            2, 1, 0,
            1, 1, 0,
            0, 1, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 5,
            1, 4, 5,
            1, 2, 3,
            1, 3, 4
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

        mesh::DeletedElementTracker deletedElementTracker;
        MergeVertexIntoVertex(&mesh, vertexPtrVector[4], vertexPtrVector[5], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 5);
        CPPUNIT_ASSERT(mesh.edgeCount() == 7);
        CPPUNIT_ASSERT(mesh.faceCount() == 3);
    }

    void testMergeVertexIntoVertexTwoSharedEdges() {
        mesh::Mesh mesh;

        // This tests merging vertices that have two shared edges.

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

        // To create the two shared edges, we start with two separate
        // triangles and then merge two vertex pairs together.

        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[3], vertexPtrVector[4], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 5);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[1], vertexPtrVector[0], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        // Then merge the two vertices that share the two edges.

        MergeVertexIntoVertex(&mesh, vertexPtrVector[4], vertexPtrVector[0], 
            &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 2);
        CPPUNIT_ASSERT(mesh.faceCount() == 0);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[3]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[0]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[1]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[4]));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(edgePtrVector[5]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[0]));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(facePtrVector[1]));
    }

    void testMergeVertexIntoNonadjacentVertexTwoTrianglesOneVertex() {
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

        mesh::AttributeKey key1 = mesh.getAttributeKey("testKey1", mesh::AttributeKey::FLOAT);
        vertexPtrVector[0]->setFloat(key1, 1.0);
        vertexPtrVector[3]->setFloat(key1, 2.0);

        mesh::AttributeKey key2 = mesh.getAttributeKey("testKey2", mesh::AttributeKey::FLOAT);
        vertexPtrVector[3]->setFloat(key2, 3.0);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 6);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentFace(facePtrVector[0]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentFace(facePtrVector[1]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[0]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[1]));
        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[2]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[3]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[4]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[5]));

        mesh::DeletedElementTracker deletedElementTracker;
        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[3], vertexPtrVector[0], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 5);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentFace(facePtrVector[0]));
        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentFace(facePtrVector[1]));

        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[0]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[1]));
        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[2]));
        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[3]));
        CPPUNIT_ASSERT(!vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[4]));
        CPPUNIT_ASSERT(vertexPtrVector[0]->hasAdjacentEdge(edgePtrVector[5]));

        CPPUNIT_ASSERT(facePtrVector[1]->hasAdjacentVertex(vertexPtrVector[0]));

        CPPUNIT_ASSERT(edgePtrVector[3]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(!edgePtrVector[4]->hasAdjacentVertex(vertexPtrVector[0]));
        CPPUNIT_ASSERT(edgePtrVector[5]->hasAdjacentVertex(vertexPtrVector[0]));

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(vertexPtrVector[3]));

        for (mesh::VertexPtr vertexPtr = mesh.vertexBegin();
             vertexPtr != mesh.vertexEnd(); ++vertexPtr) {
            CPPUNIT_ASSERT(vertexPtr != vertexPtrVector[3]);
        }

        // Not copied because it already existed in the target vertex.
        CPPUNIT_ASSERT(vertexPtrVector[0]->getFloat(key1) == 1.0);

        // Copied because it did not already exists in the target vertex.
        CPPUNIT_ASSERT(vertexPtrVector[0]->getFloat(key2) == 3.0);
    }

    void testMergeVertexIntoNonadjacentVertexTwoTrianglesTwoVertices() {
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
        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[3], vertexPtrVector[0], &deletedElementTracker);
        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[5], vertexPtrVector[1], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 4);
        CPPUNIT_ASSERT(mesh.edgeCount() == 6);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        CPPUNIT_ASSERT(vertexPtrVector[0]->adjacentEdgeCount() == 4);
        CPPUNIT_ASSERT(vertexPtrVector[1]->adjacentEdgeCount() == 4);

        CPPUNIT_ASSERT(vertexPtrVector[0]->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(vertexPtrVector[1]->adjacentFaceCount() == 2);
    }

    void testMergeVertexIntoNonadjacentVertexFourTriangles() {
        mesh::Mesh mesh;

        float vertexPositionArray[] = {
            0, 0, 0,
            1, 1, 0,
            0, 1, 0,
            1, 0, 0,
            2, 0, 0,
            2, 1, 0,
            -2, 1, 0,
            3, 0, 0
        };
        int faceVertexIndexArray[] = {
            0, 1, 2,
            3, 4, 5,
            0, 2, 6,
            3, 7, 4
        };
        std::vector<mesh::VertexPtr> vertexPtrVector;
        std::vector<mesh::EdgePtr> edgePtrVector;
        std::vector<mesh::FacePtr> facePtrVector;
        meshprim::CreateTriangleMesh(&mesh,
            sizeof(vertexPositionArray)/sizeof(float)/3, vertexPositionArray,
            sizeof(faceVertexIndexArray)/sizeof(int)/3, faceVertexIndexArray,
            &vertexPtrVector, &edgePtrVector, &facePtrVector);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 8);
        CPPUNIT_ASSERT(mesh.edgeCount() == 10);
        CPPUNIT_ASSERT(mesh.faceCount() == 4);

        CPPUNIT_ASSERT(vertexPtrVector[0]->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(vertexPtrVector[3]->adjacentEdgeCount() == 3);

        CPPUNIT_ASSERT(vertexPtrVector[0]->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(vertexPtrVector[3]->adjacentFaceCount() == 2);

        mesh::DeletedElementTracker deletedElementTracker;
        MergeVertexIntoNonadjacentVertex(&mesh, 
            vertexPtrVector[3], vertexPtrVector[0], &deletedElementTracker);

        CPPUNIT_ASSERT(mesh::IsConsistent(mesh));

        CPPUNIT_ASSERT(mesh.vertexCount() == 7);
        CPPUNIT_ASSERT(mesh.edgeCount() == 10);
        CPPUNIT_ASSERT(mesh.faceCount() == 4);

        CPPUNIT_ASSERT(vertexPtrVector[0]->adjacentEdgeCount() == 6);
        CPPUNIT_ASSERT(vertexPtrVector[0]->adjacentFaceCount() == 4);
    }

    void testFindEdgeConnectingVertices() {
        mesh::Mesh mesh;

        mesh::VertexPtr v0 = mesh.createVertex();
        mesh::VertexPtr v1 = mesh.createVertex();
        mesh::VertexPtr v2 = mesh.createVertex();
        mesh::VertexPtr v3 = mesh.createVertex();

        mesh::EdgePtr e01 = mesh.createEdge();
        mesh::EdgePtr e12 = mesh.createEdge();
        mesh::EdgePtr e23 = mesh.createEdge();
        mesh::EdgePtr e30 = mesh.createEdge();

        v0->addAdjacentEdge(e30);
        v0->addAdjacentEdge(e01);

        v1->addAdjacentEdge(e01);
        v1->addAdjacentEdge(e12);

        v2->addAdjacentEdge(e12);
        v2->addAdjacentEdge(e23);

        v3->addAdjacentEdge(e23);
        v3->addAdjacentEdge(e30);

        e01->addAdjacentVertex(v0);
        e01->addAdjacentVertex(v1);

        e12->addAdjacentVertex(v1);
        e12->addAdjacentVertex(v2);

        e23->addAdjacentVertex(v2);
        e23->addAdjacentVertex(v3);

        e30->addAdjacentVertex(v3);
        e30->addAdjacentVertex(v0);

        mesh::EdgePtr edgePtr;

        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v0, v1, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e01);
        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v1, v0, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e01);

        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v1, v2, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e12);
        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v2, v1, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e12);

        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v2, v3, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e23);
        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v3, v2, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e23);

        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v3, v0, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e30);
        CPPUNIT_ASSERT(mesh::FindEdgeConnectingVertices(v0, v3, &edgePtr));
        CPPUNIT_ASSERT(edgePtr == e30);

        CPPUNIT_ASSERT(!mesh::FindEdgeConnectingVertices(v0, v2, &edgePtr));
        CPPUNIT_ASSERT(!mesh::FindEdgeConnectingVertices(v2, v0, &edgePtr));
        CPPUNIT_ASSERT(!mesh::FindEdgeConnectingVertices(v1, v3, &edgePtr));
        CPPUNIT_ASSERT(!mesh::FindEdgeConnectingVertices(v3, v1, &edgePtr));
        
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VertexOperationsTest);
