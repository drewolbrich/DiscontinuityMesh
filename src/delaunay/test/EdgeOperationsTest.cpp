// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <unittest/NestedAssert.h>

#include <delaunay/EdgeOperations.h>
#include <delaunay/FaceOperations.h>
#include <delaunay/MeshConsistency.h>
#include <delaunay/Mesh.h>
#include <delaunay/Vertex.h>
#include <delaunay/Edge.h>
#include <delaunay/Face.h>

using delaunay::Mesh;
using delaunay::MeshPtr;
using delaunay::Vertex;
using delaunay::Edge;
using delaunay::Face;
using delaunay::VertexPtr;
using delaunay::EdgePtr;
using delaunay::FacePtr;
using cgmath::Vector2f;

class EdgeOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EdgeOperationsTest);
    CPPUNIT_TEST(testSwapEdge1);
    CPPUNIT_TEST(testSwapEdge2);
    CPPUNIT_TEST(testSwapEdge3);
    CPPUNIT_TEST(testSwapEdge4);
    CPPUNIT_TEST(testSwapEdge5);
    CPPUNIT_TEST(testSwapEdgeReversedSharedEdgeVertices1);
    CPPUNIT_TEST(testSwapEdgeReversedSharedEdgeVertices2);
    CPPUNIT_TEST(testSwapEdgeReversedSharedEdgeVertices3);
    CPPUNIT_TEST(testSwapEdgeReversedSharedEdgeVertices4);
    CPPUNIT_TEST(testSwapEdgeReversedSharedEdgeVertices5);
    CPPUNIT_TEST(testSplitEdge1);
    CPPUNIT_TEST(testSplitEdge2);
    CPPUNIT_TEST(testSplitEdge3);
    CPPUNIT_TEST(testSplitEdge4);
    CPPUNIT_TEST(testSplitEdge5);
    CPPUNIT_TEST(testSplitEdgeOneFace);
    CPPUNIT_TEST(testEdgeHasAdjacentVertex);
    CPPUNIT_TEST(testEdgeHasAdjacentVertices);
    CPPUNIT_TEST_SUITE_END();

private:
    // See the diagram in retro/src/delaunay/notes/Mesh-Operations.pdf.

    MeshPtr mMeshPtr;
    VertexPtr mV1;
    VertexPtr mV2;
    VertexPtr mV3;
    VertexPtr mV4;
    EdgePtr mE1;
    EdgePtr mE2;
    EdgePtr mE3;
    EdgePtr mE4;
    EdgePtr mE5;
    FacePtr mF1;
    FacePtr mF2;

public:
    void setUp() {
        mMeshPtr.reset(new Mesh);

        mV4 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV1 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mE1 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();
        mE4 = mMeshPtr->createEdge();
        mE5 = mMeshPtr->createEdge();

        mF2 = mMeshPtr->createFace();
        mF1 = mMeshPtr->createFace();

        mV4->setPosition(Vector2f(0, 0));
        mV2->setPosition(Vector2f(10, 0));
        mV1->setPosition(Vector2f(0, 10));
        mV3->setPosition(Vector2f(10, 10));

        mV1->addAdjacentEdge(mE3);
        mV1->addAdjacentEdge(mE4);
        mV1->addAdjacentEdge(mE1);
        mV1->addAdjacentFace(mF1);
        mV1->addAdjacentFace(mF2);

        mV2->addAdjacentEdge(mE2);
        mV2->addAdjacentEdge(mE5);
        mV2->addAdjacentEdge(mE1);
        mV2->addAdjacentFace(mF1);
        mV2->addAdjacentFace(mF2);

        mV3->addAdjacentEdge(mE2);
        mV3->addAdjacentEdge(mE3);
        mV3->addAdjacentFace(mF1);

        mV4->addAdjacentEdge(mE4);
        mV4->addAdjacentEdge(mE5);
        mV4->addAdjacentFace(mF2);

        mE5->addAdjacentVertex(mV4);
        mE5->addAdjacentVertex(mV2);
        mE5->addAdjacentFace(mF2);

        mE1->addAdjacentVertex(mV2);
        mE1->addAdjacentVertex(mV1);
        mE1->addAdjacentFace(mF2);
        mE1->addAdjacentFace(mF1);

        mE4->addAdjacentVertex(mV4);
        mE4->addAdjacentVertex(mV1);
        mE4->addAdjacentFace(mF2);

        mE2->addAdjacentVertex(mV2);
        mE2->addAdjacentVertex(mV3);
        mE2->addAdjacentFace(mF1);

        mE3->addAdjacentVertex(mV3);
        mE3->addAdjacentVertex(mV1);
        mE3->addAdjacentFace(mF1);

        mF2->addAdjacentVertex(mV4);
        mF2->addAdjacentVertex(mV2);
        mF2->addAdjacentVertex(mV1);
        mF2->addAdjacentEdge(mE5);
        mF2->addAdjacentEdge(mE1);
        mF2->addAdjacentEdge(mE4);

        mF1->addAdjacentVertex(mV2);
        mF1->addAdjacentVertex(mV3);
        mF1->addAdjacentVertex(mV1);
        mF1->addAdjacentEdge(mE2);
        mF1->addAdjacentEdge(mE3);
        mF1->addAdjacentEdge(mE1);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(mE1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(mE1->adjacentVertex(0) == mV2
            || mE1->adjacentVertex(0) == mV1);
        CPPUNIT_ASSERT(mE1->adjacentVertex(1) == mV2
            || mE1->adjacentVertex(1) == mV1);

        CPPUNIT_ASSERT(mE1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(mE1->adjacentFace(0) == mF2
            || mE1->adjacentFace(0) == mF1);
        CPPUNIT_ASSERT(mE1->adjacentFace(1) == mF2
            || mE1->adjacentFace(1) == mF1);

        CPPUNIT_ASSERT(mF2->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(mF2->adjacentEdge(0) == mE1
            || mF2->adjacentEdge(1) == mE1
            || mF2->adjacentEdge(2) == mE1);

        CPPUNIT_ASSERT(mF2->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(mF2->adjacentVertex(0) == mV4
            || mF2->adjacentVertex(0) == mV2
            || mF2->adjacentVertex(0) == mV1);
        CPPUNIT_ASSERT(mF2->adjacentVertex(1) == mV4
            || mF2->adjacentVertex(1) == mV2
            || mF2->adjacentVertex(1) == mV1);
        CPPUNIT_ASSERT(mF2->adjacentVertex(2) == mV4
            || mF2->adjacentVertex(2) == mV2
            || mF2->adjacentVertex(2) == mV1);

        CPPUNIT_ASSERT(mF1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(mF1->adjacentEdge(0) == mE1
            || mF1->adjacentEdge(1) == mE1
            || mF1->adjacentEdge(2) == mE1);

        CPPUNIT_ASSERT(mF1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(mF1->adjacentVertex(0) == mV2
            || mF1->adjacentVertex(0) == mV3
            || mF1->adjacentVertex(0) == mV1);
        CPPUNIT_ASSERT(mF1->adjacentVertex(1) == mV2
            || mF1->adjacentVertex(1) == mV3
            || mF1->adjacentVertex(1) == mV1);
        CPPUNIT_ASSERT(mF1->adjacentVertex(2) == mV2
            || mF1->adjacentVertex(2) == mV3
            || mF1->adjacentVertex(2) == mV1);
    }

    void tearDown() {
        mMeshPtr.reset();
    }

    void testSwapEdge1() {
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdge2() {
        delaunay::RotateFace(mF1);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdge3() {
        delaunay::RotateFace(mF1);
        delaunay::RotateFace(mF1);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdge4() {
        delaunay::RotateFace(mF2);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdge5() {
        delaunay::RotateFace(mF2);
        delaunay::RotateFace(mF2);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdgeReversedSharedEdgeVertices1() {
        exchangeSharedEdgeVertices();
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdgeReversedSharedEdgeVertices2() {
        exchangeSharedEdgeVertices();
        delaunay::RotateFace(mF1);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdgeReversedSharedEdgeVertices3() {
        exchangeSharedEdgeVertices();
        delaunay::RotateFace(mF1);
        delaunay::RotateFace(mF1);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdgeReversedSharedEdgeVertices4() {
        exchangeSharedEdgeVertices();
        delaunay::RotateFace(mF2);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSwapEdgeReversedSharedEdgeVertices5() {
        exchangeSharedEdgeVertices();
        delaunay::RotateFace(mF2);
        delaunay::RotateFace(mF2);
        delaunay::SwapEdge(mE1);
        UNITTEST_NESTED_CALL(verifySwapEdge());
    }

    void testSplitEdge1() {
        VertexPtr v5 = delaunay::SplitEdge(mE1, Vector2f(5, 5), mMeshPtr);
        UNITTEST_NESTED_CALL(verifySplitEdge(v5));
    }

    void testSplitEdge2() {
        delaunay::RotateFace(mF1);
        VertexPtr v5 = delaunay::SplitEdge(mE1, Vector2f(5, 5), mMeshPtr);
        UNITTEST_NESTED_CALL(verifySplitEdge(v5));
    }

    void testSplitEdge3() {
        delaunay::RotateFace(mF1);
        delaunay::RotateFace(mF1);
        VertexPtr v5 = delaunay::SplitEdge(mE1, Vector2f(5, 5), mMeshPtr);
        UNITTEST_NESTED_CALL(verifySplitEdge(v5));
    }

    void testSplitEdge4() {
        delaunay::RotateFace(mF2);
        VertexPtr v5 = delaunay::SplitEdge(mE1, Vector2f(5, 5), mMeshPtr);
        UNITTEST_NESTED_CALL(verifySplitEdge(v5));
    }

    void testSplitEdge5() {
        delaunay::RotateFace(mF2);
        delaunay::RotateFace(mF2);
        VertexPtr v5 = delaunay::SplitEdge(mE1, Vector2f(5, 5), mMeshPtr);
        UNITTEST_NESTED_CALL(verifySplitEdge(v5));
    }

    void testSplitEdgeOneFace() {

        // This is a test of SplitEdge where the specified edge
        // has only one adjacent face.

        mMeshPtr.reset(new Mesh);

        mV1 = mMeshPtr->createVertex();
        mV2 = mMeshPtr->createVertex();
        mV3 = mMeshPtr->createVertex();

        mE1 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();

        mF1 = mMeshPtr->createFace();

        mV1->setPosition(Vector2f(0, 10));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(10, 10));

        mE1->addAdjacentVertex(mV2);
        mE1->addAdjacentVertex(mV1);
        mE1->addAdjacentFace(mF1);

        mE2->addAdjacentVertex(mV2);
        mE2->addAdjacentVertex(mV3);
        mE2->addAdjacentFace(mF1);

        mE3->addAdjacentVertex(mV3);
        mE3->addAdjacentVertex(mV1);
        mE3->addAdjacentFace(mF1);

        mF1->addAdjacentVertex(mV2);
        mF1->addAdjacentVertex(mV3);
        mF1->addAdjacentVertex(mV1);
        mF1->addAdjacentEdge(mE2);
        mF1->addAdjacentEdge(mE3);
        mF1->addAdjacentEdge(mE1);

        mV1->addAdjacentEdge(mE1);
        mV1->addAdjacentEdge(mE3);

        mV2->addAdjacentEdge(mE1);
        mV2->addAdjacentEdge(mE2);

        mV3->addAdjacentEdge(mE2);
        mV3->addAdjacentEdge(mE3);

        mV1->addAdjacentFace(mF1);
        mV2->addAdjacentFace(mF1);
        mV3->addAdjacentFace(mF1);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        VertexPtr v5 = delaunay::SplitEdge(mE1, Vector2f(5, 5), mMeshPtr);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        CPPUNIT_ASSERT(v5->position() == Vector2f(5, 5));
    }

    void testEdgeHasAdjacentVertex() {
        CPPUNIT_ASSERT(delaunay::EdgeHasAdjacentVertex(mE3, mV1));
        CPPUNIT_ASSERT(delaunay::EdgeHasAdjacentVertex(mE3, mV3));
        CPPUNIT_ASSERT(!delaunay::EdgeHasAdjacentVertex(mE3, mV2));
        CPPUNIT_ASSERT(!delaunay::EdgeHasAdjacentVertex(mE3, mV4));
    }

    void testEdgeHasAdjacentVertices() {
        CPPUNIT_ASSERT(delaunay::EdgeHasAdjacentVertices(mE3, mV1, mV3));
        CPPUNIT_ASSERT(!delaunay::EdgeHasAdjacentVertices(mE3, mV1, mV2));
        CPPUNIT_ASSERT(!delaunay::EdgeHasAdjacentVertices(mE3, mV2, mV4));
    }

private:
    bool verifySwapEdge() {
        UNITTEST_NESTED_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        UNITTEST_NESTED_ASSERT(mE1->adjacentVertexCount() == 2);
        UNITTEST_NESTED_ASSERT(mE1->adjacentVertex(0) == mV4
            || mE1->adjacentVertex(0) == mV3);
        UNITTEST_NESTED_ASSERT(mE1->adjacentVertex(1) == mV4
            || mE1->adjacentVertex(1) == mV3);

        UNITTEST_NESTED_ASSERT(mE1->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(mE1->adjacentFace(0) == mF2
            || mE1->adjacentFace(0) == mF1);
        UNITTEST_NESTED_ASSERT(mE1->adjacentFace(1) == mF2
            || mE1->adjacentFace(1) == mF1);

        UNITTEST_NESTED_ASSERT(mF1->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(mF1->adjacentEdge(0) == mE1
            || mF1->adjacentEdge(1) == mE1
            || mF1->adjacentEdge(2) == mE1);

        UNITTEST_NESTED_ASSERT(mF1->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT(mF1->adjacentVertex(0) == mV4
            || mF1->adjacentVertex(0) == mV3
            || mF1->adjacentVertex(0) == mV1);
        UNITTEST_NESTED_ASSERT(mF1->adjacentVertex(1) == mV4
            || mF1->adjacentVertex(1) == mV3         
            || mF1->adjacentVertex(1) == mV1);
        UNITTEST_NESTED_ASSERT(mF1->adjacentVertex(2) == mV4
            || mF1->adjacentVertex(2) == mV3
            || mF1->adjacentVertex(2) == mV1);

        UNITTEST_NESTED_ASSERT(mF2->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(mF2->adjacentEdge(0) == mE1
            || mF2->adjacentEdge(1) == mE1
            || mF2->adjacentEdge(2) == mE1);

        UNITTEST_NESTED_ASSERT(mF2->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT(mF2->adjacentVertex(0) == mV4
            || mF2->adjacentVertex(0) == mV2
            || mF2->adjacentVertex(0) == mV3);
        UNITTEST_NESTED_ASSERT(mF2->adjacentVertex(1) == mV4
            || mF2->adjacentVertex(1) == mV2
            || mF2->adjacentVertex(1) == mV3);
        UNITTEST_NESTED_ASSERT(mF2->adjacentVertex(2) == mV4
            || mF2->adjacentVertex(2) == mV2
            || mF2->adjacentVertex(2) == mV3);

        UNITTEST_NESTED_ASSERT(mV1->adjacentEdgeCount() == 2);
        UNITTEST_NESTED_ASSERT(mV1->adjacentEdge(0) == mE3
            || mV1->adjacentEdge(0) == mE4);
        UNITTEST_NESTED_ASSERT(mV1->adjacentEdge(1) == mE3
            || mV1->adjacentEdge(1) == mE4);

        UNITTEST_NESTED_ASSERT(mV2->adjacentEdgeCount() == 2);
        UNITTEST_NESTED_ASSERT(mV2->adjacentEdge(0) == mE2
            || mV2->adjacentEdge(0) == mE5);
        UNITTEST_NESTED_ASSERT(mV2->adjacentEdge(1) == mE2
            || mV2->adjacentEdge(1) == mE5);

        UNITTEST_NESTED_ASSERT(mV3->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(mV3->adjacentEdge(0) == mE1
            || mV3->adjacentEdge(0) == mE2
            || mV3->adjacentEdge(0) == mE3);
        UNITTEST_NESTED_ASSERT(mV3->adjacentEdge(1) == mE1
            || mV3->adjacentEdge(1) == mE3
            || mV3->adjacentEdge(1) == mE3);
        UNITTEST_NESTED_ASSERT(mV3->adjacentEdge(2) == mE1
            || mV3->adjacentEdge(2) == mE3
            || mV3->adjacentEdge(2) == mE3);

        UNITTEST_NESTED_ASSERT(mV4->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(mV4->adjacentEdge(0) == mE1
            || mV4->adjacentEdge(0) == mE4
            || mV4->adjacentEdge(0) == mE5);
        UNITTEST_NESTED_ASSERT(mV4->adjacentEdge(1) == mE1
            || mV4->adjacentEdge(1) == mE4
            || mV4->adjacentEdge(1) == mE5);
        UNITTEST_NESTED_ASSERT(mV4->adjacentEdge(2) == mE1
            || mV4->adjacentEdge(2) == mE4
            || mV4->adjacentEdge(2) == mE5);

        UNITTEST_NESTED_ASSERT(mV1->adjacentFaceCount() == 1);
        UNITTEST_NESTED_ASSERT(mV1->adjacentFace(0) == mF1);

        UNITTEST_NESTED_ASSERT(mV2->adjacentFaceCount() == 1);
        UNITTEST_NESTED_ASSERT(mV2->adjacentFace(0) == mF2);

        UNITTEST_NESTED_ASSERT(mV3->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(mV3->adjacentFace(0) == mF1
            || mV3->adjacentFace(0) == mF2);
        UNITTEST_NESTED_ASSERT(mV3->adjacentFace(1) == mF1
            || mV3->adjacentFace(1) == mF2);

        UNITTEST_NESTED_ASSERT(mV4->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(mV4->adjacentFace(0) == mF1
            || mV4->adjacentFace(0) == mF2);
        UNITTEST_NESTED_ASSERT(mV4->adjacentFace(1) == mF1
            || mV4->adjacentFace(1) == mF2);

        return true;
    }

    void exchangeSharedEdgeVertices() {
        // Exchange the vertices of the original shared edge.
        // The delaunay::SwapEdge function should be able to handle this.
        VertexPtr ev1 = mE1->adjacentVertex(0);
        VertexPtr ev2 = mE1->adjacentVertex(1);
        mE1->removeAdjacentVertex(ev1);
        mE1->removeAdjacentVertex(ev2);
        mE1->addAdjacentVertex(ev2);
        mE1->addAdjacentVertex(ev1);
    }

    bool verifySplitEdge(VertexPtr v5) {
        UNITTEST_NESTED_ASSERT(delaunay::TestMeshConsistency(mMeshPtr).isConsistent());

        UNITTEST_NESTED_ASSERT(mMeshPtr->vertexCount() == 5);
        UNITTEST_NESTED_ASSERT(mMeshPtr->edgeCount() == 8);
        UNITTEST_NESTED_ASSERT(mMeshPtr->faceCount() == 4);

        FacePtr f1 = mMeshPtr->faceBegin();

        VertexPtr v1;
        VertexPtr v3;
        EdgePtr e1;
        EdgePtr e3;
        EdgePtr e7;
        UNITTEST_NESTED_ASSERT(f1->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT(f1->adjacentEdgeCount() == 3);
        if (f1->adjacentVertex(0) == v5) {
            v3 = f1->adjacentVertex(1);
            v1 = f1->adjacentVertex(2);
            e7 = f1->adjacentEdge(0);
            e3 = f1->adjacentEdge(1);
            e1 = f1->adjacentEdge(2);
        } else if (f1->adjacentVertex(1) == v5) {
            v3 = f1->adjacentVertex(2);
            v1 = f1->adjacentVertex(0);
            e7 = f1->adjacentEdge(1);
            e3 = f1->adjacentEdge(2);
            e1 = f1->adjacentEdge(0);
        } else {
            UNITTEST_NESTED_ASSERT(f1->adjacentVertex(2) == v5);
            v3 = f1->adjacentVertex(0);
            v1 = f1->adjacentVertex(1);
            e7 = f1->adjacentEdge(2);
            e3 = f1->adjacentEdge(0);
            e1 = f1->adjacentEdge(1);
        }

        FacePtr f2;
        UNITTEST_NESTED_ASSERT(e1->adjacentFaceCount() == 2);
        if (e1->adjacentFace(0) == f1) {
            f2 = e1->adjacentFace(1);
        } else {
            UNITTEST_NESTED_ASSERT(e1->adjacentFace(1) == f1);
            f2 = e1->adjacentFace(0);
        }

        VertexPtr v4;
        EdgePtr e4;
        EdgePtr e8;
        UNITTEST_NESTED_ASSERT(f2->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT(f2->adjacentEdgeCount() == 3);
        if (f2->adjacentVertex(0) == v5) {
            UNITTEST_NESTED_ASSERT(f2->adjacentEdge(0) == e1);
            v4 = f2->adjacentVertex(2);
            e4 = f2->adjacentEdge(1);
            e8 = f2->adjacentEdge(2);
        } else if (f2->adjacentVertex(1) == v5) {
            UNITTEST_NESTED_ASSERT(f2->adjacentEdge(1) == e1);
            v4 = f2->adjacentVertex(0);
            e4 = f2->adjacentEdge(2);
            e8 = f2->adjacentEdge(0);
        } else {
            UNITTEST_NESTED_ASSERT(f2->adjacentVertex(2) == v5);
            UNITTEST_NESTED_ASSERT(f2->adjacentEdge(2) == e1);
            v4 = f2->adjacentVertex(1);
            e4 = f2->adjacentEdge(0);
            e8 = f2->adjacentEdge(1);
        }

        FacePtr f4;
        UNITTEST_NESTED_ASSERT(e8->adjacentFaceCount() == 2);
        if (e8->adjacentFace(0) == f2) {
            f4 = e8->adjacentFace(1);
        } else {
            UNITTEST_NESTED_ASSERT(e8->adjacentFace(1) == f2);
            f4 = e8->adjacentFace(0);
        }

        VertexPtr v2;
        EdgePtr e5;
        EdgePtr e6;
        UNITTEST_NESTED_ASSERT(f4->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT(f4->adjacentEdgeCount() == 3);
        if (f4->adjacentVertex(0) == v5) {
            UNITTEST_NESTED_ASSERT(f4->adjacentEdge(0) == e8);
            v2 = f4->adjacentVertex(2);
            e5 = f4->adjacentEdge(1);
            e6 = f4->adjacentEdge(2);
        } else if (f4->adjacentVertex(1) == v5) {
            UNITTEST_NESTED_ASSERT(f4->adjacentEdge(1) == e8);
            v2 = f4->adjacentVertex(0);
            e5 = f4->adjacentEdge(2);
            e6 = f4->adjacentEdge(0);
        } else {
            UNITTEST_NESTED_ASSERT(f4->adjacentVertex(2) == v5);
            UNITTEST_NESTED_ASSERT(f4->adjacentEdge(2) == e8);
            v2 = f4->adjacentVertex(1);
            e5 = f4->adjacentEdge(0);
            e6 = f4->adjacentEdge(1);
        }

        FacePtr f3;
        UNITTEST_NESTED_ASSERT(e6->adjacentFaceCount() == 2);
        if (e6->adjacentFace(0) == f4) {
            f3 = e6->adjacentFace(1);
        } else {
            UNITTEST_NESTED_ASSERT(e6->adjacentFace(1) == f4);
            f3 = e6->adjacentFace(0);
        }

        EdgePtr e2;
        UNITTEST_NESTED_ASSERT(f3->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT(f4->adjacentVertexCount() == 3);
        if (f3->adjacentVertex(0) == v5) {
            UNITTEST_NESTED_ASSERT(f3->adjacentVertex(2) == v3);
            UNITTEST_NESTED_ASSERT(f3->adjacentEdge(0) == e6);
            e2 = f3->adjacentEdge(1);
            UNITTEST_NESTED_ASSERT(f3->adjacentEdge(2) == e7);
        } else if (f3->adjacentVertex(1) == v5) {
            UNITTEST_NESTED_ASSERT(f3->adjacentVertex(0) == v3);
            UNITTEST_NESTED_ASSERT(f3->adjacentEdge(1) == e6);
            e2 = f3->adjacentEdge(2);
            UNITTEST_NESTED_ASSERT(f3->adjacentEdge(0) == e7);
        } else {
            UNITTEST_NESTED_ASSERT(f3->adjacentVertex(1) == v3);
            UNITTEST_NESTED_ASSERT(f3->adjacentVertex(2) == v5);
            UNITTEST_NESTED_ASSERT(f3->adjacentEdge(2) == e6);
            e2 = f3->adjacentEdge(0);
            UNITTEST_NESTED_ASSERT(f3->adjacentEdge(1) == e7);
        }

        UNITTEST_NESTED_ASSERT((e2->adjacentVertex(0) == v2
                && e2->adjacentVertex(1) == v3)
            || (e2->adjacentVertex(1) == v2
                && e2->adjacentVertex(0) == v3));
        UNITTEST_NESTED_ASSERT((e3->adjacentVertex(0) == v1
                && e3->adjacentVertex(1) == v3)
            || (e3->adjacentVertex(1) == v1
                && e3->adjacentVertex(0) == v3));
        UNITTEST_NESTED_ASSERT((e4->adjacentVertex(0) == v1
                && e4->adjacentVertex(1) == v4)
            || (e4->adjacentVertex(1) == v1
                && e4->adjacentVertex(0) == v4));
        UNITTEST_NESTED_ASSERT((e5->adjacentVertex(0) == v2
                && e5->adjacentVertex(1) == v4)
            || (e5->adjacentVertex(1) == v2
                && e5->adjacentVertex(0) == v4));

        UNITTEST_NESTED_ASSERT((e1->adjacentVertex(0) == v1
                && e1->adjacentVertex(1) == v5)
            || (e1->adjacentVertex(1) == v1
                && e1->adjacentVertex(0) == v5));
        UNITTEST_NESTED_ASSERT((e8->adjacentVertex(0) == v4
                && e8->adjacentVertex(1) == v5)
            || (e8->adjacentVertex(1) == v4
                && e8->adjacentVertex(0) == v5));
        UNITTEST_NESTED_ASSERT((e6->adjacentVertex(0) == v2
                && e6->adjacentVertex(1) == v5)
            || (e6->adjacentVertex(1) == v2
                && e6->adjacentVertex(0) == v5));
        UNITTEST_NESTED_ASSERT((e7->adjacentVertex(0) == v3
                && e7->adjacentVertex(1) == v5)
            || (e7->adjacentVertex(1) == v3
                && e7->adjacentVertex(0) == v5));

        UNITTEST_NESTED_ASSERT(e2->adjacentFaceCount() == 1);
        UNITTEST_NESTED_ASSERT(e2->adjacentFace(0) == f3);
        UNITTEST_NESTED_ASSERT(e3->adjacentFaceCount() == 1);
        UNITTEST_NESTED_ASSERT(e3->adjacentFace(0) == f1);
        UNITTEST_NESTED_ASSERT(e4->adjacentFaceCount() == 1);
        UNITTEST_NESTED_ASSERT(e4->adjacentFace(0) == f2);
        UNITTEST_NESTED_ASSERT(e5->adjacentFaceCount() == 1);
        UNITTEST_NESTED_ASSERT(e5->adjacentFace(0) == f4);

        UNITTEST_NESTED_ASSERT(e1->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT((e1->adjacentFace(0) == f1
                && e1->adjacentFace(1) == f2)
            || (e1->adjacentFace(1) == f1
                && e1->adjacentFace(0) == f2));
        UNITTEST_NESTED_ASSERT(e8->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT((e8->adjacentFace(0) == f2
                && e8->adjacentFace(1) == f4)
            || (e8->adjacentFace(1) == f2
                && e8->adjacentFace(0) == f4));
        UNITTEST_NESTED_ASSERT(e6->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT((e6->adjacentFace(0) == f4
                && e6->adjacentFace(1) == f3)
            || (e6->adjacentFace(1) == f4
                && e6->adjacentFace(0) == f3));
        UNITTEST_NESTED_ASSERT(e7->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT((e7->adjacentFace(0) == f3
                && e7->adjacentFace(1) == f1)
            || (e7->adjacentFace(1) == f3
                && e7->adjacentFace(0) == f1));

        UNITTEST_NESTED_ASSERT(f1->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT((f1->adjacentVertex(0) == v3
                && f1->adjacentVertex(1) == v1
                && f1->adjacentVertex(2) == v5)
            || (f1->adjacentVertex(0) == v1
                && f1->adjacentVertex(1) == v5
                && f1->adjacentVertex(2) == v3)
            || (f1->adjacentVertex(0) == v5
                && f1->adjacentVertex(1) == v3
                && f1->adjacentVertex(2) == v1));
        UNITTEST_NESTED_ASSERT(f2->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT((f2->adjacentVertex(0) == v1
                && f2->adjacentVertex(1) == v4
                && f2->adjacentVertex(2) == v5)
            || (f2->adjacentVertex(0) == v4
                && f2->adjacentVertex(1) == v5
                && f2->adjacentVertex(2) == v1)
            || (f2->adjacentVertex(0) == v5
                && f2->adjacentVertex(1) == v1
                && f2->adjacentVertex(2) == v4));
        UNITTEST_NESTED_ASSERT(f4->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT((f4->adjacentVertex(0) == v4
                && f4->adjacentVertex(1) == v2
                && f4->adjacentVertex(2) == v5)
            || (f4->adjacentVertex(0) == v2
                && f4->adjacentVertex(1) == v5
                && f4->adjacentVertex(2) == v4)
            || (f4->adjacentVertex(0) == v5
                && f4->adjacentVertex(1) == v4
                && f4->adjacentVertex(2) == v2));
        UNITTEST_NESTED_ASSERT(f3->adjacentVertexCount() == 3);
        UNITTEST_NESTED_ASSERT((f3->adjacentVertex(0) == v2
                && f3->adjacentVertex(1) == v3
                && f3->adjacentVertex(2) == v5)
            || (f3->adjacentVertex(0) == v3
                && f3->adjacentVertex(1) == v5
                && f3->adjacentVertex(2) == v2)
            || (f3->adjacentVertex(0) == v5
                && f3->adjacentVertex(1) == v2
                && f3->adjacentVertex(2) == v3));

        UNITTEST_NESTED_ASSERT(f1->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT((f1->adjacentEdge(0) == e3
                && f1->adjacentEdge(1) == e1
                && f1->adjacentEdge(2) == e7)
            || (f1->adjacentEdge(0) == e1
                && f1->adjacentEdge(1) == e7
                && f1->adjacentEdge(2) == e3)
            || (f1->adjacentEdge(0) == e7
                && f1->adjacentEdge(1) == e3
                && f1->adjacentEdge(2) == e1));
        UNITTEST_NESTED_ASSERT(f2->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT((f2->adjacentEdge(0) == e4
                && f2->adjacentEdge(1) == e8
                && f2->adjacentEdge(2) == e1)
            || (f2->adjacentEdge(0) == e8
                && f2->adjacentEdge(1) == e1
                && f2->adjacentEdge(2) == e4)
            || (f2->adjacentEdge(0) == e1
                && f2->adjacentEdge(1) == e4
                && f2->adjacentEdge(2) == e8));
        UNITTEST_NESTED_ASSERT(f4->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT((f4->adjacentEdge(0) == e5
                && f4->adjacentEdge(1) == e6
                && f4->adjacentEdge(2) == e8)
            || (f4->adjacentEdge(0) == e6
                && f4->adjacentEdge(1) == e8
                && f4->adjacentEdge(2) == e5)
            || (f4->adjacentEdge(0) == e8
                && f4->adjacentEdge(1) == e5
                && f4->adjacentEdge(2) == e6));
        UNITTEST_NESTED_ASSERT(f3->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT((f3->adjacentEdge(0) == e2
                && f3->adjacentEdge(1) == e7
                && f3->adjacentEdge(2) == e6)
            || (f3->adjacentEdge(0) == e7
                && f3->adjacentEdge(1) == e6
                && f3->adjacentEdge(2) == e2)
            || (f3->adjacentEdge(0) == e6
                && f3->adjacentEdge(1) == e2
                && f3->adjacentEdge(2) == e7));

        UNITTEST_NESTED_ASSERT(v1->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(v1->adjacentEdge(0) == e1
            || v1->adjacentEdge(0) == e3
            || v1->adjacentEdge(0) == e4);
        UNITTEST_NESTED_ASSERT(v1->adjacentEdge(1) == e1
            || v1->adjacentEdge(1) == e3
            || v1->adjacentEdge(1) == e4);
        UNITTEST_NESTED_ASSERT(v1->adjacentEdge(2) == e1
            || v1->adjacentEdge(2) == e3
            || v1->adjacentEdge(2) == e4);

        UNITTEST_NESTED_ASSERT(v2->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(v2->adjacentEdge(0) == e2
            || v2->adjacentEdge(0) == e5
            || v2->adjacentEdge(0) == e6);
        UNITTEST_NESTED_ASSERT(v2->adjacentEdge(1) == e2
            || v2->adjacentEdge(1) == e5
            || v2->adjacentEdge(1) == e6);
        UNITTEST_NESTED_ASSERT(v2->adjacentEdge(2) == e2
            || v2->adjacentEdge(2) == e5
            || v2->adjacentEdge(2) == e6);

        UNITTEST_NESTED_ASSERT(v3->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(v3->adjacentEdge(0) == e2
            || v3->adjacentEdge(0) == e3
            || v3->adjacentEdge(0) == e7);
        UNITTEST_NESTED_ASSERT(v3->adjacentEdge(1) == e2
            || v3->adjacentEdge(1) == e3
            || v3->adjacentEdge(1) == e7);
        UNITTEST_NESTED_ASSERT(v3->adjacentEdge(2) == e2
            || v3->adjacentEdge(2) == e3
            || v3->adjacentEdge(2) == e7);

        UNITTEST_NESTED_ASSERT(v4->adjacentEdgeCount() == 3);
        UNITTEST_NESTED_ASSERT(v4->adjacentEdge(0) == e4
            || v4->adjacentEdge(0) == e5
            || v4->adjacentEdge(0) == e8);
        UNITTEST_NESTED_ASSERT(v4->adjacentEdge(1) == e4
            || v4->adjacentEdge(1) == e5
            || v4->adjacentEdge(1) == e8);
        UNITTEST_NESTED_ASSERT(v4->adjacentEdge(2) == e4
            || v4->adjacentEdge(2) == e5
            || v4->adjacentEdge(2) == e8);

        UNITTEST_NESTED_ASSERT(v5->adjacentEdgeCount() == 4);
        UNITTEST_NESTED_ASSERT(v5->adjacentEdge(0) == e1
            || v5->adjacentEdge(0) == e6
            || v5->adjacentEdge(0) == e7
            || v5->adjacentEdge(0) == e8);
        UNITTEST_NESTED_ASSERT(v5->adjacentEdge(1) == e1
            || v5->adjacentEdge(1) == e6
            || v5->adjacentEdge(1) == e7
            || v5->adjacentEdge(1) == e8);
        UNITTEST_NESTED_ASSERT(v5->adjacentEdge(2) == e1
            || v5->adjacentEdge(2) == e6
            || v5->adjacentEdge(2) == e7
            || v5->adjacentEdge(2) == e8);

        UNITTEST_NESTED_ASSERT(v1->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(v1->adjacentFace(0) == f1
            || v1->adjacentFace(0) == f2);
        UNITTEST_NESTED_ASSERT(v1->adjacentFace(1) == f1
            || v1->adjacentFace(1) == f2);

        UNITTEST_NESTED_ASSERT(v2->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(v2->adjacentFace(0) == f3
            || v2->adjacentFace(0) == f4);
        UNITTEST_NESTED_ASSERT(v2->adjacentFace(1) == f3
            || v2->adjacentFace(1) == f4);

        UNITTEST_NESTED_ASSERT(v3->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(v3->adjacentFace(0) == f1
            || v3->adjacentFace(0) == f3);
        UNITTEST_NESTED_ASSERT(v3->adjacentFace(1) == f1
            || v3->adjacentFace(1) == f3);

        UNITTEST_NESTED_ASSERT(v4->adjacentFaceCount() == 2);
        UNITTEST_NESTED_ASSERT(v4->adjacentFace(0) == f2
            || v4->adjacentFace(0) == f4);
        UNITTEST_NESTED_ASSERT(v4->adjacentFace(1) == f2
            || v4->adjacentFace(1) == f4);

        UNITTEST_NESTED_ASSERT(v5->adjacentFaceCount() == 4);
        UNITTEST_NESTED_ASSERT(v5->adjacentFace(0) == f1
            || v5->adjacentFace(0) == f2
            || v5->adjacentFace(0) == f3
            || v5->adjacentFace(0) == f4);
        UNITTEST_NESTED_ASSERT(v5->adjacentFace(1) == f1
            || v5->adjacentFace(1) == f2
            || v5->adjacentFace(1) == f3
            || v5->adjacentFace(1) == f4);
        UNITTEST_NESTED_ASSERT(v5->adjacentFace(2) == f1
            || v5->adjacentFace(2) == f2
            || v5->adjacentFace(2) == f3
            || v5->adjacentFace(2) == f4);
        UNITTEST_NESTED_ASSERT(v5->adjacentFace(3) == f1
            || v5->adjacentFace(3) == f2
            || v5->adjacentFace(3) == f3
            || v5->adjacentFace(3) == f4);

        return true;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(EdgeOperationsTest);
