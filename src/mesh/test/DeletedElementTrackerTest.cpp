// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <mesh/DeletedElementTracker.h>
#include <mesh/Mesh.h>
#include <mesh/Types.h>

using mesh::DeletedElementTracker;
using mesh::Mesh;
using mesh::VertexPtr;
using mesh::EdgePtr;
using mesh::FacePtr;

class ElementTrackerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ElementTrackerTest);
    CPPUNIT_TEST(testElementMask);
    CPPUNIT_TEST(testAddElements);
    CPPUNIT_TEST(testIterators);
    CPPUNIT_TEST(testClear);
    CPPUNIT_TEST(testMaskedAddElements);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testElementMask() {
        DeletedElementTracker deletedElementTracker;

        // Test the default value, which should allow all mesh element types.
        CPPUNIT_ASSERT(deletedElementTracker.elementMask() 
            == DeletedElementTracker::VERTICES | DeletedElementTracker::EDGES 
            | DeletedElementTracker::FACES);

        deletedElementTracker.setElementMask(DeletedElementTracker::VERTICES);
        CPPUNIT_ASSERT(deletedElementTracker.elementMask() == DeletedElementTracker::VERTICES);

        deletedElementTracker.setElementMask(DeletedElementTracker::EDGES);
        CPPUNIT_ASSERT(deletedElementTracker.elementMask() == DeletedElementTracker::EDGES);

        deletedElementTracker.setElementMask(DeletedElementTracker::FACES);
        CPPUNIT_ASSERT(deletedElementTracker.elementMask() == DeletedElementTracker::FACES);

        deletedElementTracker.setElementMask(DeletedElementTracker::VERTICES 
            | DeletedElementTracker::FACES);
        CPPUNIT_ASSERT(deletedElementTracker.elementMask() 
            == DeletedElementTracker::VERTICES | DeletedElementTracker::FACES);

        CPPUNIT_ASSERT(DeletedElementTracker::NONE == 0);
        CPPUNIT_ASSERT(DeletedElementTracker::ALL
            == DeletedElementTracker::VERTICES | DeletedElementTracker::EDGES 
            | DeletedElementTracker::FACES);
    }

    void testAddElements() {
        DeletedElementTracker deletedElementTracker;

        Mesh mesh;
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        EdgePtr e1 = mesh.createEdge();
        EdgePtr e2 = mesh.createEdge();
        FacePtr f1 = mesh.createFace();
        FacePtr f2 = mesh.createFace();

        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v1));
        deletedElementTracker.addVertex(v1);
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v2));
        deletedElementTracker.addVertex(v2);
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v2));

        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e1));
        deletedElementTracker.addEdge(e1);
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e2));
        deletedElementTracker.addEdge(e2);
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e2));

        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f1));
        deletedElementTracker.addFace(f1);
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f2));
        deletedElementTracker.addFace(f2);
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f2));
    }

    void testIterators() {
        DeletedElementTracker deletedElementTracker;

        Mesh mesh;
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        EdgePtr e1 = mesh.createEdge();
        EdgePtr e2 = mesh.createEdge();
        FacePtr f1 = mesh.createFace();
        FacePtr f2 = mesh.createFace();

        int count;

        deletedElementTracker.addVertex(v1);
        deletedElementTracker.addVertex(v2);
        count = 0;
        for (DeletedElementTracker::VertexIterator iterator = deletedElementTracker.vertexBegin();
             iterator != deletedElementTracker.vertexEnd(); ++iterator) {
            CPPUNIT_ASSERT(*iterator == v1 || *iterator == v2);
            ++count;
        }
        CPPUNIT_ASSERT(count == 2);

        deletedElementTracker.addEdge(e1);
        deletedElementTracker.addEdge(e2);
        count = 0;
        for (DeletedElementTracker::EdgeIterator iterator = deletedElementTracker.edgeBegin();
             iterator != deletedElementTracker.edgeEnd(); ++iterator) {
            CPPUNIT_ASSERT(*iterator == e1 || *iterator == e2);
            ++count;
        }
        CPPUNIT_ASSERT(count == 2);

        deletedElementTracker.addFace(f1);
        deletedElementTracker.addFace(f2);
        count = 0;
        for (DeletedElementTracker::FaceIterator iterator = deletedElementTracker.faceBegin();
             iterator != deletedElementTracker.faceEnd(); ++iterator) {
            CPPUNIT_ASSERT(*iterator == f1 || *iterator == f2);
            ++count;
        }
        CPPUNIT_ASSERT(count == 2);
    }

    void testClear() {
        DeletedElementTracker deletedElementTracker;

        Mesh mesh;
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        EdgePtr e1 = mesh.createEdge();
        EdgePtr e2 = mesh.createEdge();
        FacePtr f1 = mesh.createFace();
        FacePtr f2 = mesh.createFace();

        deletedElementTracker.addVertex(v1);
        deletedElementTracker.addVertex(v2);
        deletedElementTracker.addEdge(e1);
        deletedElementTracker.addEdge(e2);
        deletedElementTracker.addFace(f1);
        deletedElementTracker.addFace(f2);

        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v2));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e2));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f2));

        deletedElementTracker.clear();

        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f2));
    }

    void testMaskedAddElements() {
        DeletedElementTracker deletedElementTracker;

        Mesh mesh;
        VertexPtr v1 = mesh.createVertex();
        VertexPtr v2 = mesh.createVertex();
        EdgePtr e1 = mesh.createEdge();
        EdgePtr e2 = mesh.createEdge();
        FacePtr f1 = mesh.createFace();
        FacePtr f2 = mesh.createFace();

        deletedElementTracker.clear();
        deletedElementTracker.setElementMask(DeletedElementTracker::VERTICES);
        deletedElementTracker.addVertex(v1);
        deletedElementTracker.addVertex(v2);
        deletedElementTracker.addEdge(e1);
        deletedElementTracker.addEdge(e2);
        deletedElementTracker.addFace(f1);
        deletedElementTracker.addFace(f2);
        deletedElementTracker.setElementMask(
            DeletedElementTracker::VERTICES | DeletedElementTracker::EDGES 
            | DeletedElementTracker::FACES);
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f2));

        deletedElementTracker.clear();
        deletedElementTracker.setElementMask(DeletedElementTracker::EDGES);
        deletedElementTracker.addVertex(v1);
        deletedElementTracker.addVertex(v2);
        deletedElementTracker.addEdge(e1);
        deletedElementTracker.addEdge(e2);
        deletedElementTracker.addFace(f1);
        deletedElementTracker.addFace(f2);
        deletedElementTracker.setElementMask(
            DeletedElementTracker::VERTICES | DeletedElementTracker::EDGES 
            | DeletedElementTracker::FACES);
        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v2));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(deletedElementTracker.hasEdge(e2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasFace(f2));

        deletedElementTracker.clear();
        deletedElementTracker.setElementMask(DeletedElementTracker::FACES);
        deletedElementTracker.addVertex(v1);
        deletedElementTracker.addVertex(v2);
        deletedElementTracker.addEdge(e1);
        deletedElementTracker.addEdge(e2);
        deletedElementTracker.addFace(f1);
        deletedElementTracker.addFace(f2);
        deletedElementTracker.setElementMask(
            DeletedElementTracker::VERTICES | DeletedElementTracker::EDGES 
            | DeletedElementTracker::FACES);
        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasVertex(v2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e2));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f2));

        deletedElementTracker.clear();
        deletedElementTracker.setElementMask(DeletedElementTracker::VERTICES 
            | DeletedElementTracker::FACES);
        deletedElementTracker.addVertex(v1);
        deletedElementTracker.addVertex(v2);
        deletedElementTracker.addEdge(e1);
        deletedElementTracker.addEdge(e2);
        deletedElementTracker.addFace(f1);
        deletedElementTracker.addFace(f2);
        deletedElementTracker.setElementMask(
            DeletedElementTracker::VERTICES | DeletedElementTracker::EDGES 
            | DeletedElementTracker::FACES);
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v1));
        CPPUNIT_ASSERT(deletedElementTracker.hasVertex(v2));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e1));
        CPPUNIT_ASSERT(!deletedElementTracker.hasEdge(e2));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f1));
        CPPUNIT_ASSERT(deletedElementTracker.hasFace(f2));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ElementTrackerTest);
