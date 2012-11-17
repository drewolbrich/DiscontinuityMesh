// Copyright 2008 Retroactive Fiasco.

#include <cmath>

#include <cppunit/extensions/HelperMacros.h>

#include <rfm_direct/LineSegmentCollection.h>
#include <rfm_direct/WedgeIntersector.h>
#include <mesh/Mesh.h>
#include <meshretri/EndpointIdentifier.h>

using cgmath::Vector2f;
using cgmath::Vector3f;
using meshretri::EndpointIdentifier;

class LineSegmentCollectionTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(LineSegmentCollectionTest);
    CPPUNIT_TEST(testSplitTwoLineSegments);
    CPPUNIT_TEST(testSplitSharedEndpoints);
    CPPUNIT_TEST(testSplitEndpointAlongLine);
    CPPUNIT_TEST(testFindVisibleLineSegmentsSimple);
    CPPUNIT_TEST(testFindVisibleLineSharedEndpoint);
    CPPUNIT_TEST_SUITE_END();

public:
    mesh::Mesh *mMesh;
    mesh::VertexPtr mV;
    mesh::VertexPtr mP;
    mesh::VertexPtr mQ;
    mesh::EdgePtr mPQ;
    WedgeIntersector *mWedgeIntersector;

    void setUp() {
        mMesh = new mesh::Mesh;

        mV = mMesh->createVertex();
        mP = mMesh->createVertex();
        mQ = mMesh->createVertex();

        Vector3f v(4, 7, 0);
        Vector3f p(2, 3, 0);
        Vector3f q(6, 3, 0);

        mV->setPosition(v);
        mP->setPosition(p);
        mQ->setPosition(q);

        mPQ = mMesh->createEdge();
        mPQ->addAdjacentVertex(mP);
        mPQ->addAdjacentVertex(mQ);
        mP->addAdjacentEdge(mPQ);
        mQ->addAdjacentEdge(mPQ);

        mWedgeIntersector = new WedgeIntersector;
        mWedgeIntersector->setVeEventWedge(mV, mPQ);
    }

    void tearDown() {
        delete mMesh;
        delete mWedgeIntersector;
    }

    void testSplitTwoLineSegments() {
        EndpointIdentifier id0a = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id0b = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id1a = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id1b = EndpointIdentifier::createUniqueIdentifier();

        LineSegmentCollection lineSegmentCollection;
        lineSegmentCollection.setWedgeIntersector(mWedgeIntersector);
        Endpoint point0;
        Endpoint point1;
        LineSegment lineSegment;

        cgmath::Vector2f wedgePosition;

        point0.setWorldPosition(Vector3f(2, 2, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(0.2);
        point0.setEndpointIdentifier(id0a);

        point1.setWorldPosition(Vector3f(8, 5, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(0.8);
        point1.setEndpointIdentifier(id0b);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        point0.setWorldPosition(Vector3f(5, 0, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(0.5);
        point0.setEndpointIdentifier(id1b);

        point1.setWorldPosition(Vector3f(2, 9, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(0.2);
        point1.setEndpointIdentifier(id1a);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        lineSegmentCollection.findLineSegmentIntersections();
        lineSegmentCollection.splitLineSegments();

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 4);

        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(2, 2, 0), Vector2f(-2, -5), id0a,
                Vector3f(4, 3, 0), Vector2f(0, -4)));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(2, 9, 0), Vector2f(-2, 2), id1a,
                Vector3f(4, 3, 0), Vector2f(0, -4)));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(8, 5, 0), Vector2f(4, -2), id0b,
                Vector3f(4, 3, 0), Vector2f(0, -4)));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(5, 0, 0), Vector2f(1, -7), id1b,
                Vector3f(4, 3, 0), Vector2f(0, -4)));
    }

    void testSplitSharedEndpoints() {
        EndpointIdentifier id0b = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id1a = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();

        LineSegmentCollection lineSegmentCollection;
        lineSegmentCollection.setWedgeIntersector(mWedgeIntersector);
        Endpoint point0;
        Endpoint point1;
        LineSegment lineSegment;

        point0.setWorldPosition(Vector3f(4, 3, 0));
        point0.setWedgePosition(Vector2f(0.4, 0.3));
        point0.setVisibilityParameter(0.4);
        point0.setEndpointIdentifier(id2);
        point1.setWorldPosition(Vector3f(8, 5, 0));
        point1.setWedgePosition(Vector2f(0.8, 0.5));
        point1.setVisibilityParameter(0.8);
        point1.setEndpointIdentifier(id0b);
        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        point0.setWorldPosition(Vector3f(4, 3, 0));
        point0.setWedgePosition(Vector2f(0.4, 0.3));
        point0.setVisibilityParameter(0.4);
        point0.setEndpointIdentifier(id2);
        point1.setWorldPosition(Vector3f(2, 9, 0));
        point1.setWedgePosition(Vector2f(0.2, 0.9));
        point1.setVisibilityParameter(0.2);
        point1.setEndpointIdentifier(id1a);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        lineSegmentCollection.findLineSegmentIntersections();
        lineSegmentCollection.splitLineSegments();

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(2, 9, 0), Vector2f(0.2, 0.9), id1a,
                Vector3f(4, 3, 0), Vector2f(0.4, 0.3)));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(8, 5, 0), Vector2f(0.8, 0.5), id0b,
                Vector3f(4, 3, 0), Vector2f(0.4, 0.3)));
    }

    void testSplitEndpointAlongLine() {
        EndpointIdentifier id0a = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id0b = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id1a = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id1b = EndpointIdentifier::createUniqueIdentifier();

        LineSegmentCollection lineSegmentCollection;
        lineSegmentCollection.setWedgeIntersector(mWedgeIntersector);
        Endpoint point0;
        Endpoint point1;
        LineSegment lineSegment;

        cgmath::Vector2f wedgePosition;

        point0.setWorldPosition(Vector3f(2, 4, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(0.2);
        point0.setEndpointIdentifier(id0a);

        point1.setWorldPosition(Vector3f(8, 4, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(0.8);
        point1.setEndpointIdentifier(id0b);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        point0.setWorldPosition(Vector3f(4, 4, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(0.4);
        point0.setEndpointIdentifier(id1b);

        point1.setWorldPosition(Vector3f(4, 8, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(0.4);
        point1.setEndpointIdentifier(id1a);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        lineSegmentCollection.findLineSegmentIntersections();
        lineSegmentCollection.splitLineSegments();

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 3);

        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(2, 4, 0), Vector2f(-2, -3), id0a,
                Vector3f(4, 4, 0), Vector2f(0, -3)));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(8, 4, 0), Vector2f(4, -3), id0b,
                Vector3f(4, 4, 0), Vector2f(0, -3)));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(4, 8, 0), Vector2f(0, 1), id1a,
                Vector3f(4, 4, 0), Vector2f(0, -3)));
    }

    void testFindVisibleLineSegmentsSimple() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();

        LineSegmentCollection lineSegmentCollection;
        lineSegmentCollection.setWedgeIntersector(mWedgeIntersector);
        Endpoint point0;
        Endpoint point1;
        LineSegment lineSegment;

        cgmath::Vector2f wedgePosition;

        point0.setWorldPosition(Vector3f(1, -5, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(-3, -12));
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point0.worldPosition()));
        CPPUNIT_ASSERT(point0.visibilityParameter() == 0.25);
        point0.setEndpointIdentifier(id1);

        point1.setWorldPosition(Vector3f(7, -5, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(3, -12));
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point1.worldPosition()));
        CPPUNIT_ASSERT(point1.visibilityParameter() == 0.75);
        point1.setEndpointIdentifier(id2);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        point0.setWorldPosition(Vector3f(4, -9, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(0, -16));
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point0.worldPosition()));
        CPPUNIT_ASSERT(point0.visibilityParameter() == 0.5);
        point0.setEndpointIdentifier(id3);

        point1.setWorldPosition(Vector3f(12, -9, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(8, -16));
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point1.worldPosition()));
        CPPUNIT_ASSERT(point1.visibilityParameter() == 1.0);
        point1.setEndpointIdentifier(id4);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        lineSegmentCollection.findVisibleLineSegments();
        
        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(1, -5, 0), Vector2f(-3, -12), id1,
                Vector3f(7, -5, 0), Vector2f(3, -12), id2));
#if 0
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(8, -9, 0), Vector2f(4, -16), id2,
                Vector3f(12, -9, 0), Vector2f(8, -16), id4));
#endif
    }

    void testFindVisibleLineSharedEndpoint() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();

        LineSegmentCollection lineSegmentCollection;
        lineSegmentCollection.setWedgeIntersector(mWedgeIntersector);
        Endpoint point0;
        Endpoint point1;
        LineSegment lineSegment;

        cgmath::Vector2f wedgePosition;

        point0.setWorldPosition(Vector3f(1, -5, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(-3, -12));
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point0.worldPosition()));
        CPPUNIT_ASSERT(point0.visibilityParameter() == 0.25);
        point0.setEndpointIdentifier(id1);

        point1.setWorldPosition(Vector3f(7, -5, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(3, -12));
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point1.worldPosition()));
        CPPUNIT_ASSERT(point1.visibilityParameter() == 0.75);
        point1.setEndpointIdentifier(id2);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        point0.setWorldPosition(Vector3f(7, -5, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point0.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(3, -12));
        point0.setWedgePosition(wedgePosition);
        point0.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point0.worldPosition()));
        CPPUNIT_ASSERT(point0.visibilityParameter() == 0.75);
        point0.setEndpointIdentifier(id2);

        point1.setWorldPosition(Vector3f(12, -9, 0));
        mWedgeIntersector->transformWorldSpacePointToWedgeSpacePoint(
            point1.worldPosition(), &wedgePosition);
        CPPUNIT_ASSERT(wedgePosition == Vector2f(8, -16));
        point1.setWedgePosition(wedgePosition);
        point1.setVisibilityParameter(mWedgeIntersector->projectPointOntoEdgePQ(
                                          point1.worldPosition()));
        CPPUNIT_ASSERT(point1.visibilityParameter() == 1.0);
        point1.setEndpointIdentifier(id3);

        lineSegment.setPoint0(point0);
        lineSegment.setPoint1(point1);
        lineSegmentCollection.addLineSegment(lineSegment);

        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        lineSegmentCollection.findVisibleLineSegments();
        
        CPPUNIT_ASSERT(lineSegmentCollection.size() == 2);

        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(1, -5, 0), Vector2f(-3, -12), id1,
                Vector3f(7, -5, 0), Vector2f(3, -12), id2));
        CPPUNIT_ASSERT(lineSegmentIsInCollection(lineSegmentCollection,
                Vector3f(7, -5, 0), Vector2f(3, -12), id2,
                Vector3f(12, -9, 0), Vector2f(8, -16), id3));
    }

    bool lineSegmentIsInCollection(const LineSegmentCollection &lineSegmentCollection,
        const Vector3f &world0, const Vector2f &wedge0, EndpointIdentifier &id0,
        const Vector3f &world1, const Vector2f &wedge1) {

        for (LineSegmentCollection::const_iterator iterator 
            = lineSegmentCollection.begin();
            iterator != lineSegmentCollection.end(); ++iterator) {
            const LineSegment &lineSegment = *iterator;

            if (equivalent(lineSegment.point0().worldPosition(), world0)
                && equivalent(lineSegment.point0().wedgePosition(), wedge0)
                && lineSegment.point0().endpointIdentifier() == id0
                && equivalent(lineSegment.point1().worldPosition(), world1)
                && equivalent(lineSegment.point1().wedgePosition(), wedge1)) {
                return true;
            }
            if (equivalent(lineSegment.point0().worldPosition(), world1)
                && equivalent(lineSegment.point0().wedgePosition(), wedge1)
                && equivalent(lineSegment.point1().worldPosition(), world0)
                && equivalent(lineSegment.point1().wedgePosition(), wedge0)
                && lineSegment.point1().endpointIdentifier() == id0) {
                return true;
            }
        }

        return false;
    }

    bool lineSegmentIsInCollection(const LineSegmentCollection &lineSegmentCollection,
        const Vector3f &world0, const Vector2f &wedge0, EndpointIdentifier &id0,
        const Vector3f &world1, const Vector2f &wedge1, EndpointIdentifier &id1) {

        for (LineSegmentCollection::const_iterator iterator 
            = lineSegmentCollection.begin();
            iterator != lineSegmentCollection.end(); ++iterator) {
            const LineSegment &lineSegment = *iterator;

            if (equivalent(lineSegment.point0().worldPosition(), world0)
                && equivalent(lineSegment.point0().wedgePosition(), wedge0)
                && lineSegment.point0().endpointIdentifier() == id0
                && equivalent(lineSegment.point1().worldPosition(), world1)
                && equivalent(lineSegment.point1().wedgePosition(), wedge1)
                && lineSegment.point1().endpointIdentifier() == id1) {
                return true;
            }
            if (equivalent(lineSegment.point0().worldPosition(), world1)
                && equivalent(lineSegment.point0().wedgePosition(), wedge1)
                && lineSegment.point0().endpointIdentifier() == id1
                && equivalent(lineSegment.point1().worldPosition(), world0)
                && equivalent(lineSegment.point1().wedgePosition(), wedge0)
                && lineSegment.point1().endpointIdentifier() == id0) {
                return true;
            }
        }

        return false;
    }

    bool equivalent(const Vector3f &lhs, const Vector3f &rhs) const {
        return fabs((lhs - rhs).length() < 0.001);
    }

    bool equivalent(const Vector2f &lhs, const Vector2f &rhs) const {
        return fabs((lhs - rhs).length() < 0.001);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LineSegmentCollectionTest);
