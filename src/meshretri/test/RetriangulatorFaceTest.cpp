// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <meshretri/RetriangulatorFace.h>
#include <meshretri/FaceLineSegment.h>
#include <meshretri/Retriangulator.h>
#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <mesh/IsConsistent.h>
#include <cgmath/Vector3f.h>

using meshretri::Retriangulator;
using meshretri::RetriangulatorFace;
using meshretri::RetriangulatorEdge;
using meshretri::EdgePoint;
using meshretri::FaceLineSegment;
using meshretri::PointSnapper;
using meshretri::EndpointIdentifier;
using cgmath::Vector3f;

class RetriangulatorFaceTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RetriangulatorFaceTest);
    CPPUNIT_TEST(testSetFacePtr);
    CPPUNIT_TEST(testAddFaceLineSegment);
    CPPUNIT_TEST(testSetFaceLineSegmentEndpointPosition);
    CPPUNIT_TEST(testSetAxis);
    CPPUNIT_TEST(testSetEpsilon);
    CPPUNIT_TEST(testCreateEdgepointsFromEndpointIdentifierEdgePtrsNoPoints);
    CPPUNIT_TEST(testCreateEdgepointsFromEndpointIdentifierEdgePtrsLeftPoint);
    CPPUNIT_TEST(testCreateEdgepointsFromEndpointIdentifierEdgePtrsRightPoint);
    CPPUNIT_TEST(testCreateEdgepointsFromEndpointIdentifierEdgePtrsTwoPoints);
    CPPUNIT_TEST(testCreateEdgepointsFromEndpointIdentifierEdgePtrsColinearWithEdge);
    CPPUNIT_TEST(testCreateEdgePointsFromPerimeterIntersectionsExactEdgeEndpointIntersection);
    CPPUNIT_TEST(testCreateEdgePointsFromPerimeterIntersectionsLooseEdgeEndpointIntersection);
    CPPUNIT_TEST(testCreateEdgePointsFromPerimeterIntersectionsBothEndpointsInside);
    CPPUNIT_TEST(testCreateEdgePointsFromPerimeterIntersectionsBothEndpointsOutside);
    CPPUNIT_TEST(testCreateEdgePointsFromPerimeterIntersectionsTwoEdges);
    CPPUNIT_TEST(testCreateEdgePointsFromPerimeterIntersectionsDegenerateSpan);
    CPPUNIT_TEST(testSplitAllMutuallyIntersectingFaceLineSegmentsFailure);
    CPPUNIT_TEST(testSplitAllMutuallyIntersectingFaceLineSegmentsSuccess);
    CPPUNIT_TEST(testSplitAllMutuallyIntersectingFaceLineSegmentsTwice);
    CPPUNIT_TEST(testSplitAllMutuallyIntersectingFaceLineSegmentsEndpoints);
    CPPUNIT_TEST(testSplitAllMutuallyIntersectingFaceLineSegmentsCoincident);
    CPPUNIT_TEST(testSplitFaceLineSegment);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsFullOverlap);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsEndpoints);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsEndpointsFlipped);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsEndpointsReversed);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsLeftOverlap);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsRightOverlap);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsLeftOverlapReversed);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsRightOverlapReversed);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsLeftStraddle);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsRightStraddle);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsMiddle);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsFullStraddle);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsLeftDegenerate);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsRightDegenerate);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsBothDegenerate);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsDegenerateMiddle);
    CPPUNIT_TEST(testSplitColinearFaceLineSegmentsDegenerateStraddle);
    CPPUNIT_TEST(testPerimeterSpansIntersectSimple);
    CPPUNIT_TEST(testPerimeterSpansIntersectSinglePoint);
    CPPUNIT_TEST(testPerimeterSpansIntersectDegenerate);
    CPPUNIT_TEST(testPerimeterSpansIntersectOverlappedColinearNoSharedEndpoints);
    CPPUNIT_TEST(testPerimeterSpansIntersectOverlappedColinearOneSharedEndpoint);
    CPPUNIT_TEST(testPerimeterSpansIntersectOverlappedColinearTwoSharedEndpoints);
    CPPUNIT_TEST(testPerimeterSpansIntersectOneSharedEndpoint);
    CPPUNIT_TEST(testPerimeterSpansIntersectNonoverlappedColinearOneSharedEndpoint);
    CPPUNIT_TEST(testPerimeterSpansIntersectOneSharedEndpointDegenerate);
    CPPUNIT_TEST(testPerimeterSpansIntersectTwoDegenerate);
    CPPUNIT_TEST(testPerimeterSpansIntersectNoIntersection);
    CPPUNIT_TEST(testCorrectSelfIntersectingFacePerimeterNoIntersection);
    CPPUNIT_TEST(testCorrectSelfIntersectingFacePerimeterSymmetricalIntersection);
    CPPUNIT_TEST(testCorrectSelfIntersectingFacePerimeterAsymmetricalIntersection);
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

    void testSetFacePtr() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        CPPUNIT_ASSERT(retriangulatorFace.facePtr() == mFace);
    }

    void testAddFaceLineSegment() {
        RetriangulatorFace retriangulatorFace;

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 200.0, 300.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 200.0, 300.0));
    }

    void testSetFaceLineSegmentEndpointPosition() {
        RetriangulatorFace retriangulatorFace;

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 200.0, 300.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 200.0, 300.0));

        retriangulatorFace.faceLineSegment(0).setWorldPosition(0, Vector3f(50.0, 60.0, 70.0));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 60.0, 70.0));
    }

    void testSetAxis() {
        RetriangulatorFace retriangulatorFace;

        retriangulatorFace.setAxis0(1);
        retriangulatorFace.setAxis1(2);
        retriangulatorFace.setAxis2(0);

        CPPUNIT_ASSERT(retriangulatorFace.axis0() == 1);
        CPPUNIT_ASSERT(retriangulatorFace.axis1() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.axis2() == 0);
    }

    void testSetEpsilon() {
        RetriangulatorFace retriangulatorFace;

        retriangulatorFace.setEpsilon(100.0);
        CPPUNIT_ASSERT(retriangulatorFace.epsilon() == 100.0);
    }

    void testCreateEdgepointsFromEndpointIdentifierEdgePtrsNoPoints() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(1.0);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 10.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::createUniqueIdentifier());
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);
    }

    void testCreateEdgepointsFromEndpointIdentifierEdgePtrsLeftPoint() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(1.0);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);
    }

    void testCreateEdgepointsFromEndpointIdentifierEdgePtrsRightPoint() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(1.0);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::createUniqueIdentifier());
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);
    }

    void testCreateEdgepointsFromEndpointIdentifierEdgePtrsTwoPoints() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(1.0);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 2);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);
    }

    void testCreateEdgepointsFromEndpointIdentifierEdgePtrsColinearWithEdge() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(1.0);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 2);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() > 0);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isColinearWithExistingMeshEdge());
    }

    void testCreateEdgePointsFromPerimeterIntersectionsExactEdgeEndpointIntersection() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, 0.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        retriangulatorFace->createEdgePointsFromPerimeterIntersections(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);

        const EdgePoint &edgePoint = retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(0);
        CPPUNIT_ASSERT(edgePoint.retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint.faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint.endpointIndex() == 1);
        CPPUNIT_ASSERT(edgePoint.position().equivalent(cgmath::Vector3f(40.0, 0.0, 0.0), 0.001));
    }

    void testCreateEdgePointsFromPerimeterIntersectionsLooseEdgeEndpointIntersection() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, -5.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        retriangulatorFace->createEdgePointsFromPerimeterIntersections(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);

        const EdgePoint &edgePoint = retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(0);
        CPPUNIT_ASSERT(edgePoint.retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint.faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint.endpointIndex() == 1);
        CPPUNIT_ASSERT(edgePoint.position().equivalent(cgmath::Vector3f(40.0, -5.0, 0.0), 0.001));
    }

    void testCreateEdgePointsFromPerimeterIntersectionsBothEndpointsInside() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 40.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        retriangulatorFace->createEdgePointsFromPerimeterIntersections(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);
    }

    void testCreateEdgePointsFromPerimeterIntersectionsBothEndpointsOutside() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(50.0, -5.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, -5.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        retriangulatorFace->createEdgePointsFromPerimeterIntersections(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 2);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);

        const EdgePoint *edgePoint = &retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(0);
        CPPUNIT_ASSERT(edgePoint->retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint->faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->endpointIndex() == 1);
        CPPUNIT_ASSERT(edgePoint->position().equivalent(cgmath::Vector3f(40.0, -5.0, 0.0), 0.001));

        edgePoint = &retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(1);
        CPPUNIT_ASSERT(edgePoint->retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint->faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->endpointIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->position().equivalent(cgmath::Vector3f(50.0, -5.0, 0.0), 0.001));

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 2);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->spanCount() == 3);

        cgmath::Vector2f p0;
        cgmath::Vector2f p1;
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->getSpanEndpoints2d(0, &p0, &p1, 0, 1);
        CPPUNIT_ASSERT(p0 == cgmath::Vector2f(0.0, 0.0));
        CPPUNIT_ASSERT(p1 == cgmath::Vector2f(40.0, -5.0));
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->getSpanEndpoints2d(1, &p0, &p1, 0, 1);
        CPPUNIT_ASSERT(p0 == cgmath::Vector2f(40.0, -5.0));
        CPPUNIT_ASSERT(p1 == cgmath::Vector2f(50.0, -5.0));
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->getSpanEndpoints2d(2, &p0, &p1, 0, 1);
        CPPUNIT_ASSERT(p0 == cgmath::Vector2f(50.0, -5.0));
        CPPUNIT_ASSERT(p1 == cgmath::Vector2f(100.0, 0.0));
    }

    void testCreateEdgePointsFromPerimeterIntersectionsTwoEdges() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(-5.0, 40.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, -5.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        retriangulatorFace->createEdgePointsFromPerimeterIntersections(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 1);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 1);

        const EdgePoint *edgePoint = &retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(0);
        CPPUNIT_ASSERT(edgePoint->retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint->faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->endpointIndex() == 1);
        CPPUNIT_ASSERT(edgePoint->position().equivalent(cgmath::Vector3f(40.0, -5.0, 0.0), 0.001));

        edgePoint = &retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePoint(0);
        CPPUNIT_ASSERT(edgePoint->retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint->faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->endpointIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->position().equivalent(cgmath::Vector3f(-5.0, 40.0, 0.0), 0.001));
    }

    void testCreateEdgePointsFromPerimeterIntersectionsDegenerateSpan() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 30.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        // Since we manually add this EdgePoint to the edge later,
        // we have to mark it as such, so that it's not added to the edge
        // a second time.
        faceLineSegment.setEdgePtr(1, mEdge0);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0);
        EdgePoint ep;
        ep.setRetriangulatorFace(retriangulatorFace);
        ep.setFaceLineSegmentIndex(0);
        ep.setEndpointIndex(1);
        retriangulatorEdge->addEdgePoint(ep);
        retriangulatorEdge->updateEdgePoints();

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);

        faceLineSegment = FaceLineSegment();
        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 10.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(90.0, 0.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulatorFace->createEdgePointsFromPerimeterIntersections(&retriangulator);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 2);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 0);

        const EdgePoint *edgePoint = &retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(0);
        CPPUNIT_ASSERT(edgePoint->retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint->faceLineSegmentIndex() == 1);
        CPPUNIT_ASSERT(edgePoint->endpointIndex() == 1);
        CPPUNIT_ASSERT(edgePoint->position().equivalent(cgmath::Vector3f(90.0, 0.0, 0.0), 0.001));

        edgePoint = &retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePoint(1);
        CPPUNIT_ASSERT(edgePoint->retriangulatorFace() == retriangulatorFace);
        CPPUNIT_ASSERT(edgePoint->faceLineSegmentIndex() == 0);
        CPPUNIT_ASSERT(edgePoint->endpointIndex() == 1);
        CPPUNIT_ASSERT(edgePoint->position().equivalent(cgmath::Vector3f(100.0, 0.0, 0.0), 0.001));
    }

    void testSplitAllMutuallyIntersectingFaceLineSegmentsFailure() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 40.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(20.0, 100, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        faceLineSegment.setWorldPosition(0, Vector3f(30.0, 20.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 20.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(!retriangulatorFace->splitAllMutuallyIntersectingFaceLineSegments());

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(0)
            == Vector3f(20.0, 40.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 100.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(0)
            == Vector3f(30.0, 20.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(1)
            == Vector3f(30.0, 20.0, 0.0));
    }

    void testSplitAllMutuallyIntersectingFaceLineSegmentsSuccess() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 40.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(20.0, 100, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 50.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, 50.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulatorFace->splitAllMutuallyIntersectingFaceLineSegments());

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 4);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(0)
            == Vector3f(20.0, 40.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(0)
            == Vector3f(10.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(2).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(2).worldPosition(1)
            == Vector3f(20.0, 100.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(3).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(3).worldPosition(1)
            == Vector3f(40.0, 50.0, 0.0));
    }

    void testSplitAllMutuallyIntersectingFaceLineSegmentsTwice() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 40.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(20.0, 100, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        faceLineSegment.setWorldPosition(0, Vector3f(10.0, 50.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, 50.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        // First pass. The two lines should get split into four.
        CPPUNIT_ASSERT(retriangulatorFace->splitAllMutuallyIntersectingFaceLineSegments());

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 4);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(0)
            == Vector3f(20.0, 40.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(0)
            == Vector3f(10.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(2).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(2).worldPosition(1)
            == Vector3f(20.0, 100.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(3).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(3).worldPosition(1)
            == Vector3f(40.0, 50.0, 0.0));

        // Second pass. Nothing should happen because the
        // lines are already split.
        CPPUNIT_ASSERT(!retriangulatorFace->splitAllMutuallyIntersectingFaceLineSegments());

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 4);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(0)
            == Vector3f(20.0, 40.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(0)
            == Vector3f(10.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(2).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(2).worldPosition(1)
            == Vector3f(20.0, 100.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(3).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(3).worldPosition(1)
            == Vector3f(40.0, 50.0, 0.0));
    }

    void testSplitAllMutuallyIntersectingFaceLineSegmentsEndpoints() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 40.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(20.0, 50.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 50.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(30.0, 50.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(!retriangulatorFace->splitAllMutuallyIntersectingFaceLineSegments());

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(0)
            == Vector3f(20.0, 40.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 50.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(0)
            == Vector3f(20.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(1)
            == Vector3f(30.0, 50.0, 0.0));
    }

    void testSplitAllMutuallyIntersectingFaceLineSegmentsCoincident() {
        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, 0.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        faceLineSegment.setWorldPosition(0, Vector3f(20.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(40.0, 0.0, 0.0));
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);

        retriangulator.initialize();

        RetriangulatorFace *retriangulatorFace 
            = retriangulator.getRetriangulatorFaceFromMeshFace(mFace);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulatorFace->splitAllMutuallyIntersectingFaceLineSegments());

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(0)
            == Vector3f(20.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 0.0, 0.0));

        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(0)
            == Vector3f(20.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace->faceLineSegment(1).worldPosition(1)
            == Vector3f(40.0, 0.0, 0.0));
    }

    void testSplitFaceLineSegment() {
        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));

        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 1);

        retriangulatorFace.splitFaceLineSegment(0, Vector3f(65.0, 0.0, 0.0), true);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(!retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(65.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(65.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(!retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsFullOverlap() {
        // +---+
        // +===+
        // +===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsEndpoints() {
        // +---+
        //     +===+
        // +---+===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsEndpointsFlipped() {
        // +---+
        //     +===+
        // +---+===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsEndpointsReversed() {
        //     +---+
        // +===+
        // +===+---+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsLeftOverlap() {
        // +-------+
        // +===+
        // +===+---+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(!retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsRightOverlap() {
        // +-------+
        //     +===+
        // +---+===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(!retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsLeftOverlapReversed() {
        // +---+
        // +=======+
        // +---+===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsRightOverlapReversed() {
        //     +---+
        // +=======+
        // +===+---+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsLeftStraddle() {
        //     +-------+
        // +=======+
        // +===+***+---+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, false);
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, false);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 3);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(!retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsRightStraddle() {
        // +-------+
        //     +=======+
        // +---+***+===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, false);
        faceLineSegment.setWorldPosition(1, Vector3f(250.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, false);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 3);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(250.0, 0.0, 0.0));
        CPPUNIT_ASSERT(!retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsMiddle() {
        // +-----------+
        //     +===+
        // +---+===+***+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(250.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, false);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, false);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 3);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(1));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(0)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(1)
            == Vector3f(250.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsFullStraddle() {
        //     +---+
        // +===========+
        // +===+---+***+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, false);
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, false);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(0, true);
        faceLineSegment.setWorldPosition(1, Vector3f(250.0, 0.0, 0.0));
        faceLineSegment.setIsDegreeZeroDiscontinuity(1, true);
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 3);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).isDegreeZeroDiscontinuity(0));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).isDegreeZeroDiscontinuity(1));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(0)
            == Vector3f(200.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(1)
            == Vector3f(250.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).isDegreeZeroDiscontinuity(1));
    }

    void testSplitColinearFaceLineSegmentsLeftDegenerate() {
        // +
        // +===+
        // +===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsRightDegenerate() {
        //     +
        // +===+
        // +===+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsBothDegenerate() {
        // +
        // +
        // +
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(100.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 2);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(100.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsDegenerateMiddle() {
        // +-------+
        //     +
        // +---+***+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 3);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
    }

    void testSplitColinearFaceLineSegmentsDegenerateStraddle() {
        //     +
        // +=======+
        // +===+***+
        RetriangulatorFace retriangulatorFace;
        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(150.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(150.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(100.0, 0.0, 0.0));
        faceLineSegment.setWorldPosition(1, Vector3f(200.0, 0.0, 0.0));
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        retriangulatorFace.splitColinearFaceLineSegments(0, 1);

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegmentCount() == 3);
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(100.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(0)
            == Vector3f(150.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(2).worldPosition(1)
            == Vector3f(200.0, 0.0, 0.0));
    }

    void testPerimeterSpansIntersectSimple() {
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 200.0),
                           cgmath::Vector2f(100.0, 200.0), cgmath::Vector2f(200.0, 100.0)));
    }

    void testPerimeterSpansIntersectSinglePoint() {
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(200.0, 200.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(200.0, 200.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
    }

    void testPerimeterSpansIntersectDegenerate() {
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
    }

    void testPerimeterSpansIntersectOverlappedColinearNoSharedEndpoints() {
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(110.0, 100.0), cgmath::Vector2f(190.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(190.0, 100.0), cgmath::Vector2f(110.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(110.0, 100.0), cgmath::Vector2f(190.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(190.0, 100.0), cgmath::Vector2f(110.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
    }

    void testPerimeterSpansIntersectOverlappedColinearOneSharedEndpoint() {
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(200.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(100.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(200.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(100.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
    }

    void testPerimeterSpansIntersectOverlappedColinearTwoSharedEndpoints() {
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(200.0, 100.0), cgmath::Vector2f(100.0, 100.0)));
    }

    void testPerimeterSpansIntersectOneSharedEndpoint() {
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 200.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(100.0, 200.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 200.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 200.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0)));
    }

    void testPerimeterSpansIntersectNonoverlappedColinearOneSharedEndpoint() {
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(200.0, 200.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(200.0, 200.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
    }

    void testPerimeterSpansIntersectOneSharedEndpointDegenerate() {
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(100.0, 100.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(150.0, 100.0), cgmath::Vector2f(150.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(100.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(150.0, 100.0)));
    }

    void testPerimeterSpansIntersectTwoDegenerate() {
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(100.0, 100.0),
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(100.0, 100.0)));
    }

    void testPerimeterSpansIntersectNoIntersection() {
        CPPUNIT_ASSERT(!RetriangulatorFace::perimeterSpansIntersect(
                           cgmath::Vector2f(100.0, 100.0), cgmath::Vector2f(200.0, 100.0),
                           cgmath::Vector2f(150.0, 150.0), cgmath::Vector2f(150.0, 200.0)));
    }

    void testCorrectSelfIntersectingFacePerimeterNoIntersection() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(1.0);

        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(50.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 10.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(0.0, 50.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge2, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(10.0, 50.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)->updateEdgePoints();
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)->updateEdgePoints();
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)->updateEdgePoints();

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 1);

        CPPUNIT_ASSERT(
            !retriangulatorFace.correctSelfIntersectingFacePerimeterLineSegments(&retriangulator));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(50.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(50.0, 10.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(0.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(10.0, 50.0, 0.0));
    }

    void testCorrectSelfIntersectingFacePerimeterSymmetricalIntersection() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(6.0);

        FaceLineSegment faceLineSegment;

        faceLineSegment.setWorldPosition(0, Vector3f(0.0, 5.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge0, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(10.0, 50.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        faceLineSegment.setWorldPosition(0, Vector3f(5.0, 0.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge2, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(50.0, 10.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)->updateEdgePoints();
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)->updateEdgePoints();
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)->updateEdgePoints();
 
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePtr() == mEdge0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePtr() == mEdge1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePtr() == mEdge2);

        CPPUNIT_ASSERT(mEdge0->hasAdjacentVertex(mVertex0));
        CPPUNIT_ASSERT(mEdge2->hasAdjacentVertex(mVertex0));

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 1);

        CPPUNIT_ASSERT(
            retriangulatorFace.correctSelfIntersectingFacePerimeterLineSegments(&retriangulator));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(0.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(10.0, 50.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(0)
            == Vector3f(0.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(1).worldPosition(1)
            == Vector3f(50.0, 10.0, 0.0));
    }

    void testCorrectSelfIntersectingFacePerimeterAsymmetricalIntersection() {
        RetriangulatorFace retriangulatorFace;
        retriangulatorFace.setFacePtr(mFace);
        retriangulatorFace.setAxis0(0);
        retriangulatorFace.setAxis1(1);
        retriangulatorFace.setAxis2(2);
        retriangulatorFace.setEpsilon(6.0);

        FaceLineSegment faceLineSegment;
        faceLineSegment.setWorldPosition(0, Vector3f(3.0, -3.0, 0.0));
        faceLineSegment.setEndpointIdentifier(0,
            EndpointIdentifier::fromEdgePtrAndIndex(mEdge2, 0));
        faceLineSegment.setWorldPosition(1, Vector3f(20.0, 45.0, 0.0));
        faceLineSegment.setEndpointIdentifier(1,
            EndpointIdentifier::createUniqueIdentifier());
        retriangulatorFace.addFaceLineSegment(faceLineSegment);

        Retriangulator retriangulator;
        retriangulator.setMesh(mMesh);
        retriangulator.addFaceLineSegmentToFace(faceLineSegment, mFace);
        retriangulator.initialize();

        retriangulatorFace.createEdgePointsFromEndpointIdentifierEdgePtrs(&retriangulator);

        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)->updateEdgePoints();
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)->updateEdgePoints();
        retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)->updateEdgePoints();
 
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePtr() == mEdge0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePtr() == mEdge1);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePtr() == mEdge2);

        CPPUNIT_ASSERT(mEdge0->hasAdjacentVertex(mVertex0));
        CPPUNIT_ASSERT(mEdge2->hasAdjacentVertex(mVertex0));

        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge0)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge1)
            ->edgePointCount() == 0);
        CPPUNIT_ASSERT(retriangulator.getRetriangulatorEdgeFromEdgePtr(mEdge2)
            ->edgePointCount() == 1);

        CPPUNIT_ASSERT(
            retriangulatorFace.correctSelfIntersectingFacePerimeterLineSegments(&retriangulator));

        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(0)
            == Vector3f(0.0, 0.0, 0.0));
        CPPUNIT_ASSERT(retriangulatorFace.faceLineSegment(0).worldPosition(1)
            == Vector3f(20.0, 45.0, 0.0));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RetriangulatorFaceTest);
