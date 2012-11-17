// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <meshretri/Retriangulator.h>
#include <meshretri/RetriangulatorFace.h>
#include <meshretri/RetriangulatorEdge.h>
#include <meshretri/FaceLineSegment.h>
#include <meshretri/EndpointIdentifier.h>
#include <mesh/Mesh.h>
#include <mesh/IsConsistent.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Tolerance.h>

using meshretri::Retriangulator;
using meshretri::RetriangulatorFace;
using meshretri::RetriangulatorEdge;
using meshretri::FaceLineSegment;
using meshretri::EndpointIdentifier;
using cgmath::Vector3f;

class RetriangulatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RetriangulatorTest);
    CPPUNIT_TEST(testSetMesh);
    CPPUNIT_TEST(testAddFaceLineSegmentToFace);
    CPPUNIT_TEST(testAddEdgePointToRetriangulatorEdge);
    CPPUNIT_TEST(testCreateEdgePoints);
    CPPUNIT_TEST(testRetriangulate);
    CPPUNIT_TEST(testRetriangulateWeldFaceLineSegmentEndpointsEqual);
    CPPUNIT_TEST(testRetriangulateWeldFaceLineSegmentEndpointsUnequal);
    CPPUNIT_TEST_SUITE_END();

public:
    mesh::Mesh *mMesh;
    mesh::FacePtr mFace;
    mesh::EdgePtr mEdge0;
    mesh::EdgePtr mEdge1;
    mesh::EdgePtr mEdge2;
    mesh::VertexPtr mVertex0;
    mesh::VertexPtr mVertex1;
    mesh::VertexPtr mVertex2;

    void setUp() {
        mMesh = new mesh::Mesh;

        mFace = mMesh->createFace();
        mEdge0 = mMesh->createEdge();
        mEdge1 = mMesh->createEdge();
        mEdge2 = mMesh->createEdge();
        mVertex0 = mMesh->createVertex();
        mVertex1 = mMesh->createVertex();
        mVertex2 = mMesh->createVertex();

        mFace->addAdjacentEdge(mEdge0);
        mFace->addAdjacentEdge(mEdge1);
        mFace->addAdjacentEdge(mEdge2);
        mFace->addAdjacentVertex(mVertex0);
        mFace->addAdjacentVertex(mVertex1);
        mFace->addAdjacentVertex(mVertex2);

        mEdge0->addAdjacentFace(mFace);
        mEdge1->addAdjacentFace(mFace);
        mEdge2->addAdjacentFace(mFace);
        mEdge0->addAdjacentVertex(mVertex0);
        mEdge0->addAdjacentVertex(mVertex1);
        mEdge1->addAdjacentVertex(mVertex1);
        mEdge1->addAdjacentVertex(mVertex2);
        mEdge2->addAdjacentVertex(mVertex2);
        mEdge2->addAdjacentVertex(mVertex0);

        mVertex0->addAdjacentFace(mFace);
        mVertex1->addAdjacentFace(mFace);
        mVertex2->addAdjacentFace(mFace);
        mVertex0->addAdjacentEdge(mEdge0);
        mVertex1->addAdjacentEdge(mEdge0);
        mVertex1->addAdjacentEdge(mEdge1);
        mVertex2->addAdjacentEdge(mEdge1);
        mVertex2->addAdjacentEdge(mEdge2);
        mVertex0->addAdjacentEdge(mEdge2);

        mVertex0->setPosition(Vector3f(0.0, 0.0, 0.0));
        mVertex1->setPosition(Vector3f(100.0, 0.0, 0.0));
        mVertex2->setPosition(Vector3f(0.0, 100.0, 0.0));

        CPPUNIT_ASSERT(mesh::IsConsistent(*mMesh));
    }

    void tearDown() {
        delete mMesh;
    }

    void testSetMesh() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        CPPUNIT_ASSERT(retriangulator.mesh() == mMesh);
    }

    void testAddFaceLineSegmentToFace() {
        Retriangulator retriangulator;

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 10.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 30.0, 0.0));

        retriangulator.setMesh(mMesh);

        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 1);

        const FaceLineSegment &resultFaceLineSegment
            = retriangulatorFace->faceLineSegment(0);
        CPPUNIT_ASSERT(resultFaceLineSegment.worldPosition(0) == Vector3f(10.0, 10.0, 0.0));
        CPPUNIT_ASSERT(resultFaceLineSegment.worldPosition(1) == Vector3f(30.0, 30.0, 0.0));
    }

    void testAddEdgePointToRetriangulatorEdge() {
        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 10.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 30.0, 0.0));

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);
        retriangulator.addEdgePointToRetriangulatorEdge(
            mEdge0,
            retriangulatorFace,
            0,    // faceLineSegmentIndex
            0);   // endpointIndex

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0) 
            ->edgePointCount() == 1);
    }

    void testCreateEdgePoints() {
        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0) 
            ->edgePointCount() == 0);

        retriangulator.createEdgePointsFromEndpointIdentifierEdgePtrs();

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0) 
            ->edgePointCount() == 1);
    }

    void testRetriangulate() {
        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 10.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::createUniqueIdentifier());
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.retriangulate();

        CPPUNIT_ASSERT(mesh::IsConsistent(*mMesh));

        // TODO: At this point, we'd test to make sure that the
        // expected number of triangles were output.
        // This should be some number larger than one.
    }

    void testRetriangulateWeldFaceLineSegmentEndpointsEqual() {
        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 10.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::createUniqueIdentifier());
        faceLineSegment.setWorldPosition(1, Vector3f(10.0, 10.0001, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            faceLineSegment.endpointIdentifier(0));

        CPPUNIT_ASSERT(faceLineSegment.worldPosition(0) != faceLineSegment.worldPosition(1));
        CPPUNIT_ASSERT(faceLineSegment.worldPosition(0).equivalent(
                           faceLineSegment.worldPosition(1), cgmath::TOLERANCE));

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.retriangulate();

        CPPUNIT_ASSERT(mesh::IsConsistent(*mMesh));

        CPPUNIT_ASSERT(mMesh->vertexCount() == 4);
        CPPUNIT_ASSERT(mMesh->edgeCount() == 6);
        CPPUNIT_ASSERT(mMesh->faceCount() == 3);
    }

    void testRetriangulateWeldFaceLineSegmentEndpointsUnequal() {
        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 10.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::createUniqueIdentifier());
        faceLineSegment.setWorldPosition(1, Vector3f(10.0, 10.0001, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());

        CPPUNIT_ASSERT(faceLineSegment.worldPosition(0) != faceLineSegment.worldPosition(1));
        CPPUNIT_ASSERT(faceLineSegment.worldPosition(0).equivalent(
                           faceLineSegment.worldPosition(1), cgmath::TOLERANCE));

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.retriangulate();

        CPPUNIT_ASSERT(mesh::IsConsistent(*mMesh));

        CPPUNIT_ASSERT(mMesh->vertexCount() == 5);
        CPPUNIT_ASSERT(mMesh->edgeCount() != 6);
        CPPUNIT_ASSERT(mMesh->faceCount() != 3);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RetriangulatorTest);
