// Copyright 2008 Drew Olbrich

#include <cmath>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector2f.h>
#include <delaunay/MeshConsistency.h>
#include <delaunay/Mesh.h>
#include <delaunay/Vertex.h>
#include <delaunay/Edge.h>
#include <delaunay/Face.h>

using delaunay::TestMeshConsistency;
using delaunay::Mesh;
using delaunay::MeshPtr;
using delaunay::Vertex;
using delaunay::Edge;
using delaunay::Face;
using delaunay::VertexPtr;
using delaunay::EdgePtr;
using delaunay::FacePtr;
using cgmath::Vector2f;

class MeshConsistencyTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MeshConsistencyTest);
    CPPUNIT_TEST(testIsConsistent);
    CPPUNIT_TEST(testNanVertices);
    CPPUNIT_TEST(testInfinityVertices);
    CPPUNIT_TEST(testDuplicateVertices);
    CPPUNIT_TEST(testDuplicateEdges);
    CPPUNIT_TEST(testDuplicateFaces);
    CPPUNIT_TEST(testVerticesWithZeroEdges);
    CPPUNIT_TEST(testVerticesWithOneEdge);
    CPPUNIT_TEST(testVerticesWithDuplicateEdges);
    CPPUNIT_TEST(testVerticesWithoutFaces);
    CPPUNIT_TEST(testVerticesWithDuplicateFaces);
    CPPUNIT_TEST(testEdgesWithoutTwoVertices);
    CPPUNIT_TEST(testEdgesWithDuplicateVertices);
    CPPUNIT_TEST(testEdgesWithoutFaces);
    CPPUNIT_TEST(testEdgesWithDuplicateFaces);
    CPPUNIT_TEST(testFacesWithoutThreeVertices);
    CPPUNIT_TEST(testFacesWithDuplicateVertices);
    CPPUNIT_TEST(testFacesWithoutThreeEdges);
    CPPUNIT_TEST(testFacesWithDuplicateEdges);
    CPPUNIT_TEST(testFacesWithZeroArea);
    CPPUNIT_TEST(testFacesWithClockwiseVertices);
    CPPUNIT_TEST(testVertexEdgesWithoutBackpointer);
    CPPUNIT_TEST(testVertexFacesWithoutBackpointer);
    CPPUNIT_TEST(testEdgeFacesWithoutBackpointer);
    CPPUNIT_TEST(testEdgeVerticesWithoutBackpointer);
    CPPUNIT_TEST(testFaceEdgesWithoutBackpointer);
    CPPUNIT_TEST(testFaceVerticesWithoutBackpointer);
    CPPUNIT_TEST(testFacesWithMismatchedVertexEdgeOrder);
    CPPUNIT_TEST_SUITE_END();

private:
    MeshPtr mMeshPtr;
    VertexPtr mV1;
    VertexPtr mV2;
    VertexPtr mV3;
    EdgePtr mE1;
    EdgePtr mE2;
    EdgePtr mE3;
    FacePtr mF1;

public:
    void setUp() {
        mMeshPtr.reset(new Mesh);

        mV1 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mE1 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();

        mF1 = mMeshPtr->createFace();

        mV1->setPosition(Vector2f(0, 0));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(5, 10));

        mV1->addAdjacentEdge(mE1);
        mV1->addAdjacentEdge(mE3);

        mV2->addAdjacentEdge(mE1);
        mV2->addAdjacentEdge(mE2);

        mV3->addAdjacentEdge(mE2);
        mV3->addAdjacentEdge(mE3);

        mV1->addAdjacentFace(mF1);
        mV2->addAdjacentFace(mF1);
        mV3->addAdjacentFace(mF1);

        mE1->addAdjacentVertex(mV1);
        mE1->addAdjacentVertex(mV2);
        mE1->addAdjacentFace(mF1);

        mE2->addAdjacentVertex(mV2);
        mE2->addAdjacentVertex(mV3);
        mE2->addAdjacentFace(mF1);

        mE3->addAdjacentVertex(mV3);
        mE3->addAdjacentVertex(mV1);
        mE3->addAdjacentFace(mF1);

        mF1->addAdjacentVertex(mV1);
        mF1->addAdjacentVertex(mV2);
        mF1->addAdjacentVertex(mV3);
        mF1->addAdjacentEdge(mE1);
        mF1->addAdjacentEdge(mE2);
        mF1->addAdjacentEdge(mE3);
    }

    void tearDown() {
        mMeshPtr.reset();
    }

    void testIsConsistent() {
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr).isConsistent());
    }

    void testNanVertices() {
        mV1->setPosition(Vector2f(sqrt(-1.0), 0));
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr)
            .inconsistencyCount(delaunay::NAN_VERTICES) > 0);
    }

    void testInfinityVertices() {
        float zero = 0.0;
        mV1->setPosition(Vector2f(1.0/zero, 0));
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::INFINITY_VERTICES) > 0);
    }

    void testDuplicateVertices() {
        mV1->setPosition(Vector2f(3, 0));
        mV2->setPosition(Vector2f(3, 0));
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::DUPLICATE_VERTICES) > 0);
    }

    void testDuplicateEdges() {
        EdgePtr edgePtr = mMeshPtr->createEdge();
        edgePtr->addAdjacentVertex(mV1);
        edgePtr->addAdjacentVertex(mV2);
        edgePtr->addAdjacentFace(mF1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::DUPLICATE_EDGES) > 0);
    }

    void testDuplicateFaces() {
        FacePtr facePtr = mMeshPtr->createFace();
        facePtr->addAdjacentVertex(mV1);
        facePtr->addAdjacentVertex(mV2);
        facePtr->addAdjacentVertex(mV3);
        facePtr->addAdjacentEdge(mE1);
        facePtr->addAdjacentEdge(mE2);
        facePtr->addAdjacentEdge(mE3);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::DUPLICATE_FACES) > 0);
    }

    void testVerticesWithZeroEdges() {
        VertexPtr vertexPtr = mMeshPtr->createVertex();
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::VERTICES_WITH_ZERO_EDGES) > 0);
    }

    void testVerticesWithOneEdge() {
        VertexPtr vertexPtr = mMeshPtr->createVertex();
        EdgePtr edgePtr = mMeshPtr->createEdge();
        vertexPtr->addAdjacentEdge(edgePtr);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::VERTICES_WITH_ONE_EDGE) > 0);
    }

    void testVerticesWithDuplicateEdges() {
        // We can't test this, because adding a duplicate edge triggers an assert.
    }

    void testVerticesWithoutFaces() {
        VertexPtr vertexPtr = mMeshPtr->createVertex();
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::VERTICES_WITHOUT_FACES) > 0);
    }

    void testVerticesWithDuplicateFaces() {
        // We can't test this, because adding a duplicate face triggers an assert.
    }

    void testEdgesWithoutTwoVertices() {
        mE1->removeAdjacentVertex(mV1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::EDGES_WITHOUT_TWO_VERTICES) > 0);
    }

    void testEdgesWithDuplicateVertices() {
        mE1->removeAdjacentVertex(mV2);
        mE1->addAdjacentVertex(mV1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::EDGES_WITH_DUPLICATE_VERTICES) > 0);
    }

    void testEdgesWithoutFaces() {
        mE1->removeAdjacentFace(mF1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::EDGES_WITHOUT_FACES) > 0);
    }

    void testEdgesWithDuplicateFaces() {
        mE1->addAdjacentFace(mF1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::EDGES_WITH_DUPLICATE_FACES) > 0);
    }

    void testFacesWithoutThreeVertices() {
        mF1->removeAdjacentVertex(mV1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITHOUT_THREE_VERTICES) > 0);
    }

    void testFacesWithDuplicateVertices() {
        mF1->removeAdjacentVertex(mV1);
        mF1->addAdjacentVertex(mV2);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITH_DUPLICATE_VERTICES) > 0);
    }

    void testFacesWithoutThreeEdges() {
        mF1->removeAdjacentEdge(mE1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITHOUT_THREE_EDGES) > 0);
    }

    void testFacesWithDuplicateEdges() {
        mF1->removeAdjacentEdge(mE1);
        mF1->addAdjacentEdge(mE2);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITH_DUPLICATE_EDGES) > 0);
    }

    void testFacesWithZeroArea() {
        mV1->setPosition(Vector2f(0, 0));
        mV2->setPosition(Vector2f(5, 0));
        mV3->setPosition(Vector2f(10, 0));
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITH_ZERO_AREA) > 0);
    }

    void testFacesWithClockwiseVertices() {
        mV1->setPosition(Vector2f(5, 10));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(0, 0));
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITH_CLOCKWISE_VERTICES) > 0);
    }

    void testVertexEdgesWithoutBackpointer() {
        mE1->removeAdjacentVertex(mV1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::VERTEX_EDGES_WITHOUT_BACKPOINTER) > 0);
    }

    void testVertexFacesWithoutBackpointer() {
        mF1->removeAdjacentVertex(mV1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::VERTEX_FACES_WITHOUT_BACKPOINTER) > 0);
    }

    void testEdgeFacesWithoutBackpointer() {
        mF1->removeAdjacentEdge(mE1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::EDGE_FACES_WITHOUT_BACKPOINTER) > 0);
    }

    void testEdgeVerticesWithoutBackpointer() {
        mV1->removeAdjacentEdge(mE1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::EDGE_VERTICES_WITHOUT_BACKPOINTER) > 0);
    }

    void testFaceEdgesWithoutBackpointer() {
        mE1->removeAdjacentFace(mF1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACE_EDGES_WITHOUT_BACKPOINTER) > 0);
    }

    void testFaceVerticesWithoutBackpointer() {
        mV1->removeAdjacentFace(mF1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACE_VERTICES_WITHOUT_BACKPOINTER) > 0);
    }

    void testFacesWithMismatchedVertexEdgeOrder() {
        mF1->removeAdjacentEdge(mE1);
        mF1->removeAdjacentEdge(mE2);
        mF1->removeAdjacentEdge(mE3);
        mF1->addAdjacentEdge(mE3);
        mF1->addAdjacentEdge(mE2);
        mF1->addAdjacentEdge(mE1);
        CPPUNIT_ASSERT(TestMeshConsistency(mMeshPtr) 
            .inconsistencyCount(delaunay::FACES_WITH_MISMATCHED_VERTEX_EDGE_ORDER) > 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MeshConsistencyTest);
