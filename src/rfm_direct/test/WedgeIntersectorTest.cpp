// Copyright 2008 Retroactive Fiasco.

#include <cassert>
#include <cmath>

#include <cppunit/extensions/HelperMacros.h>

#include <rfm_direct/WedgeIntersector.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <mesh/Types.h>
#include <mesh/Mesh.h>
#include <mesh/Vertex.h>
#include <mesh/Edge.h>
#include <mesh/Face.h>
#include <meshretri/EndpointIdentifier.h>

using cgmath::Vector2f;
using cgmath::Vector3f;
using meshretri::EndpointIdentifier;

class WedgeIntersectorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(WedgeIntersectorTest);
    CPPUNIT_TEST(testVeWedgeSpace);
    CPPUNIT_TEST(testEvWedgeSpace);
    CPPUNIT_TEST(testWorldWedgeTransformation);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleSimple);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleClipLeft);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleClipRight);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleClipLeftReversed);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleClipRightReversed);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleFullyClippedLeft);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleFullyClippedRight);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleClippedTop);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleTopAligned);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleInFrontOfWedge);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleBehindOfWedge);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleCoplanarWithWedge);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleOneVertex);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleTwoVertices);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleIntersectsVPlane);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleLeftEmpty);
    CPPUNIT_TEST(testIntersectVeWedgeWithTriangleRightEmpty);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleBelowV);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleAboveV);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleStraddleOccluder);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleIntersectsVPlane);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleIntersectsV);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleIntersectsP);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleIntersectsQ);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleLeftEmpty);
    CPPUNIT_TEST(testIntersectEvWedgeWithTriangleRightEmpty);
    CPPUNIT_TEST_SUITE_END();

public:
    mesh::Mesh mMesh;
    mesh::VertexPtr mV;
    mesh::VertexPtr mP;
    mesh::VertexPtr mQ;
    mesh::EdgePtr mPQ;
    mesh::VertexPtr mA;
    mesh::VertexPtr mB;
    mesh::VertexPtr mC;
    mesh::EdgePtr mAB;
    mesh::EdgePtr mBC;
    mesh::EdgePtr mCA;
    mesh::FacePtr mABC;

    void setUp() {
        mV = mMesh.createVertex();
        mP = mMesh.createVertex();
        mQ = mMesh.createVertex();

        mPQ = mMesh.createEdge();
        mPQ->addAdjacentVertex(mP);
        mPQ->addAdjacentVertex(mQ);
        mP->addAdjacentEdge(mPQ);
        mQ->addAdjacentEdge(mPQ);

        mA = mMesh.createVertex();
        mB = mMesh.createVertex();
        mC = mMesh.createVertex();

        mAB = mMesh.createEdge();
        mAB->addAdjacentVertex(mA);
        mAB->addAdjacentVertex(mB);
        mA->addAdjacentEdge(mAB);
        mB->addAdjacentEdge(mAB);

        mBC = mMesh.createEdge();
        mBC->addAdjacentVertex(mB);
        mBC->addAdjacentVertex(mC);
        mB->addAdjacentEdge(mBC);
        mC->addAdjacentEdge(mBC);

        mCA = mMesh.createEdge();
        mCA->addAdjacentVertex(mC);
        mCA->addAdjacentVertex(mA);
        mC->addAdjacentEdge(mCA);
        mA->addAdjacentEdge(mCA);

        mABC = mMesh.createFace();
        mABC->addAdjacentVertex(mA);
        mABC->addAdjacentVertex(mB);
        mABC->addAdjacentVertex(mC);
        mABC->addAdjacentEdge(mAB);
        mABC->addAdjacentEdge(mBC);
        mABC->addAdjacentEdge(mCA);
        mA->addAdjacentFace(mABC);
        mB->addAdjacentFace(mABC);
        mC->addAdjacentFace(mABC);
        mAB->addAdjacentFace(mABC);
        mBC->addAdjacentFace(mABC);
        mCA->addAdjacentFace(mABC);
    }

    void tearDown() {
        // Reset the mesh.
        mesh::Mesh tempMesh;
        mMesh.swap(tempMesh);
    }

    int intersectVeWedgeWithTriangle(mesh::VertexPtr vertexPtr, mesh::EdgePtr edgePtr, 
        mesh::FacePtr facePtr, Endpoint *endpoint0, Endpoint *endpoint1) {

        WedgeIntersector wedgeIntersector;
        wedgeIntersector.setVeEventWedge(vertexPtr, edgePtr);
        LineSegment *lineSegmentArray = NULL;
        int intersections = wedgeIntersector.testTriangle(facePtr, &lineSegmentArray);
        assert(intersections < 2);
        if (intersections == 0) {
            return 0;
        }

        *endpoint0 = lineSegmentArray[0].point0();
        *endpoint1 = lineSegmentArray[0].point1();

        return 1;
    }

    int intersectEvWedgeWithTriangle(mesh::EdgePtr edgePtr, mesh::VertexPtr vertexPtr, 
        mesh::FacePtr facePtr, Endpoint *endpoint0, Endpoint *endpoint1,
        Endpoint *endpoint2, Endpoint *endpoint3) {

        WedgeIntersector wedgeIntersector;
        wedgeIntersector.setEvEventWedge(edgePtr, vertexPtr);
        LineSegment *lineSegmentArray = NULL;
        int intersections = wedgeIntersector.testTriangle(facePtr, &lineSegmentArray);
        assert(intersections < 3);
        if (intersections == 0) {
            return 0;
        }

        *endpoint0 = lineSegmentArray[0].point0();
        *endpoint1 = lineSegmentArray[0].point1();

        if (intersections == 2) {
            *endpoint2 = lineSegmentArray[1].point0();
            *endpoint3 = lineSegmentArray[1].point1();
        }

        return intersections;
    }

    void testVeWedgeSpace() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(6, -1, 0);
        Vector3f b(2, -1, 0);
        Vector3f c(4, -1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(ep0.visibilityParameter() == 0.25);
        CPPUNIT_ASSERT(ep1.visibilityParameter() == 0.75);
        CPPUNIT_ASSERT(fabs((ep0.wedgePosition() - Vector2f(-2, -8)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.wedgePosition() - Vector2f(2, -8)).length()) < 0.001);
    }

    void testEvWedgeSpace() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 11, 0);
        Vector3f q(6, 11, 0);
        Vector3f a(6, -1, 0);
        Vector3f b(2, -1, 0);
        Vector3f c(4, -1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(ep0.visibilityParameter() == 0.75);
        CPPUNIT_ASSERT(ep1.visibilityParameter() == 0.25);
        CPPUNIT_ASSERT(fabs((ep0.wedgePosition() - Vector2f(2, -8)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.wedgePosition() - Vector2f(-2, -8)).length()) < 0.001);
    }

    void testWorldWedgeTransformation() {
        WedgeIntersector wedgeIntersector;

        mV->setPosition(Vector3f(4, 7, 0));
        mP->setPosition(Vector3f(2, 11, 0));
        mQ->setPosition(Vector3f(6, 11, 0));

        wedgeIntersector.setVeEventWedge(mV, mPQ);

        cgmath::Vector3f worldSpacePoint(6, 10, 0);
        cgmath::Vector2f wedgeSpacePoint;
        wedgeIntersector.transformWorldSpacePointToWedgeSpacePoint(worldSpacePoint,
            &wedgeSpacePoint);
        CPPUNIT_ASSERT(wedgeSpacePoint == Vector2f(2, -3));
        wedgeIntersector.transformWedgeSpacePointToWorldSpacePoint(wedgeSpacePoint,
            &worldSpacePoint);
        CPPUNIT_ASSERT(worldSpacePoint == Vector3f(6, 10, 0));
    }

    void testIntersectVeWedgeWithTriangleSimple() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(6, -3, -1);
        Vector3f b(0, -3, -1);
        Vector3f c(3, -3, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.2) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.6) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(1, -3, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(5, -3, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setEdgePtrAndIndex(mCA, 1);
        EndpointIdentifier id1;
        id1.setEdgePtrAndIndex(mBC, 1);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleClipLeft() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(3, -1, -1);
        Vector3f b(-3, -1, -1);
        Vector3f c(0, -1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(ep0.visibilityParameter() == 0.0);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.25) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(0, -1, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(2, -1, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtrPair(mV, mP);
        EndpointIdentifier id1;
        id1.setEdgePtrAndIndex(mCA, 1);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleClipRight() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(9, 1, -1);
        Vector3f b(3, 1, -1);
        Vector3f c(6, 1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.5) < 0.001);
        CPPUNIT_ASSERT(ep1.visibilityParameter() == 1.0);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(4, 1, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(7, 1, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setEdgePtrAndIndex(mBC, 1);
        EndpointIdentifier id1;
        id1.setVertexPtrPair(mV, mQ);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleClipLeftReversed() {
        Vector3f v(4, 7, 0);
        Vector3f p(6, 3, 0);
        Vector3f q(2, 3, 0);
        Vector3f a(3, -1, -1);
        Vector3f b(-3, -1, -1);
        Vector3f c(0, -1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(ep0.visibilityParameter() == 1.0);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.75) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(0, -1, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(2, -1, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtrPair(mV, mQ);
        EndpointIdentifier id1;
        id1.setEdgePtrAndIndex(mCA, 1);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleClipRightReversed() {
        Vector3f v(4, 7, 0);
        Vector3f p(6, 3, 0);
        Vector3f q(2, 3, 0);
        Vector3f a(9, 1, -1);
        Vector3f b(3, 1, -1);
        Vector3f c(6, 1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.5) < 0.001);
        CPPUNIT_ASSERT(ep1.visibilityParameter() == 0.0);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(4, 1, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(7, 1, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setEdgePtrAndIndex(mBC, 1);
        EndpointIdentifier id1;
        id1.setVertexPtrPair(mV, mP);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleFullyClippedLeft() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(-20, -3, -1);
        Vector3f b(-14, -3, -1);
        Vector3f c(-17, -3, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleFullyClippedRight() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(20, -3, -1);
        Vector3f b(26, -3, -1);
        Vector3f c(23, -3, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleClippedTop() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(0, 25, -1);
        Vector3f b(6, 25, -1);
        Vector3f c(3, 25, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleTopAligned() {
        // This tests a triangle that intersects the top of the wedge (point V).
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(0, 7, -1);
        Vector3f b(6, 7, -1);
        Vector3f c(3, 7, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(4, 7, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(4, 7, 0)).length()) < 0.001);
    }

    void testIntersectVeWedgeWithTriangleInFrontOfWedge() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(0, -3, 5);
        Vector3f b(6, -3, 5);
        Vector3f c(3, -3, 8);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleBehindOfWedge() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(0, -3, -5);
        Vector3f b(6, -3, -5);
        Vector3f c(3, -3, -2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleCoplanarWithWedge() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(0, -2, 0);
        Vector3f b(6, -2, 0);
        Vector3f c(3, -6, 0);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleOneVertex() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(6, -3, -1);
        Vector3f b(0, -3, 0);
        Vector3f c(3, -3, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.1) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.6) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(0, -3, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(5, -3, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mB);
        EndpointIdentifier id1;
        id1.setEdgePtrAndIndex(mCA, 1);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleTwoVertices() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(6, -3, 0);
        Vector3f b(0, -3, 0);
        Vector3f c(3, -3, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.1) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.7) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(0, -3, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(6, -3, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtr(mB);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleIntersectsVPlane() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(3, 4, 0);
        Vector3f b(3, 7, 0);
        Vector3f c(3, 5.5, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.5/3.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(3, 4, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(3, 5, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtrPair(mV, mP);
        EndpointIdentifier id1;
        id1.setVertexPtr(mA);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectVeWedgeWithTriangleLeftEmpty() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(2, 3, 0);
        Vector3f b(-3.1234125, 3.2, -1);
        Vector3f c(-5, 3.1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectVeWedgeWithTriangleRightEmpty() {
        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);
        Vector3f a(6, 3, 0);
        Vector3f b(9.1234125, 3.2, -1);
        Vector3f c(12, 3.1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        int intersections = intersectVeWedgeWithTriangle(mV, mPQ, mABC, &ep0, &ep1);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectEvWedgeWithTriangleBelowV() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(8, -7, 0);
        Vector3f b(4, -7, 0);
        Vector3f c(6, -7, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 6.5/9.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 2.5/9.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(4, -7, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(8, -7, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtr(mB);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectEvWedgeWithTriangleAboveV() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(8, 7, 0);
        Vector3f b(4, 7, 0);
        Vector3f c(6, 7, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);
        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.5/5.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 4.5/5.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(4, 7, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(8, 7, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtr(mB);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectEvWedgeWithTriangleStraddleOccluder() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(5, -1, 0);
        Vector3f b(5, 5, 0);
        Vector3f c(5, 2, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 2);

        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.5/3.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(5, 4, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(5, 5, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mB);
        EndpointIdentifier id1;
        id1.setVertexPtrPair(mV, mP);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));

        CPPUNIT_ASSERT(fabs(ep2.visibilityParameter() - 2.5/3.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep3.visibilityParameter() - 1.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep2.worldPosition() - Vector3f(5, -1, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep3.worldPosition() - Vector3f(5, 0, 0)).length()) < 0.001);
        EndpointIdentifier id2;
        id2.setVertexPtr(mA);
        EndpointIdentifier id3;
        id3.setVertexPtrPair(mV, mQ);
        CPPUNIT_ASSERT((ep2.endpointIdentifier() == id2 && ep3.endpointIdentifier() == id3)
            || (ep2.endpointIdentifier() == id3 && ep3.endpointIdentifier() == id2));
    }

    void testIntersectEvWedgeWithTriangleIntersectsVPlane() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(5, -1, 0);
        Vector3f b(5, 2, 0);
        Vector3f c(5, 1, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);

        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 2.5/3.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 1.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(5, -1, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(5, 0, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtrPair(mV, mQ);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectEvWedgeWithTriangleIntersectsV() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(6, 5, 0);
        Vector3f b(6, 2, 0);
        Vector3f c(6, 3, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);

        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.5) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(6, 2, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(6, 5, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtr(mB);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectEvWedgeWithTriangleIntersectsP() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(6, 6, 0);
        Vector3f b(3, 8, 0);
        Vector3f c(4, 7, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);

        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.0) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 0.5) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(3, 8, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(6, 6, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtr(mB);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectEvWedgeWithTriangleIntersectsQ() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(9, 8, 0);
        Vector3f b(6, 6, 0);
        Vector3f c(8, 7, 2);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 1);

        CPPUNIT_ASSERT(fabs(ep0.visibilityParameter() - 0.5) < 0.001);
        CPPUNIT_ASSERT(fabs(ep1.visibilityParameter() - 1.0) < 0.001);
        CPPUNIT_ASSERT(fabs((ep0.worldPosition() - Vector3f(6, 6, 0)).length()) < 0.001);
        CPPUNIT_ASSERT(fabs((ep1.worldPosition() - Vector3f(9, 8, 0)).length()) < 0.001);
        EndpointIdentifier id0;
        id0.setVertexPtr(mA);
        EndpointIdentifier id1;
        id1.setVertexPtr(mB);
        CPPUNIT_ASSERT((ep0.endpointIdentifier() == id0 && ep1.endpointIdentifier() == id1)
            || (ep0.endpointIdentifier() == id1 && ep1.endpointIdentifier() == id0));
    }

    void testIntersectEvWedgeWithTriangleLeftEmpty() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(6, 2, 0);
        Vector3f b(-3.2414921, 2.112421, 0);
        Vector3f c(-9, 2.3125152, 1);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 0);
    }

    void testIntersectEvWedgeWithTriangleRightEmpty() {
        Vector3f v(6, 2, 0);
        Vector3f p(3, 8, 0);
        Vector3f q(9, 8, 0);
        Vector3f a(6, 2, 0);
        Vector3f b(7.24149219, 2.1, 0);
        Vector3f c(8, 2.3, 1);
        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);
        mA->setPosition(a);
        mB->setPosition(b);
        mC->setPosition(c);
        Endpoint ep0;
        Endpoint ep1;
        Endpoint ep2;
        Endpoint ep3;
        int intersections = intersectEvWedgeWithTriangle(mPQ, mV, mABC, 
            &ep0, &ep1, &ep2, &ep3);
        CPPUNIT_ASSERT(intersections == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WedgeIntersectorTest);
