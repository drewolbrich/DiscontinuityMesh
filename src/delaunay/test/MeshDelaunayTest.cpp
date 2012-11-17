// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <delaunay/MeshDelaunay.h>
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

class MeshDelaunayTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MeshDelaunayTest);
    CPPUNIT_TEST(testMeshIsDelaunay);
    CPPUNIT_TEST(testMeshIsNotDelaunay);
    CPPUNIT_TEST(testMeshIsNotDelaunayCoincident);
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

        mE5 = mMeshPtr->createEdge();
        mE1 = mMeshPtr->createEdge();
        mE4 = mMeshPtr->createEdge();
        mE2 = mMeshPtr->createEdge();
        mE3 = mMeshPtr->createEdge();

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

    void testMeshIsDelaunay() {
        // This mesh satisfies the Delaunay criterion.
        mV1->setPosition(Vector2f(0, 10));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(12, 12));
        mV4->setPosition(Vector2f(0, 0));

        CPPUNIT_ASSERT(delaunay::MeshIsDelaunay(mMeshPtr));
    }

    void testMeshIsNotDelaunay() {
        // This mesh does not satisfy the Delaunay criterion.
        mV1->setPosition(Vector2f(0, 10));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(6, 6));
        mV4->setPosition(Vector2f(0, 0));

        CPPUNIT_ASSERT(!delaunay::MeshIsDelaunay(mMeshPtr));
    }

    void testMeshIsNotDelaunayCoincident() {
        // This mesh satisfies the Delaunay criterion but contains
        // triangles whose bounding circles intersect
        // more than three vertices.
        mV1->setPosition(Vector2f(0, 10));
        mV2->setPosition(Vector2f(10, 0));
        mV3->setPosition(Vector2f(10, 10));
        mV4->setPosition(Vector2f(0, 0));

        CPPUNIT_ASSERT(delaunay::MeshIsDelaunay(mMeshPtr));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MeshDelaunayTest);
