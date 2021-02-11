// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector2f.h>
#include <delaunay/Mesh.h>
#include <delaunay/Vertex.h>
#include <delaunay/Edge.h>
#include <delaunay/Face.h>
#include <delaunay/MeshConsistency.h>

using cgmath::Vector2f;
using delaunay::Mesh;
using delaunay::MeshPtr;
using delaunay::Vertex;
using delaunay::Edge;
using delaunay::Face;
using delaunay::VertexPtr;
using delaunay::EdgePtr;
using delaunay::FacePtr;

class MeshTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MeshTest);
    CPPUNIT_TEST(testCreateVertex);
    CPPUNIT_TEST(testDestroyVertex);
    CPPUNIT_TEST(testVertexBegin);
    CPPUNIT_TEST(testCreateEdge);
    CPPUNIT_TEST(testDestroyEdge);
    CPPUNIT_TEST(testEdgeBegin);
    CPPUNIT_TEST(testCreateFace);
    CPPUNIT_TEST(testDestroyFace);
    CPPUNIT_TEST(testFaceBegin);
    CPPUNIT_TEST(testClone);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCreateVertex() {
        Mesh mesh;
        CPPUNIT_ASSERT(mesh.vertexCount() == 0);

        mesh.createVertex();
        CPPUNIT_ASSERT(mesh.vertexCount() == 1);

        mesh.createVertex();
        CPPUNIT_ASSERT(mesh.vertexCount() == 2);

        mesh.createVertex();
        CPPUNIT_ASSERT(mesh.vertexCount() == 3);
    }

    void testDestroyVertex() {
        Mesh mesh;
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        CPPUNIT_ASSERT(mesh.vertexCount() == 3);

        mesh.destroyVertex(v1);
        CPPUNIT_ASSERT(mesh.vertexCount() == 2);

        mesh.destroyVertex(v2);
        CPPUNIT_ASSERT(mesh.vertexCount() == 1);

        mesh.destroyVertex(v3);
        CPPUNIT_ASSERT(mesh.vertexCount() == 0);
    }

    void testVertexBegin() {
        Mesh mesh;
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        VertexPtr v3 = mesh.createVertex();
        CPPUNIT_ASSERT(mesh.vertexCount() == 3);

        VertexPtr vertexPtr = mesh.vertexBegin();
        ++vertexPtr;
        ++vertexPtr;
        ++vertexPtr;
        CPPUNIT_ASSERT(vertexPtr == mesh.vertexEnd());
    }

    void testCreateEdge() {
        Mesh mesh;
        CPPUNIT_ASSERT(mesh.edgeCount() == 0);

        mesh.createEdge();
        CPPUNIT_ASSERT(mesh.edgeCount() == 1);

        mesh.createEdge();
        CPPUNIT_ASSERT(mesh.edgeCount() == 2);

        mesh.createEdge();
        CPPUNIT_ASSERT(mesh.edgeCount() == 3);
    }

    void testDestroyEdge() {
        Mesh mesh;
        EdgePtr v1 = mesh.createEdge();
        EdgePtr v2 = mesh.createEdge();
        EdgePtr v3 = mesh.createEdge();
        CPPUNIT_ASSERT(mesh.edgeCount() == 3);

        mesh.destroyEdge(v1);
        CPPUNIT_ASSERT(mesh.edgeCount() == 2);

        mesh.destroyEdge(v2);
        CPPUNIT_ASSERT(mesh.edgeCount() == 1);

        mesh.destroyEdge(v3);
        CPPUNIT_ASSERT(mesh.edgeCount() == 0);
    }

    void testEdgeBegin() {
        Mesh mesh;
        EdgePtr v1 = mesh.createEdge();
        EdgePtr v2 = mesh.createEdge();
        EdgePtr v3 = mesh.createEdge();
        CPPUNIT_ASSERT(mesh.edgeCount() == 3);

        EdgePtr edgePtr = mesh.edgeBegin();
        ++edgePtr;
        ++edgePtr;
        ++edgePtr;
        CPPUNIT_ASSERT(edgePtr == mesh.edgeEnd());
    }

    void testCreateFace() {
        Mesh mesh;
        CPPUNIT_ASSERT(mesh.faceCount() == 0);

        mesh.createFace();
        CPPUNIT_ASSERT(mesh.faceCount() == 1);

        mesh.createFace();
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh.createFace();
        CPPUNIT_ASSERT(mesh.faceCount() == 3);
    }

    void testDestroyFace() {
        Mesh mesh;
        FacePtr v1 = mesh.createFace();
        FacePtr v2 = mesh.createFace();
        FacePtr v3 = mesh.createFace();
        CPPUNIT_ASSERT(mesh.faceCount() == 3);

        mesh.destroyFace(v1);
        CPPUNIT_ASSERT(mesh.faceCount() == 2);

        mesh.destroyFace(v2);
        CPPUNIT_ASSERT(mesh.faceCount() == 1);

        mesh.destroyFace(v3);
        CPPUNIT_ASSERT(mesh.faceCount() == 0);
    }

    void testFaceBegin() {
        Mesh mesh;
        FacePtr v1 = mesh.createFace();
        FacePtr v2 = mesh.createFace();
        FacePtr v3 = mesh.createFace();
        CPPUNIT_ASSERT(mesh.faceCount() == 3);

        FacePtr facePtr = mesh.faceBegin();
        ++facePtr;
        ++facePtr;
        ++facePtr;
        CPPUNIT_ASSERT(facePtr == mesh.faceEnd());
    }

    void testClone() {
        MeshPtr meshPtr(new Mesh);

        VertexPtr v1 = meshPtr->createVertex();
        VertexPtr v2 = meshPtr->createVertex();
        VertexPtr v3 = meshPtr->createVertex();

        EdgePtr e1 = meshPtr->createEdge();
        EdgePtr e2 = meshPtr->createEdge();
        EdgePtr e3 = meshPtr->createEdge();

        FacePtr f1 = meshPtr->createFace();

        v1->setPosition(Vector2f(0, 0));
        v2->setPosition(Vector2f(10, 0));
        v3->setPosition(Vector2f(0, 10));

        v1->addAdjacentEdge(e1);
        v1->addAdjacentEdge(e3);

        v2->addAdjacentEdge(e1);
        v2->addAdjacentEdge(e2);

        v3->addAdjacentEdge(e2);
        v3->addAdjacentEdge(e3);

        v1->addAdjacentFace(f1);
        v2->addAdjacentFace(f1);
        v3->addAdjacentFace(f1);

        e1->addAdjacentVertex(v1);
        e1->addAdjacentVertex(v2);
        e1->addAdjacentFace(f1);

        e2->addAdjacentVertex(v2);
        e2->addAdjacentVertex(v3);
        e2->addAdjacentFace(f1);

        e3->addAdjacentVertex(v3);
        e3->addAdjacentVertex(v1);
        e3->addAdjacentFace(f1);

        f1->addAdjacentVertex(v1);
        f1->addAdjacentVertex(v2);
        f1->addAdjacentVertex(v3);
        f1->addAdjacentEdge(e1);
        f1->addAdjacentEdge(e2);
        f1->addAdjacentEdge(e3);

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(meshPtr).isConsistent());

        MeshPtr clonedMesh(meshPtr->clone());

        CPPUNIT_ASSERT(delaunay::TestMeshConsistency(clonedMesh).isConsistent());

        CPPUNIT_ASSERT(clonedMesh->vertexCount() == 3);
        CPPUNIT_ASSERT(clonedMesh->edgeCount() == 3);
        CPPUNIT_ASSERT(clonedMesh->faceCount() == 1);

        v1 = clonedMesh->vertexBegin();
        v2 = v1;
        ++v2;
        v3 = v2;
        ++v3;

        CPPUNIT_ASSERT(v1->position() == Vector2f(0, 0));
        CPPUNIT_ASSERT(v2->position() == Vector2f(10, 0));
        CPPUNIT_ASSERT(v3->position() == Vector2f(0, 10));

        e1 = clonedMesh->edgeBegin();
        e2 = e1;
        ++e2;
        e3 = e2;
        ++e3;
        
        f1 = clonedMesh->faceBegin();

        CPPUNIT_ASSERT(e1->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(e1->adjacentVertex(1) == v2);
        CPPUNIT_ASSERT(e1->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(e1->adjacentFace(0) == f1);

        CPPUNIT_ASSERT(e2->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e2->adjacentVertex(0) == v2);
        CPPUNIT_ASSERT(e2->adjacentVertex(1) == v3);
        CPPUNIT_ASSERT(e2->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(e2->adjacentFace(0) == f1);

        CPPUNIT_ASSERT(e3->adjacentVertexCount() == 2);
        CPPUNIT_ASSERT(e3->adjacentVertex(0) == v3);
        CPPUNIT_ASSERT(e3->adjacentVertex(1) == v1);
        CPPUNIT_ASSERT(e3->adjacentFaceCount() == 1);
        CPPUNIT_ASSERT(e3->adjacentFace(0) == f1);

        CPPUNIT_ASSERT(f1->adjacentVertexCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentVertex(0) == v1);
        CPPUNIT_ASSERT(f1->adjacentVertex(1) == v2);
        CPPUNIT_ASSERT(f1->adjacentVertex(2) == v3);
        CPPUNIT_ASSERT(f1->adjacentEdgeCount() == 3);
        CPPUNIT_ASSERT(f1->adjacentEdge(0) == e1);
        CPPUNIT_ASSERT(f1->adjacentEdge(1) == e2);
        CPPUNIT_ASSERT(f1->adjacentEdge(2) == e3);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(MeshTest);
