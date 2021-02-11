// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <meshretri/RetriangulatorEdge.h>
#include <meshretri/EdgePoint.h>
#include <meshretri/RetriangulatorFace.h>
#include <meshretri/FaceLineSegment.h>
#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <cgmath/Vector3f.h>

using meshretri::RetriangulatorEdge;
using meshretri::EdgePoint;
using meshretri::RetriangulatorFace;
using meshretri::FaceLineSegment;
using meshretri::EndpointIdentifier;
using cgmath::Vector3f;

class RetriangulatorEdgeTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RetriangulatorEdgeTest);
    CPPUNIT_TEST(testEdgePtr);
    CPPUNIT_TEST(testEpsilon3f);
    CPPUNIT_TEST(testAddEdgePoint);
    CPPUNIT_TEST(testWeldEdgePointsCreatedFromCommonWedge);
    CPPUNIT_TEST(testGetNearestEdgePointFromSpanOneEdgePoint);
    CPPUNIT_TEST(testGetNearestEdgePointFromSpanTwoEdgePoints);
    CPPUNIT_TEST(testGetNearestEdgePointFromSpanThreeEdgePoints);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testEdgePtr() {
        mesh::Mesh mesh;
        mesh::EdgePtr edge = mesh.createEdge();

        RetriangulatorEdge retriangulatorEdge;
        retriangulatorEdge.setEdgePtr(edge);

        CPPUNIT_ASSERT(retriangulatorEdge.edgePtr() == edge);
    }

    void testEpsilon3f() {
        RetriangulatorEdge retriangulatorEdge;

        retriangulatorEdge.setEpsilon(100.0);
        CPPUNIT_ASSERT(retriangulatorEdge.epsilon() == 100.0);
    }

    void testAddEdgePoint() {
        RetriangulatorEdge retriangulatorEdge;

        CPPUNIT_ASSERT(retriangulatorEdge.edgePointCount() == 0);

        EdgePoint edgePoint;
        retriangulatorEdge.addEdgePoint(edgePoint);
        retriangulatorEdge.addEdgePoint(edgePoint);
        retriangulatorEdge.addEdgePoint(edgePoint);

        CPPUNIT_ASSERT(retriangulatorEdge.edgePointCount() == 3);
    }

    void testWeldEdgePointsCreatedFromCommonWedge() {
        RetriangulatorEdge retriangulatorEdge;

        RetriangulatorFace retriangulatorFace1;
        RetriangulatorFace retriangulatorFace2;

        FaceLineSegment faceLineSegment;

        EndpointIdentifier id = EndpointIdentifier::createUniqueIdentifier();

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 200.0, 300.0));
        faceLineSegment.setEndpointIdentifier(0, id);
        retriangulatorFace1.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.001, 200.0, 300.0));
        faceLineSegment.setEndpointIdentifier(0, id);
        retriangulatorFace2.addFaceLineSegment(faceLineSegment);
        
        EdgePoint edgePoint;

        edgePoint.setRetriangulatorFace(&retriangulatorFace1);
        edgePoint.setFaceLineSegmentIndex(0);
        edgePoint.setEndpointIndex(0);
        retriangulatorEdge.addEdgePoint(edgePoint);

        edgePoint.setRetriangulatorFace(&retriangulatorFace2);
        edgePoint.setFaceLineSegmentIndex(0);
        edgePoint.setEndpointIndex(0);
        retriangulatorEdge.addEdgePoint(edgePoint);

        retriangulatorEdge.setEpsilon(0.001);
        retriangulatorEdge.weldEdgePointsCreatedFromCommonWedge();

        CPPUNIT_ASSERT(retriangulatorFace1.faceLineSegmentCount() == 1);
        CPPUNIT_ASSERT(retriangulatorFace2.faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulatorFace1.faceLineSegment(0).worldPosition(0)
            == retriangulatorFace2.faceLineSegment(0).worldPosition(0));
    }

    void testGetNearestEdgePointFromSpanOneEdgePoint() {
        RetriangulatorEdge retriangulatorEdge;

        EdgePoint edgePoint;
        retriangulatorEdge.addEdgePoint(edgePoint);

        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(0, 0) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(1, 0) == 0);
    }

    void testGetNearestEdgePointFromSpanTwoEdgePoints() {
        RetriangulatorEdge retriangulatorEdge;

        EdgePoint edgePoint;
        retriangulatorEdge.addEdgePoint(edgePoint);
        retriangulatorEdge.addEdgePoint(edgePoint);
        
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(0, 0) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(1, 0) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(2, 0) == 1);

        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(0, 1) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(1, 1) == 1);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(2, 1) == 1);
    }

    void testGetNearestEdgePointFromSpanThreeEdgePoints() {
        RetriangulatorEdge retriangulatorEdge;

        EdgePoint edgePoint;
        retriangulatorEdge.addEdgePoint(edgePoint);
        retriangulatorEdge.addEdgePoint(edgePoint);
        retriangulatorEdge.addEdgePoint(edgePoint);
        
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(0, 0) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(1, 0) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(2, 0) == 1);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(3, 0) == 2);

        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(0, 2) == 0);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(1, 2) == 1);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(2, 2) == 2);
        CPPUNIT_ASSERT(retriangulatorEdge.getNearestEdgePointFromSpan(3, 2) == 2);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RetriangulatorEdgeTest);
