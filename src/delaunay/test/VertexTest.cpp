// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <delaunay/Vertex.h>
#include <delaunay/Mesh.h>

using delaunay::Vertex;
using delaunay::VertexPtr;
using delaunay::Edge;
using delaunay::EdgePtr;
using delaunay::Face;
using delaunay::FacePtr;
using delaunay::Mesh;
using delaunay::MeshPtr;
using cgmath::Vector2f;

class VertexTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(VertexTest);
    CPPUNIT_TEST(testClone);
    CPPUNIT_TEST(testPosition);
    CPPUNIT_TEST(testAddEdge);
    CPPUNIT_TEST(testRemoveEdge);
    CPPUNIT_TEST(testRemoveEdgeReversed);
    CPPUNIT_TEST(testAddFace);
    CPPUNIT_TEST(testRemoveFace);
    CPPUNIT_TEST(testRemoveFaceReversed);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testClone() {
        MeshPtr meshPtr(new Mesh);
        VertexPtr v1 = meshPtr->createVertex();
        v1->setPosition(Vector2f(1, 2));
        VertexPtr v2 = v1->clone(meshPtr);
        CPPUNIT_ASSERT(v2->position() == Vector2f(1, 2));
    }

    void testPosition() {
        const Vector2f position(1, 2);
        Vertex vertex;
        vertex.setPosition(position);
        CPPUNIT_ASSERT(vertex.position() == position);
    }

    void testAddEdge() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 0);

        EdgePtr e1 = meshPtr->createEdge();
        v1->addAdjacentEdge(e1);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 1);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);

        EdgePtr e2 = meshPtr->createEdge();
        v1->addAdjacentEdge(e2);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(v1->adjacentEdge(1) == e2);

        EdgePtr e3 = meshPtr->createEdge();
        v1->addAdjacentEdge(e3);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(v1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(v1->adjacentEdge(2) == e3);
    }

    void testRemoveEdge() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 0);

        EdgePtr e1 = meshPtr->createEdge();
        v1->addAdjacentEdge(e1);
        EdgePtr e2 = meshPtr->createEdge();
        v1->addAdjacentEdge(e2);
        EdgePtr e3 = meshPtr->createEdge();
        v1->addAdjacentEdge(e3);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(v1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(v1->adjacentEdge(2) == e3);

        v1->removeAdjacentEdge(e1);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e2);
        CPPUNIT_ASSERT(v1->adjacentEdge(1) == e3);

        v1->removeAdjacentEdge(e2);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 1);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e3);

        v1->removeAdjacentEdge(e3);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 0);
    }

    void testRemoveEdgeReversed() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 0);

        EdgePtr e1 = meshPtr->createEdge();
        v1->addAdjacentEdge(e1);
        EdgePtr e2 = meshPtr->createEdge();
        v1->addAdjacentEdge(e2);
        EdgePtr e3 = meshPtr->createEdge();
        v1->addAdjacentEdge(e3);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(v1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(v1->adjacentEdge(2) == e3);

        v1->removeAdjacentEdge(e3);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(v1->adjacentEdge(1) == e2);

        v1->removeAdjacentEdge(e2);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 1);
        CPPUNIT_ASSERT(v1->adjacentEdge(0) == e1);

        v1->removeAdjacentEdge(e1);
        CPPUNIT_ASSERT(v1->adjacentEdgeCount() == 0);
    }

    void testAddFace() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 0);

        FacePtr f1 = meshPtr->createFace();
        v1->addAdjacentFace(f1);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);

        FacePtr f2 = meshPtr->createFace();
        v1->addAdjacentFace(f2);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(v1->adjacentFace(1) == f2);

        FacePtr f3 = meshPtr->createFace();
        v1->addAdjacentFace(f3);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 3);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(v1->adjacentFace(1) == f2);
        CPPUNIT_ASSERT(v1->adjacentFace(2) == f3);
    }

    void testRemoveFace() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 0);

        FacePtr f1 = meshPtr->createFace();
        v1->addAdjacentFace(f1);
        FacePtr f2 = meshPtr->createFace();
        v1->addAdjacentFace(f2);
        FacePtr f3 = meshPtr->createFace();
        v1->addAdjacentFace(f3);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 3);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(v1->adjacentFace(1) == f2);
        CPPUNIT_ASSERT(v1->adjacentFace(2) == f3);

        v1->removeAdjacentFace(f1);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f2);
        CPPUNIT_ASSERT(v1->adjacentFace(1) == f3);

        v1->removeAdjacentFace(f2);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f3);

        v1->removeAdjacentFace(f3);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 0);
    }

    void testRemoveFaceReversed() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 0);

        FacePtr f1 = meshPtr->createFace();
        v1->addAdjacentFace(f1);
        FacePtr f2 = meshPtr->createFace();
        v1->addAdjacentFace(f2);
        FacePtr f3 = meshPtr->createFace();
        v1->addAdjacentFace(f3);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 3);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(v1->adjacentFace(1) == f2);
        CPPUNIT_ASSERT(v1->adjacentFace(2) == f3);

        v1->removeAdjacentFace(f3);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(v1->adjacentFace(1) == f2);

        v1->removeAdjacentFace(f2);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(v1->adjacentFace(0) == f1);

        v1->removeAdjacentFace(f1);
        CPPUNIT_ASSERT(v1->adjacentFaceCount() == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VertexTest);
