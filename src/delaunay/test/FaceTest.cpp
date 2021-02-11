// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <delaunay/Mesh.h>
#include <delaunay/Vertex.h>
#include <delaunay/Edge.h>
#include <delaunay/Face.h>

using delaunay::Mesh;
using delaunay::MeshPtr;
using delaunay::VertexPtr;
using delaunay::EdgePtr;
using delaunay::FacePtr;

class FaceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(FaceTest);
    CPPUNIT_TEST(testClone);
    CPPUNIT_TEST(testAddVertex);
    CPPUNIT_TEST(testRemoveVertex);
    CPPUNIT_TEST(testRemoveVertexReversed);
    CPPUNIT_TEST(testAddEdge);
    CPPUNIT_TEST(testRemoveEdge);
    CPPUNIT_TEST(testRemoveEdgeReversed);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testClone() {
        MeshPtr meshPtr(new Mesh);
        FacePtr f1 = meshPtr->createFace();
        FacePtr f2 = f1->clone(meshPtr);
        CPPUNIT_ASSERT(f2->adjacentVertexCount() == 0);
        CPPUNIT_ASSERT(f2->adjacentEdgeCount() == 0);
    }

    void testAddVertex() {
        MeshPtr meshPtr(new Mesh);

        FacePtr f1 = meshPtr->createFace();
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 0);

        VertexPtr v1 = meshPtr->createVertex();
        f1->addAdjacentVertex(v1);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 1);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);

        VertexPtr v2 = meshPtr->createVertex();
        f1->addAdjacentVertex(v2);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v2);

        VertexPtr v3 = meshPtr->createVertex();
        f1->addAdjacentVertex(v3);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v2);
        CPPUNIT_ASSERT(f1->adjacentVertex(2) == v3);
    }

    void testRemoveVertex() {
        MeshPtr meshPtr(new Mesh);

        FacePtr f1 = meshPtr->createFace();
        VertexPtr v1 = meshPtr->createVertex();
        f1->addAdjacentVertex(v1);
        VertexPtr v2 = meshPtr->createVertex();
        f1->addAdjacentVertex(v2);
        VertexPtr v3 = meshPtr->createVertex();
        f1->addAdjacentVertex(v3);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v2);
        CPPUNIT_ASSERT(f1->adjacentVertex(2) == v3);

        f1->removeAdjacentVertex(v1);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v2);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v3);

        f1->removeAdjacentVertex(v2);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 1);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v3);

        f1->removeAdjacentVertex(v3);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 0);
    }

    void testRemoveVertexReversed() {
        MeshPtr meshPtr(new Mesh);

        FacePtr f1 = meshPtr->createFace();
        VertexPtr v1 = meshPtr->createVertex();
        f1->addAdjacentVertex(v1);
        VertexPtr v2 = meshPtr->createVertex();
        f1->addAdjacentVertex(v2);
        VertexPtr v3 = meshPtr->createVertex();
        f1->addAdjacentVertex(v3);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v2);
        CPPUNIT_ASSERT(f1->adjacentVertex(2) == v3);

        f1->removeAdjacentVertex(v3);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v2);

        f1->removeAdjacentVertex(v2);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 1);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);

        f1->removeAdjacentVertex(v1);
        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 0);
    }

    void testAddEdge() {
        MeshPtr meshPtr(new Mesh);

        FacePtr f1 = meshPtr->createFace();
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 0);

        EdgePtr e1 = meshPtr->createEdge();
        f1->addAdjacentEdge(e1);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 1);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);

        EdgePtr e2 = meshPtr->createEdge();
        f1->addAdjacentEdge(e2);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e2);

        EdgePtr e3 = meshPtr->createEdge();
        f1->addAdjacentEdge(e3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(f1->adjacentEdge(2) == e3);
    }

    void testRemoveEdge() {
        MeshPtr meshPtr(new Mesh);

        FacePtr f1 = meshPtr->createFace();
        EdgePtr e1 = meshPtr->createEdge();
        f1->addAdjacentEdge(e1);
        EdgePtr e2 = meshPtr->createEdge();
        f1->addAdjacentEdge(e2);
        EdgePtr e3 = meshPtr->createEdge();
        f1->addAdjacentEdge(e3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(f1->adjacentEdge(2) == e3);

        f1->removeAdjacentEdge(e1);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e2);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e3);

        f1->removeAdjacentEdge(e2);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 1);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e3);

        f1->removeAdjacentEdge(e3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 0);
    }

    void testRemoveEdgeReversed() {
        MeshPtr meshPtr(new Mesh);

        FacePtr f1 = meshPtr->createFace();
        EdgePtr e1 = meshPtr->createEdge();
        f1->addAdjacentEdge(e1);
        EdgePtr e2 = meshPtr->createEdge();
        f1->addAdjacentEdge(e2);
        EdgePtr e3 = meshPtr->createEdge();
        f1->addAdjacentEdge(e3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(f1->adjacentEdge(2) == e3);

        f1->removeAdjacentEdge(e3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 2);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e2);

        f1->removeAdjacentEdge(e2);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 1);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);

        f1->removeAdjacentEdge(e1);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(FaceTest);
