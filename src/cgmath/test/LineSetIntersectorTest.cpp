// Copyright 2008 Retroactive Fiasco.

#include <cassert>
#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/LineSetIntersector.h>
#include <cgmath/Vector2f.h>

using cgmath::LineSetIntersector;
using cgmath::Vector2f;

class LineSetIntersectorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(LineSetIntersectorTest);
    CPPUNIT_TEST(testFailure);
    CPPUNIT_TEST(testOneIntersection);
    CPPUNIT_TEST(testTwoIntersections);
    CPPUNIT_TEST(testVerticalLines);
    CPPUNIT_TEST(testHorizontalLines);
    CPPUNIT_TEST(testIntersectionAtEndpoint);
    CPPUNIT_TEST(testCommonIntersectionOfThreeLines);
    CPPUNIT_TEST(testCommonIntersectionOfFiveLines);
    CPPUNIT_TEST(testThirdLineEndPointHasSameXValueAsIntersection);
    CPPUNIT_TEST(testThirdLineEndPointHasSameYValueAsIntersection);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testFailure() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(3, 4), Vector2f(11, 12)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(5, 7), Vector2f(14, 19)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(!foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 0);
    }

    void testOneIntersection() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(5, 0), Vector2f(-1, 4)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mLineSegmentIndexVector[0] == 0
                && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
            || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                && intersectionVector[0].mLineSegmentIndexVector[0] == 1));
    }

    void testTwoIntersections() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(5, 0), Vector2f(-1, 4)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(6, 1), Vector2f(0, 5)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mPoint == Vector2f(2, 2)
                && intersectionVector[1].mPoint == Vector2f(3, 3))
            || (intersectionVector[0].mPoint == Vector2f(3, 3)
                && intersectionVector[1].mPoint == Vector2f(2, 2)));
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(intersectionVector[1].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (((intersectionVector[0].mLineSegmentIndexVector[0] == 0
                    && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
                || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                    && intersectionVector[0].mLineSegmentIndexVector[0] == 1))
                && ((intersectionVector[1].mLineSegmentIndexVector[0] == 0
                        && intersectionVector[1].mLineSegmentIndexVector[1] == 2)
                    || (intersectionVector[1].mLineSegmentIndexVector[1] == 0
                        && intersectionVector[1].mLineSegmentIndexVector[0] == 2)))
            || (((intersectionVector[1].mLineSegmentIndexVector[0] == 0
                        && intersectionVector[1].mLineSegmentIndexVector[1] == 1)
                    || (intersectionVector[1].mLineSegmentIndexVector[1] == 0
                        && intersectionVector[1].mLineSegmentIndexVector[0] == 1))
                && ((intersectionVector[0].mLineSegmentIndexVector[0] == 0
                        && intersectionVector[0].mLineSegmentIndexVector[1] == 2)
                    || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                        && intersectionVector[0].mLineSegmentIndexVector[0] == 2)))
            );
    }

    void testVerticalLines() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(2, 0), Vector2f(2, 7)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mLineSegmentIndexVector[0] == 0
                && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
            || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                && intersectionVector[0].mLineSegmentIndexVector[0] == 1));
    }

    void testHorizontalLines() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 2), Vector2f(7, 2)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mLineSegmentIndexVector[0] == 0
                && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
            || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                && intersectionVector[0].mLineSegmentIndexVector[0] == 1));
    }

    void testIntersectionAtEndpoint() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(2, 2), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(2, 2), Vector2f(7, 2)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mLineSegmentIndexVector[0] == 0
                && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
            || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                && intersectionVector[0].mLineSegmentIndexVector[0] == 1));
    }

    void testCommonIntersectionOfThreeLines() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(2, 1), Vector2f(2, 3)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(1, 2), Vector2f(3, 2)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 3);
        for (size_t index = 0; index < 3; ++index) {
            assert(std::find(intersectionVector[0].mLineSegmentIndexVector.begin(),
                    intersectionVector[0].mLineSegmentIndexVector.end(), index)
                != intersectionVector[0].mLineSegmentIndexVector.end());
        }
    }

    void testCommonIntersectionOfFiveLines() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(2, 1), Vector2f(2, 3)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(1, 2), Vector2f(3, 2)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(4, 3), Vector2f(0, 1)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(5, 3), Vector2f(-1, 1)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 5);
        for (size_t index = 0; index < 5; ++index) {
            assert(std::find(intersectionVector[0].mLineSegmentIndexVector.begin(),
                    intersectionVector[0].mLineSegmentIndexVector.end(), index)
                != intersectionVector[0].mLineSegmentIndexVector.end());
        }
    }

    void testThirdLineEndPointHasSameXValueAsIntersection() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(5, 0), Vector2f(-1, 4)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(2, -1), Vector2f(40, -1)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mLineSegmentIndexVector[0] == 0
                && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
            || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                && intersectionVector[0].mLineSegmentIndexVector[0] == 1));
    }

    void testThirdLineEndPointHasSameYValueAsIntersection() {
        LineSetIntersector lineSetIntersector;
        LineSetIntersector::LineSegmentVector lineSegmentVector;
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(0, 0), Vector2f(10, 10)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(5, 0), Vector2f(-1, 4)));
        lineSegmentVector.push_back(
            LineSetIntersector::LineSegment(Vector2f(-2, 2), Vector2f(-2, 40)));
        lineSetIntersector.setLineSegmentVector(&lineSegmentVector);
        bool foundIntersections = lineSetIntersector.findIntersections();
        CPPUNIT_ASSERT(foundIntersections);
        const LineSetIntersector::IntersectionVector &intersectionVector
            = lineSetIntersector.getIntersectionVector();
        CPPUNIT_ASSERT(intersectionVector.size() == 1);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[0] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mPoint[1] == 2);
        CPPUNIT_ASSERT(intersectionVector[0].mLineSegmentIndexVector.size() == 2);
        CPPUNIT_ASSERT(
            (intersectionVector[0].mLineSegmentIndexVector[0] == 0
                && intersectionVector[0].mLineSegmentIndexVector[1] == 1)
            || (intersectionVector[0].mLineSegmentIndexVector[1] == 0
                && intersectionVector[0].mLineSegmentIndexVector[0] == 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LineSetIntersectorTest);
