// Copyright 2009 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/TriangleOperations.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Tolerance.h>

using cgmath::GetTriangleArea;
using cgmath::GetIntersectionOfTriangleWithCoplanarLineSegment;
using cgmath::Vector3f;
using cgmath::TOLERANCE;

class TriangleOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TriangleOperationsTest);
    CPPUNIT_TEST(testGetTriangleArea);
    CPPUNIT_TEST(testGetIntersectionOfTriangleWithCoplanarLineSegment);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testGetTriangleArea() {
        CPPUNIT_ASSERT(GetTriangleArea(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0)) == 50.0);
    }

    void testGetIntersectionOfTriangleWithCoplanarLineSegment() {
        Vector3f r0;
        Vector3f r1;

        int clippingEdgeIndex0 = -1;
        int clippingEdgeIndex1 = -1;

        // Degenerate triangle.
        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(20.0, 0.0, 0.0),
                           Vector3f(2.0, 0.0, 0.0),
                           Vector3f(0.0, 3.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        // Line segment not in plane of triangle.
        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, 5.0),
                           Vector3f(0.0, 3.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, 0.0),
                           Vector3f(0.0, 3.0, 5.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, -5.0),
                           Vector3f(0.0, 3.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, 0.0),
                           Vector3f(0.0, 3.0, -5.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, 5.0),
                           Vector3f(0.0, 3.0, 5.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, -5.0),
                           Vector3f(0.0, 3.0, -5.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, -5.0),
                           Vector3f(0.0, 3.0, 5.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        CPPUNIT_ASSERT(!GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 0.0, 5.0),
                           Vector3f(0.0, 3.0, -5.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        // Line segment that intersects the triangle without any clipping.
        CPPUNIT_ASSERT(GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 1.0, 0.0),
                           Vector3f(1.0, 3.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(r0.equivalent(Vector3f(2.0, 1.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(r1.equivalent(Vector3f(1.0, 3.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == -1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == -1);

        // Clipped line segments.
        CPPUNIT_ASSERT(GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, -2.0, 0.0),
                           Vector3f(2.0, 50.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(r0.equivalent(Vector3f(2.0, 0.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(r1.equivalent(Vector3f(2.0, 8.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == 0);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == 1);

        CPPUNIT_ASSERT(GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(2.0, 50.0, 0.0),
                           Vector3f(2.0, -2.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(r0.equivalent(Vector3f(2.0, 8.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(r1.equivalent(Vector3f(2.0, 0.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == 1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == 0);

        CPPUNIT_ASSERT(GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(50.0, 2.0, 0.0),
                           Vector3f(-2.0, 2.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(r0.equivalent(Vector3f(8.0, 2.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(r1.equivalent(Vector3f(0.0, 2.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == 1);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == 2);

        CPPUNIT_ASSERT(GetIntersectionOfTriangleWithCoplanarLineSegment(
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(0.0, 10.0, 0.0),
                           Vector3f(-5.0, -5.0, 0.0),
                           Vector3f(50.0, 50.0, 0.0), TOLERANCE, TOLERANCE, &r0, &r1,
                           &clippingEdgeIndex0, &clippingEdgeIndex1));
        CPPUNIT_ASSERT(r0.equivalent(Vector3f(0.0, 0.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(r1.equivalent(Vector3f(5.0, 5.0, 0.0), TOLERANCE));
        CPPUNIT_ASSERT(clippingEdgeIndex0 == 0 || clippingEdgeIndex0 == 2);
        CPPUNIT_ASSERT(clippingEdgeIndex1 == 1);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TriangleOperationsTest);
