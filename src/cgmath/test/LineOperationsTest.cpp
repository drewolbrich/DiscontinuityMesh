// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/LineOperations.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>

using cgmath::Vector2f;
using cgmath::Vector3f;

class LineOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(LineOperationsTest);
    CPPUNIT_TEST(testGetClosestPointsOnLinesSimple);
    CPPUNIT_TEST(testGetClosestPointsOnLinesParallel);
    CPPUNIT_TEST(testClipLineSegmentAgainstPlaneForward);
    CPPUNIT_TEST(testClipLineSegmentAgainstPlaneBackward);
    CPPUNIT_TEST(testClipLineSegmentAgainstPlaneDiagonal);
    CPPUNIT_TEST(testClipLineSegmentAgainstPlaneAngledPlane);
    CPPUNIT_TEST(testClipLineSegmentAgainstPlaneNoClip);
    CPPUNIT_TEST(testClipLineSegmentAgainstPlaneFullClip);
    CPPUNIT_TEST(testGetIntersectionOfLineWithPlane);
    CPPUNIT_TEST(testGetIntersectionOfLineWithPlaneParallel);
    CPPUNIT_TEST(testGetIntersectionOfTwoLinesSimple);
    CPPUNIT_TEST(testGetIntersectionOfTwoLinesPoint);
    CPPUNIT_TEST(testGetIntersectionOfTwoLinesParallel);
    CPPUNIT_TEST(testGetIntersectionOfTwoLinesDegenerate);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsSimple);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsEndpoint);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureLeft);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureRight);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureTop);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureBottom);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureParallel);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureDegenerate);
    CPPUNIT_TEST(testGetIntersectionOfTwoLineSegmentsFailureColinear);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fCoincidentEdge);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fCoincidentEndpointLeft);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fCoincidentEndpointRight);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fNearestEdge);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fNearestEndpointLeft);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fNearestEndpointRight);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment2fCoincident);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fCoincidentEdge);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fCoincidentEndpointLeft);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fCoincidentEndpointRight);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fNearestEdge);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fNearestEndpointLeft);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fNearestEndpointRight);
    CPPUNIT_TEST(testGetClosestPointOnLineSegment3fCoincident);
    CPPUNIT_TEST(testDistanceFromLineSegmentToPoint2f);
    CPPUNIT_TEST(testDistanceFromLineSegmentToPoint2fLeft);
    CPPUNIT_TEST(testDistanceFromLineSegmentToPoint3f);
    CPPUNIT_TEST(testDistanceFromLineSegmentToPoint3fLeft);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    }

    void tearDown() {
    }

    void testGetClosestPointsOnLinesSimple() {
        Vector3f p0(1.0, 3.0, 2.0);
        Vector3f p1(7.0, 3.0, 2.0);
        Vector3f q0(9.0, 1.0, 4.0);
        Vector3f q1(9.0, 2.0, 4.0);
        Vector3f cp;
        Vector3f cq;
        bool foundClosestPoints = cgmath::GetClosestPointsOnLines(p0, p1, q0, q1, &cp, &cq);
        CPPUNIT_ASSERT(foundClosestPoints);
        CPPUNIT_ASSERT(cp == Vector3f(9.0, 3.0, 2.0));
        CPPUNIT_ASSERT(cq == Vector3f(9.0, 3.0, 4.0));
    }

    void testGetClosestPointsOnLinesParallel() {
        Vector3f p0(1.0, 3.0, 2.0);
        Vector3f p1(7.0, 3.0, 2.0);
        Vector3f q0(1.0, 3.0, 4.0);
        Vector3f q1(2.0, 3.0, 4.0);
        Vector3f cp;
        Vector3f cq;
        bool foundClosestPoints = cgmath::GetClosestPointsOnLines(p0, p1, q0, q1, &cp, &cq);
        CPPUNIT_ASSERT(!foundClosestPoints);
    }

    void testClipLineSegmentAgainstPlaneForward() {
        Vector3f p(0.0, 0.0, 5.0);
        Vector3f n(0.0, 0.0, 1.0);
        Vector3f ep0(3.0, 4.0, -23.0);
        Vector3f ep1(3.0, 4.0, 9.0);
        bool resultIsNonEmpty = cgmath::ClipLineSegmentAgainstPlane(&ep0, &ep1, p, n);
        CPPUNIT_ASSERT(resultIsNonEmpty);
        CPPUNIT_ASSERT(ep0 == Vector3f(3.0, 4.0, 5.0));
        CPPUNIT_ASSERT(ep1 == Vector3f(3.0, 4.0, 9.0));
    }

    void testClipLineSegmentAgainstPlaneBackward() {
        Vector3f p(0.0, 0.0, 5.0);
        Vector3f n(0.0, 0.0, 1.0);
        Vector3f ep0(3.0, 4.0, 9.0);
        Vector3f ep1(3.0, 4.0, -23.0);
        bool resultIsNonEmpty = cgmath::ClipLineSegmentAgainstPlane(&ep0, &ep1, p, n);
        CPPUNIT_ASSERT(resultIsNonEmpty);
        CPPUNIT_ASSERT(ep0 == Vector3f(3.0, 4.0, 9.0));
        CPPUNIT_ASSERT(ep1 == Vector3f(3.0, 4.0, 5.0));
    }

    void testClipLineSegmentAgainstPlaneDiagonal() {
        Vector3f p(3.0, 6.0, 0.0);
        Vector3f n(1.0, 0.0, 0.0);
        Vector3f ep0(1.0, 3.0, 0.0);
        Vector3f ep1(7.0, 6.0, 0.0);
        bool resultIsNonEmpty = cgmath::ClipLineSegmentAgainstPlane(&ep0, &ep1, p, n);
        CPPUNIT_ASSERT(resultIsNonEmpty);
        CPPUNIT_ASSERT(ep0 == Vector3f(3.0, 4.0, 0.0));
        CPPUNIT_ASSERT(ep1 == Vector3f(7.0, 6.0, 0.0));
    }

    void testClipLineSegmentAgainstPlaneAngledPlane() {
        Vector3f p(3.0, 5.0, 0.0);
        Vector3f n(2.0, 1.0, 0.0);
        n.normalize();
        Vector3f ep0(1.0, 2.0, 0.0);
        Vector3f ep1(10.0, 5.0, 0.0);
        bool resultIsNonEmpty = cgmath::ClipLineSegmentAgainstPlane(&ep0, &ep1, p, n);
        CPPUNIT_ASSERT(resultIsNonEmpty);
        CPPUNIT_ASSERT(ep0 == Vector3f(4.0, 3.0, 0.0));
        CPPUNIT_ASSERT(ep1 == Vector3f(10.0, 5.0, 0.0));
    }

    void testClipLineSegmentAgainstPlaneNoClip() {
        Vector3f p(0.0, 0.0, 5.0);
        Vector3f n(0.0, 0.0, 1.0);
        Vector3f ep0(3.0, 4.0, 6.0);
        Vector3f ep1(3.0, 4.0, 9.0);
        bool resultIsNonEmpty = cgmath::ClipLineSegmentAgainstPlane(&ep0, &ep1, p, n);
        CPPUNIT_ASSERT(resultIsNonEmpty);
        CPPUNIT_ASSERT(ep0 == Vector3f(3.0, 4.0, 6.0));
        CPPUNIT_ASSERT(ep1 == Vector3f(3.0, 4.0, 9.0));
    }

    void testClipLineSegmentAgainstPlaneFullClip() {
        Vector3f p(0.0, 0.0, 5.0);
        Vector3f n(0.0, 0.0, 1.0);
        Vector3f ep0(3.0, 4.0, 2.0);
        Vector3f ep1(3.0, 4.0, 3.0);
        bool resultIsNonEmpty = cgmath::ClipLineSegmentAgainstPlane(&ep0, &ep1, p, n);
        CPPUNIT_ASSERT(!resultIsNonEmpty);
    }

    void testGetIntersectionOfLineWithPlane() {
        Vector3f p0(3, 4, 5);
        Vector3f p1(3, 4, 12);
        Vector3f p(11, 12, 7);
        Vector3f n(0, 0, 1);
        Vector3f ip;
        bool intersectionExists = cgmath::GetIntersectionOfLineWithPlane(p0, p1, p, n, &ip);
        CPPUNIT_ASSERT(intersectionExists);
        CPPUNIT_ASSERT(ip == Vector3f(3, 4, 7));
    }

    void testGetIntersectionOfLineWithPlaneParallel() {
        Vector3f p0(3, 4, 5);
        Vector3f p1(3, 4, 12);
        Vector3f p(11, 12, 7);
        Vector3f n(1, 0, 0);
        Vector3f ip;
        bool intersectionExists = cgmath::GetIntersectionOfLineWithPlane(p0, p1, p, n, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLinesSimple() {
        Vector2f p0(0, 0);
        Vector2f p1(10, 10);
        Vector2f p2(5, 0);
        Vector2f p3(-1, 4);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLines(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(intersectionExists);
        CPPUNIT_ASSERT(ip == Vector2f(2, 2));
    }

    void testGetIntersectionOfTwoLinesPoint() {
        Vector2f p0(4, 3);
        Vector2f p1(10, 10);
        Vector2f p2(4, 3);
        Vector2f p3(117, 201);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLines(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(intersectionExists);
        CPPUNIT_ASSERT(ip == Vector2f(4, 3));

        p0 = Vector2f(17, 3);
        p1 = Vector2f(117, 201);
        intersectionExists = cgmath::GetIntersectionOfTwoLines(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(intersectionExists);
        CPPUNIT_ASSERT(ip == Vector2f(117, 201));
    }

    void testGetIntersectionOfTwoLinesParallel() {
        Vector2f p0(10, 12);
        Vector2f p1(23, 27);
        Vector2f p2(40, 42);
        Vector2f p3(53, 57);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLines(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLinesDegenerate() {
        Vector2f p0(10, 12);
        Vector2f p1(23, 27);
        Vector2f p2(40, 42);
        Vector2f p3(40, 42);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLines(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsSimple() {
        Vector2f p0(0, 0);
        Vector2f p1(10, 10);
        Vector2f p2(5, 0);
        Vector2f p3(-1, 4);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(intersectionExists);
        CPPUNIT_ASSERT(ip == Vector2f(2, 2));
    }

    void testGetIntersectionOfTwoLineSegmentsEndpoint() {
        Vector2f p0(0, 0);
        Vector2f p1(4, 3);
        Vector2f p2(5, 0);
        Vector2f p3(4, 3);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(intersectionExists);
        CPPUNIT_ASSERT(ip == Vector2f(4, 3));
    }

    void testGetIntersectionOfTwoLineSegmentsFailureLeft() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 11);
        Vector2f p2(3, 12);
        Vector2f p3(3, -5);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsFailureRight() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 11);
        Vector2f p2(14, 12);
        Vector2f p3(14, -5);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsFailureTop() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 11);
        Vector2f p2(3, 12);
        Vector2f p3(14, 13);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsFailureBottom() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 11);
        Vector2f p2(3, -12);
        Vector2f p3(14, -13);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsFailureParallel() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 11);
        Vector2f p2(6, 6);
        Vector2f p3(11, 12);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsFailureDegenerate() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 10);
        Vector2f p2(6, 6);
        Vector2f p3(6, 6);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetIntersectionOfTwoLineSegmentsFailureColinear() {
        Vector2f p0(5, 5);
        Vector2f p1(10, 10);
        Vector2f p2(6, 6);
        Vector2f p3(7, 7);
        Vector2f ip;
        bool intersectionExists = cgmath::GetIntersectionOfTwoLineSegments(p0, p1, p2, p3, &ip);
        CPPUNIT_ASSERT(!intersectionExists);
    }

    void testGetClosestPointOnLineSegment2fCoincidentEdge() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(3, 0);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(3, 0));
    }

    void testGetClosestPointOnLineSegment2fCoincidentEndpointLeft() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(0, 0);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(0, 0));
    }

    void testGetClosestPointOnLineSegment2fCoincidentEndpointRight() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(10, 0);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(10, 0));
    }

    void testGetClosestPointOnLineSegment2fNearestEdge() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(3, 4);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(3, 0));
    }

    void testGetClosestPointOnLineSegment2fNearestEndpointLeft() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(-3, 4);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(0, 0));
    }

    void testGetClosestPointOnLineSegment2fNearestEndpointRight() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(13, 4);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(10, 0));
    }

    void testGetClosestPointOnLineSegment2fCoincident() {
        Vector2f q0(10, 0);
        Vector2f q1(10, 0);
        Vector2f p(13, 4);
        Vector2f result;
        GetClosestPointOnLineSegment2f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector2f(10, 0));
    }

    void testGetClosestPointOnLineSegment3fCoincidentEdge() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(3, 0, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(3, 0, 0));
    }

    void testGetClosestPointOnLineSegment3fCoincidentEndpointLeft() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(0, 0, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(0, 0, 0));
    }

    void testGetClosestPointOnLineSegment3fCoincidentEndpointRight() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(10, 0, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(10, 0, 0));
    }

    void testGetClosestPointOnLineSegment3fNearestEdge() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(3, 4, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(3, 0, 0));
    }

    void testGetClosestPointOnLineSegment3fNearestEndpointLeft() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(-3, 4, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(0, 0, 0));
    }

    void testGetClosestPointOnLineSegment3fNearestEndpointRight() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(13, 4, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(10, 0, 0));
    }

    void testGetClosestPointOnLineSegment3fCoincident() {
        Vector3f q0(10, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(13, 4, 0);
        Vector3f result;
        GetClosestPointOnLineSegment3f(q0, q1, p, &result);
        CPPUNIT_ASSERT(result == Vector3f(10, 0, 0));
    }

    void testDistanceFromLineSegmentToPoint2f() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(3, 8);
        Vector2f result;
        CPPUNIT_ASSERT(GetDistanceFromLineSegmentToPoint2f(q0, q1, p) == 8.0);
    }

    void testDistanceFromLineSegmentToPoint2fLeft() {
        Vector2f q0(0, 0);
        Vector2f q1(10, 0);
        Vector2f p(-4, 0);
        Vector2f result;
        CPPUNIT_ASSERT(GetDistanceFromLineSegmentToPoint2f(q0, q1, p) == 4.0);
    }

    void testDistanceFromLineSegmentToPoint3f() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(3, 8, 0);
        Vector3f result;
        CPPUNIT_ASSERT(GetDistanceFromLineSegmentToPoint3f(q0, q1, p) == 8.0);
    }

    void testDistanceFromLineSegmentToPoint3fLeft() {
        Vector3f q0(0, 0, 0);
        Vector3f q1(10, 0, 0);
        Vector3f p(-4, 0, 0);
        Vector3f result;
        CPPUNIT_ASSERT(GetDistanceFromLineSegmentToPoint3f(q0, q1, p) == 4.0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LineOperationsTest);
