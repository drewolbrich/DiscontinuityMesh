// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <exact/GeometricPredicates.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>

using namespace exact;
using cgmath::Vector2f;
using cgmath::Vector3f;

class GeometricPredicatesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GeometricPredicatesTest);
    CPPUNIT_TEST(testTestOrientation2dCounterclockwise);
    CPPUNIT_TEST(testTestOrientation2dClockwise);
    CPPUNIT_TEST(testTestOrientation2dColinear);
    CPPUNIT_TEST(testTestOrientation2dCoincident);
    CPPUNIT_TEST(testTestOrientation3dBelow);
    CPPUNIT_TEST(testTestOrientation3dAbove);
    CPPUNIT_TEST(testTestOrientation3dCoplanar);
    CPPUNIT_TEST(testTestOrientation3dColinear);
    CPPUNIT_TEST(testTestInCircleInside);
    CPPUNIT_TEST(testTestInCircleOutside);
    CPPUNIT_TEST(testTestInCircleCocircular);
    CPPUNIT_TEST(testTestInSphereInside);
    CPPUNIT_TEST(testTestInSphereOutside);
    CPPUNIT_TEST(testTestInSphereCospherical);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fHorizontal);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fVertical);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fDiagonal);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fDiagonalReversed);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fEndpoint);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fDegenerate);
    CPPUNIT_TEST(testTestPointIntersectsLineSegment2fAllCoincident);
    CPPUNIT_TEST(testTestPointIntersectsLine2fSuccess);
    CPPUNIT_TEST(testTestPointIntersectsLine2fFailure);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fFailAbove);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fFailBelow);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fFailAboveAndBelow);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fFailAboveAndLeft);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fFailOneDegenerate);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fFailBothDegenerate);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fPassUpDown);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fPassDownUp);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fPassEndpoint);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fPassCoincidentEndpoints);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fPassColinear);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fPassAllCoincident);
    CPPUNIT_TEST(testTestLineSegmentsIntersect2fDisjointOneDegenerate);
    CPPUNIT_TEST(testTestLineSegmentsAreColinear2fOverlap);
    CPPUNIT_TEST(testTestLineSegmentsAreColinear2fEndpoint);
    CPPUNIT_TEST(testTestLineSegmentsAreColinear2fDisjoint);
    CPPUNIT_TEST(testTestLineSegmentsAreColinear2fFailure);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testTestOrientation2dCounterclockwise() {
        const Vector2f a(0, 0);
        const Vector2f b(1, 0);
        const Vector2f c(1, 1);
        CPPUNIT_ASSERT(TestOrientation2d(a, b, c) > 0.0);
    }

    void testTestOrientation2dClockwise() {
        const Vector2f a(0, 0);
        const Vector2f b(0, 1);
        const Vector2f c(1, 1);
        CPPUNIT_ASSERT(TestOrientation2d(a, b, c) < 0.0);
    }

    void testTestOrientation2dColinear() {
        const Vector2f a(0, 0);
        const Vector2f b(1, 0);
        const Vector2f c(2, 0);
        CPPUNIT_ASSERT(TestOrientation2d(a, b, c) == 0.0);
    }

    void testTestOrientation2dCoincident() {
        const Vector2f a(2, 3);
        const Vector2f b(2, 3);
        const Vector2f c(3, 4);
        CPPUNIT_ASSERT(TestOrientation2d(a, b, c) == 0.0);
    }

    void testTestOrientation3dBelow() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(1, 1, 0);
        const Vector3f d(0, 0, -1);
        CPPUNIT_ASSERT(TestOrientation3d(a, b, c, d) > 0.0);
    }

    void testTestOrientation3dAbove() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(1, 1, 0);
        const Vector3f d(0, 0, 1);
        CPPUNIT_ASSERT(TestOrientation3d(a, b, c, d) < 0.0);
    }

    void testTestOrientation3dCoplanar() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(1, 1, 0);
        const Vector3f d(1, 1, 0);
        CPPUNIT_ASSERT(TestOrientation3d(a, b, c, d) == 0.0);
    }

    void testTestOrientation3dColinear() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(2, 0, 0);
        const Vector3f d(3, 0, 0);
        CPPUNIT_ASSERT(TestOrientation3d(a, b, c, d) == 0.0);
    }

    void testTestInCircleInside() {
        const Vector2f a(0, 0);
        const Vector2f b(1, 0);
        const Vector2f c(1, 1);
        const Vector2f d(0.5, 0.5);
        CPPUNIT_ASSERT(TestInCircle(a, b, c, d) > 0.0);
    }

    void testTestInCircleOutside() {
        const Vector2f a(0, 0);
        const Vector2f b(1, 0);
        const Vector2f c(1, 1);
        const Vector2f d(2, 2);
        CPPUNIT_ASSERT(TestInCircle(a, b, c, d) < 0.0);
    }

    void testTestInCircleCocircular() {
        const Vector2f a(0, 0);
        const Vector2f b(1, 0);
        const Vector2f c(1, 1);
        const Vector2f d(0, 1);
        CPPUNIT_ASSERT(TestInCircle(a, b, c, d) == 0.0);
    }

    void testTestInSphereInside() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(1, 1, 0);
        const Vector3f d(0, 0, -1);
        const Vector3f e(0.5, 0.5, -0.5);
        CPPUNIT_ASSERT(TestInSphere(a, b, c, d, e) > 0.0);
    }

    void testTestInSphereOutside() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(1, 1, 0);
        const Vector3f d(0, 0, -1);
        const Vector3f e(2, 2, 2);
        CPPUNIT_ASSERT(TestInSphere(a, b, c, d, e) < 0.0);
    }

    void testTestInSphereCospherical() {
        const Vector3f a(0, 0, 0);
        const Vector3f b(1, 0, 0);
        const Vector3f c(1, 1, 0);
        const Vector3f d(0, 0, -1);
        const Vector3f e(1, 1, -1);
        CPPUNIT_ASSERT(TestInSphere(a, b, c, d, e) == 0.0);
    }

    void testTestPointIntersectsLineSegment2fHorizontal() {
        const Vector2f a(0, 0);
        const Vector2f b(5, 0);
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 0)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(-1, 0)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(6, 0)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 1)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, -1)));
    }

    void testTestPointIntersectsLineSegment2fVertical() {
        const Vector2f a(0, 0);
        const Vector2f b(0, 4);
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(0, 2)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(0, -1)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(0, 6)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(1, 2)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(-1, 2)));
    }

    void testTestPointIntersectsLineSegment2fDiagonal() {
        const Vector2f a(0, 0);
        const Vector2f b(4, 4);
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 2)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(-1, -1)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(5, 5)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 3)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 1)));
    }

    void testTestPointIntersectsLineSegment2fDiagonalReversed() {
        const Vector2f a(0, 4);
        const Vector2f b(4, 0);
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 2)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(-1, 5)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(5, -1)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(3, 2)));
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(1, 2)));
    }

    void testTestPointIntersectsLineSegment2fEndpoint() {
        const Vector2f a(0, 0);
        const Vector2f b(4, 1);
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(0, 0)));
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(4, 1)));
    }

    void testTestPointIntersectsLineSegment2fDegenerate() {
        const Vector2f a(2, 3);
        const Vector2f b(2, 3);
        CPPUNIT_ASSERT(!TestLineSegmentIntersectsPoint2f(a, b, Vector2f(3, 4)));
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 3)));
    }

    void testTestPointIntersectsLineSegment2fAllCoincident() {
        const Vector2f a(2, 3);
        const Vector2f b(2, 3);
        CPPUNIT_ASSERT(TestLineSegmentIntersectsPoint2f(a, b, Vector2f(2, 3)));
    }

    void testTestPointIntersectsLine2fSuccess() {
        CPPUNIT_ASSERT(TestLineIntersectsPoint2f(
                           Vector2f(2, 4), Vector2f(4, 8),
                           Vector2f(6, 12)));
    }

    void testTestPointIntersectsLine2fFailure() {
        CPPUNIT_ASSERT(!TestLineIntersectsPoint2f(
                           Vector2f(2, 4), Vector2f(4, 8),
                           Vector2f(6, 13)));
    }

    void testTestLineSegmentsIntersect2fFailAbove() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(4, 2);
        const Vector2f b2(4, 4);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fFailBelow() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(4, -2);
        const Vector2f b2(4, -4);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fFailAboveAndBelow() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(0, 4);
        const Vector2f b2(0, -4);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fFailAboveAndLeft() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(0, 4);
        const Vector2f b2(0, 2);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fFailOneDegenerate() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(0, 2);
        const Vector2f b2(0, 2);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fFailBothDegenerate() {
        const Vector2f a1(2, 0);
        const Vector2f a2(2, 0);
        const Vector2f b1(0, 2);
        const Vector2f b2(0, 2);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fPassUpDown() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(4, 2);
        const Vector2f b2(4, -2);
        CPPUNIT_ASSERT(TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fPassDownUp() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(4, 2);
        const Vector2f b2(4, -2);
        CPPUNIT_ASSERT(TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fPassEndpoint() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(4, 2);
        const Vector2f b2(4, 0);
        CPPUNIT_ASSERT(TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fPassCoincidentEndpoints() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(6, 0);
        const Vector2f b2(2, 0);
        CPPUNIT_ASSERT(TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fPassColinear() {
        const Vector2f a1(2, 0);
        const Vector2f a2(6, 0);
        const Vector2f b1(4, 0);
        const Vector2f b2(8, 0);
        CPPUNIT_ASSERT(TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fPassAllCoincident() {
        const Vector2f a1(2, 0);
        const Vector2f a2(2, 0);
        const Vector2f b1(2, 0);
        const Vector2f b2(2, 0);
        CPPUNIT_ASSERT(TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsIntersect2fDisjointOneDegenerate() {
        const Vector2f a1(2, 0);
        const Vector2f a2(2, 0);
        const Vector2f b1(3, 0);
        const Vector2f b2(4, 0);
        CPPUNIT_ASSERT(!TestLineSegmentsIntersect2f(a1, a2, b1, b2));
    }

    void testTestLineSegmentsAreColinear2fOverlap() {
        CPPUNIT_ASSERT(TestLineSegmentsAreColinear2f(
                           Vector2f(2.0, 0.0), Vector2f(8.0, 0.0),
                           Vector2f(3.0, 0.0), Vector2f(9.0, 0.0)));
    }

    void testTestLineSegmentsAreColinear2fEndpoint() {
        CPPUNIT_ASSERT(TestLineSegmentsAreColinear2f(
                           Vector2f(2.0, 0.0), Vector2f(8.0, 0.0),
                           Vector2f(8.0, 0.0), Vector2f(9.0, 0.0)));
    }

    void testTestLineSegmentsAreColinear2fDisjoint() {
        CPPUNIT_ASSERT(TestLineSegmentsAreColinear2f(
                           Vector2f(2.0, 0.0), Vector2f(8.0, 0.0),
                           Vector2f(9.0, 0.0), Vector2f(10.0, 0.0)));
    }

    void testTestLineSegmentsAreColinear2fFailure() {
        CPPUNIT_ASSERT(!TestLineSegmentsAreColinear2f(
                           Vector2f(2.0, 0.0), Vector2f(8.0, 0.0),
                           Vector2f(9.0, 0.0), Vector2f(10.0, 1.0)));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(GeometricPredicatesTest);
