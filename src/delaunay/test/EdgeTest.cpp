// Copyright 2008 Retroactive Fiasco.

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

class EdgeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EdgeTest);
    CPPUNIT_TEST(testClone);
    CPPUNIT_TEST(testAddVertex);
    CPPUNIT_TEST(testRemoveVertex);
    CPPUNIT_TEST(testRemoveVertexReversed);
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
        EdgePtr e1 = meshPtr->createEdge();
        EdgePtr e2 = e1->clone(meshPtr);
        CPPUNIT_ASSERT(e2->adjacentVertexCount() == 0);
        CPPUNIT_ASSERT(e2->adjacentFaceCount() == 0);
    }

    void testAddVertex() {
        MeshPtr meshPtr(new Mesh);

        EdgePtr e1 = meshPtr->createEdge();
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 0);

        VertexPtr v1 = meshPtr->createVertex();
        e1->addAdjacentVertex(v1);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v1);

        VertexPtr v2 = meshPtr->createVertex();
        e1->addAdjacentVertex(v2);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(e1->adjacentVertex(1) == v2);
    }

    void testRemoveVertex() {
        MeshPtr meshPtr(new Mesh);

        EdgePtr e1 = meshPtr->createEdge();
        VertexPtr v1 = meshPtr->createVertex();
        e1->addAdjacentVertex(v1);
        VertexPtr v2 = meshPtr->createVertex();
        e1->addAdjacentVertex(v2);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(e1->adjacentVertex(1) == v2);

        e1->removeAdjacentVertex(v1);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v2);

        e1->removeAdjacentVertex(v2);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 0);
    }

    void testRemoveVertexReversed() {
        MeshPtr meshPtr(new Mesh);

        EdgePtr e1 = meshPtr->createEdge();
        VertexPtr v1 = meshPtr->createVertex();
        e1->addAdjacentVertex(v1);
        VertexPtr v2 = meshPtr->createVertex();
        e1->addAdjacentVertex(v2);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(e1->adjacentVertex(1) == v2);

        e1->removeAdjacentVertex(v2);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v1);

        e1->removeAdjacentVertex(v1);
        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 0);
    }

    void testAddFace() {
        MeshPtr meshPtr(new Mesh);

        EdgePtr e1 = meshPtr->createEdge();
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 0);

        FacePtr f1 = meshPtr->createFace();
        e1->addAdjacentFace(f1);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f1);

        FacePtr f2 = meshPtr->createFace();
        e1->addAdjacentFace(f2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(e1->adjacentFace(1) == f2);
    }

    void testRemoveFace() {
        MeshPtr meshPtr(new Mesh);

        EdgePtr e1 = meshPtr->createEdge();
        FacePtr f1 = meshPtr->createFace();
        e1->addAdjacentFace(f1);
        FacePtr f2 = meshPtr->createFace();
        e1->addAdjacentFace(f2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(e1->adjacentFace(1) == f2);

        e1->removeAdjacentFace(f1);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f2);

        e1->removeAdjacentFace(f2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 0);
    }

    void testRemoveFaceReversed() {
        MeshPtr meshPtr(new Mesh);

        EdgePtr e1 = meshPtr->createEdge();
        FacePtr f1 = meshPtr->createFace();
        e1->addAdjacentFace(f1);
        FacePtr f2 = meshPtr->createFace();
        e1->addAdjacentFace(f2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f1);
        CPPUNIT_ASSERT(e1->adjacentFace(1) == f2);

        e1->removeAdjacentFace(f2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f1);

        e1->removeAdjacentFace(f1);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(EdgeTest);
