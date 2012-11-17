// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <rfm_direct/Region.h>
#include <rfm_direct/WedgeIntersector.h>
#include <meshretri/EndpointIdentifier.h>

using meshretri::EndpointIdentifier;

class RegionTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(RegionTest);
    CPPUNIT_TEST(testEmpty);
    CPPUNIT_TEST(testNotEmpty);
    CPPUNIT_TEST(testFull);
    CPPUNIT_TEST(testNotFull);
    CPPUNIT_TEST(testIsSingleSpan);
    CPPUNIT_TEST(testNotIsSingleSpan);
    CPPUNIT_TEST(testMinS);
    CPPUNIT_TEST(testMaxS);
    CPPUNIT_TEST(testComputeUnionSimple);
    CPPUNIT_TEST(testComputeUnionDisjointLeft);
    CPPUNIT_TEST(testComputeUnionDisjointRight);
    CPPUNIT_TEST(testComputeUnionSharedPointLeft);
    CPPUNIT_TEST(testComputeUnionSharedPointRight);
    CPPUNIT_TEST(testComputeUnionSharedPointLeftOverlap);
    CPPUNIT_TEST(testComputeUnionSharedPointRightOverlap);
    CPPUNIT_TEST(testComputeUnionOverlapTwo);
    CPPUNIT_TEST(testComputeUnionOverlapThree);
    CPPUNIT_TEST(testComputeUnionFullOverlapThree);
    CPPUNIT_TEST(testComputeUnionLhsEmpty);
    CPPUNIT_TEST(testDifferenceLeft);
    CPPUNIT_TEST(testDifferenceRight);
    CPPUNIT_TEST(testDifferenceCenter);
    CPPUNIT_TEST(testDifferenceEmpty);
    CPPUNIT_TEST(testDifferenceDisjointLeft);
    CPPUNIT_TEST(testDifferenceDisjointRight);
    CPPUNIT_TEST(testDifferenceSharedPointLeft);
    CPPUNIT_TEST(testDifferenceSharedPointRight);
    CPPUNIT_TEST(testDifferenceTwoLeft);
    CPPUNIT_TEST(testDifferenceTwoRight);
    CPPUNIT_TEST(testDifferenceThreeLeft);
    CPPUNIT_TEST(testDifferenceThreeRight);
    CPPUNIT_TEST(testDifferenceRhsEmpty);
    CPPUNIT_TEST(testDifferenceSharedPointLeftOverlap);
    CPPUNIT_TEST(testDifferenceSharedPointRightOverlap);
    CPPUNIT_TEST_SUITE_END();

public:
    WedgeIntersector mWedgeIntersector;

    void setUp() {
    }

    void tearDown() {
    }

    bool spansAreEqual(const Region::Span &lhs, const Region::Span &rhs) {
        return lhs.mMinS == rhs.mMinS
            && lhs.mMaxS == rhs.mMaxS
            && (lhs.mMinId == rhs.mMinId 
                || lhs.mMinId.isUndefined() || rhs.mMinId.isUndefined())
            && (lhs.mMaxId == rhs.mMaxId
                || lhs.mMaxId.isUndefined() || rhs.mMaxId.isUndefined());
    }

    void testEmpty() {
        Region region;
        CPPUNIT_ASSERT(region.empty());
    }

    void testNotEmpty() {
        Region region(0.25, 0.75,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        CPPUNIT_ASSERT(!region.empty());
    }

    void testFull() {
        Region region(0.0, 1.0,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        CPPUNIT_ASSERT(region.full());
    }

    void testNotFull() {
        Region region(0.25, 0.75,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        CPPUNIT_ASSERT(!region.full());
    }

    void testIsSingleSpan() {
        Region region(0.25, 0.75,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        CPPUNIT_ASSERT(region.isSingleSpan());
    }

    void testNotIsSingleSpan() {
        Region region(0.25, 0.5,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        region.appendSpan(Region::Span(0.6, 0.8,
                EndpointIdentifier::createUniqueIdentifier(),
                EndpointIdentifier::createUniqueIdentifier(),
                false, false));
        CPPUNIT_ASSERT(!region.isSingleSpan());
    }

    void testMinS() {
        Region region(0.5, 1.0,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        CPPUNIT_ASSERT(region.minS() == 0.5);
    }

    void testMaxS() {
        Region region(0.5, 1.0,
            EndpointIdentifier::createUniqueIdentifier(),
            EndpointIdentifier::createUniqueIdentifier(),
            false, false);
        CPPUNIT_ASSERT(region.maxS() == 1.0);
    }

    // .   .   .   .   .
    //     +---+
    //       +=========+
    void testComputeUnionSimple() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeUnion(Region(0.375, 1.0, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.25, 1.0, id1, id4, false, false));
    }

    // .   .   .   .   .
    //             +---+
    //     +===+
    void testComputeUnionDisjointLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.75, 1.0, id1, id2, false, false);
        region.computeUnion(Region(0.25, 0.5, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 2);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.25, 0.5, id3, id4, false, false));
        CPPUNIT_ASSERT(region.span(1) == Region::Span(0.75, 1.0, id1, id2, false, false));
    }

    // .   .   .   .   .
    //     +---+
    //             +===+
    void testComputeUnionDisjointRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeUnion(Region(0.75, 1.0, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 2);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.25, 0.5, id1, id2, false, false));
        CPPUNIT_ASSERT(region.span(1) == Region::Span(0.75, 1.0, id3, id4, false, false));
    }

    // .   .   .   .   .
    //     +---+
    // +===+
    void testComputeUnionSharedPointLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeUnion(Region(0.0, 0.25, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.0, 0.5, id3, id2, false, false));
    }

    // .   .   .   .   .
    //     +---+
    //         +===+
    void testComputeUnionSharedPointRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeUnion(Region(0.5, 0.75, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.25, 0.75, id1, id4, false, false));
    }

    // .   .   .   .   .
    //     +---+
    // +=======+
    void testComputeUnionSharedPointLeftOverlap() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeUnion(Region(0.0, 0.5, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.0, 0.5, id3, id2, false, false));
    }

    // .   .   .   .   .
    //     +---+
    //     +=======+
    void testComputeUnionSharedPointRightOverlap() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeUnion(Region(0.25, 0.75, id3, id4, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.25, 0.75, id1, id4, false, false));
    }

    // .   .   .   .   .
    //     +---+   +---+
    //       +=======+
    void testComputeUnionOverlapTwo() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.appendSpan(Region::Span(0.75, 1.0, id3, id4, false, false));
        region.computeUnion(Region(0.375, 0.875, id5, id6, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.25, 1.0, id1, id4, false, false));
    }

    // .   .   .   .   .
    // +-+ +---+   +---+
    //   +===========+
    void testComputeUnionOverlapThree() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id7 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id8 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.0, 0.125, id1, id2, false, false);
        region.appendSpan(Region::Span(0.25, 0.5, id3, id4, false, false));
        region.appendSpan(Region::Span(0.75, 1.0, id5, id6, false, false));
        region.computeUnion(Region(0.125, 0.9, id7, id8, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.0, 1.0, id1, id6, false, false));
    }

    // .   .   .   .   .
    //   +-+ +-+ +-+
    // +===============+
    void testComputeUnionFullOverlapThree() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id7 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id8 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.125, 0.25, id1, id2, false, false);
        region.appendSpan(Region::Span(0.375, 0.5, id3, id4, false, false));
        region.appendSpan(Region::Span(0.625, 0.75, id5, id6, false, false));
        region.computeUnion(Region(0.0, 1.0, id7, id8, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.0, 1.0, id7, id8, false, false));
    }

    // .   .   .   .   .
    //     
    //         +===+
    void testComputeUnionLhsEmpty() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        Region region;
        region.computeUnion(Region(0.5, 0.75, id1, id2, false, false));
        CPPUNIT_ASSERT(region.spanCount() == 1);
        CPPUNIT_ASSERT(region.span(0) == Region::Span(0.5, 0.75, id1, id2, false, false));
    }

    // .   .   .   .   .
    //         +-------+
    //     +=======+
    void testDifferenceLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.5, 1.0, id1, id2, false, false);
        Region rhs(0.25, 0.75, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.75, 1.0, EndpointIdentifier::createUndefined(), id2, 
                    false, false)));
    }

    // .   .   .   .   .
    // +-------+
    //     +=======+
    void testDifferenceRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 0.5, id1, id2, false, false);
        Region rhs(0.25, 0.75, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.0, 0.25, id1, EndpointIdentifier::createUndefined(),
                    false, false)));
    }

    // .   .   .   .   .
    // +---------------+
    //     +=======+
    void testDifferenceCenter() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 1.0, id1, id2, false, false);
        Region rhs(0.25, 0.75, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 2);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.0, 0.25, id1, EndpointIdentifier::createUndefined(), 
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(1),
                Region::Span(0.75, 1.0, EndpointIdentifier::createUndefined(), id2, 
                    false, false)));
    }

    // .   .   .   .   .
    //     +-------+
    // +===============+
    void testDifferenceEmpty() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.25, 0.75, id1, id2, false, false);
        Region rhs(0.0, 1.0, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 0);
    }

    // .   .   .   .   .
    //         +-------+
    // +===+
    void testDifferenceDisjointLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.5, 1.0, id1, id2, false, false);
        Region rhs(0.0, 0.25, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(lhs.span(0) == Region::Span(0.5, 1.0, id1, id2, false, false));
    }

    // .   .   .   .   .
    // +-------+
    //             +===+
    void testDifferenceDisjointRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 0.5, id1, id2, false, false);
        Region rhs(0.75, 1.0, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(lhs.span(0) == Region::Span(0.0, 0.5, id1, id2, false, false));
    }

    // .   .   .   .   .
    //         +-------+
    // +=======+
    void testDifferenceSharedPointLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.5, 1.0, id1, id2, false, false);
        Region rhs(0.0, 0.5, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(lhs.span(0) == Region::Span(0.5, 1.0, id1, id2, false, false));
    }

    // .   .   .   .   .
    // +-------+
    //         +=======+
    void testDifferenceSharedPointRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 0.5, id1, id2, false, false);
        Region rhs(0.5, 1.0, id3, id4, false, false);
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(lhs.span(0) == Region::Span(0.0, 0.5, id1, id2, false, false));
    }

    // .   .   .   .   .
    // +---------------+
    // +===+   +===+
    void testDifferenceTwoLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 1.0, id1, id2, false, false);
        Region rhs(0.0, 0.25, id3, id4, false, false);
        rhs.appendSpan(Region::Span(0.5, 0.75, id5, id6, false, false));
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 2);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.25, 0.5, EndpointIdentifier::createUndefined(),
                    EndpointIdentifier::createUndefined(), 
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(1),
                Region::Span(0.75, 1.0, EndpointIdentifier::createUndefined(), id2, 
                    false, false)));
    }

    // .   .   .   .   .
    // +---------------+
    //     +===+   +===+
    void testDifferenceTwoRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 1.0, id1, id2, false, false);
        Region rhs(0.25, 0.5, id3, id4, false, false);
        rhs.appendSpan(Region::Span(0.75, 1.0, id5, id6, false, false));
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 2);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.0, 0.25, id1, EndpointIdentifier::createUndefined(),
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(1),
                Region::Span(0.5, 0.75, EndpointIdentifier::createUndefined(),
                    EndpointIdentifier::createUndefined(), 
                    false, false)));
    }

    // .   .   .   .   .
    //   +-------------+
    // +===+   +=+ +=+
    void testDifferenceThreeLeft() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id7 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id8 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.125, 1.0, id1, id2, false, false);
        Region rhs(0.0, 0.25, id3, id4, false, false);
        rhs.appendSpan(Region::Span(0.5, 0.625, id5, id6, false, false));
        rhs.appendSpan(Region::Span(0.75, 0.875, id7, id8, false, false));
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 3);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.25, 0.5, EndpointIdentifier::createUndefined(),
                    EndpointIdentifier::createUndefined(), 
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(1),
                Region::Span(0.625, 0.75, EndpointIdentifier::createUndefined(),
                    EndpointIdentifier::createUndefined(), 
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(2),
                Region::Span(0.875, 1.0, EndpointIdentifier::createUndefined(), id2, 
                    false, false)));
    }

    // .   .   .   .   .
    // +-------------+
    //   +=+ +=+   +===+
    void testDifferenceThreeRight() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id5 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id6 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id7 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id8 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 0.875, id1, id2, false, false);
        Region rhs(0.125, 0.25, id3, id4, false, false);
        rhs.appendSpan(Region::Span(0.375, 0.5, id5, id6, false, false));
        rhs.appendSpan(Region::Span(0.75, 1.0, id7, id8, false, false));
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 3);
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(0),
                Region::Span(0.0, 0.125, id1, EndpointIdentifier::createUndefined(),
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(1),
                Region::Span(0.25, 0.375, EndpointIdentifier::createUndefined(),
                    EndpointIdentifier::createUndefined(), 
                    false, false)));
        CPPUNIT_ASSERT(spansAreEqual(lhs.span(2),
                Region::Span(0.5, 0.75, EndpointIdentifier::createUndefined(),
                    EndpointIdentifier::createUndefined(), 
                    false, false)));
    }

    // .   .   .   .   .
    // +---+
    // 
    void testDifferenceRhsEmpty() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        Region lhs(0.0, 0.25, id1, id2, false, false);
        Region rhs;
        lhs.computeDifference(rhs, &mWedgeIntersector);
        CPPUNIT_ASSERT(lhs.spanCount() == 1);
        CPPUNIT_ASSERT(lhs.span(0) == Region::Span(0.0, 0.25, id1, id2, false, false));
    }

    // .   .   .   .   .
    //     +---+
    // +=======+
    void testDifferenceSharedPointLeftOverlap() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeDifference(Region(0.0, 0.5, id3, id4, false, false),
            &mWedgeIntersector);
        CPPUNIT_ASSERT(region.spanCount() == 0);
    }

    // .   .   .   .   .
    //     +---+
    //     +=======+
    void testDifferenceSharedPointRightOverlap() {
        EndpointIdentifier id1 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id2 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id3 = EndpointIdentifier::createUniqueIdentifier();
        EndpointIdentifier id4 = EndpointIdentifier::createUniqueIdentifier();
        Region region(0.25, 0.5, id1, id2, false, false);
        region.computeDifference(Region(0.25, 0.75, id3, id4, false, false),
            &mWedgeIntersector);
        CPPUNIT_ASSERT(region.spanCount() == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(RegionTest);
