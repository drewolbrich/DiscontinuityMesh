// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cmath>

#include <cgmath/BoundingBox3fOperations.h>
#include <cgmath/BoundingBox3f.h>

using cgmath::BoundingBox3fIntersectsPlane;
using cgmath::BoundingBox3f;
using cgmath::Vector3f;

class BoundingBox3fOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BoundingBox3fOperationsTest);
    CPPUNIT_TEST(testBoundingBox3fIntersectsBoundingBox3f);
    CPPUNIT_TEST(testBoundingBox3fIntersectsRaySegment);
    CPPUNIT_TEST(testBoundingBox3fIntersectsRaySegmentWithT);
    CPPUNIT_TEST(testBoundingBox3fIntersectsPlaneSuccess);
    CPPUNIT_TEST(testBoundingBox3fIntersectsPlaneFailure);
    CPPUNIT_TEST(testBoundingBox3fIntersectsTriangleSuccess);
    CPPUNIT_TEST(testBoundingBox3fIntersectsTriangleFailure);
    CPPUNIT_TEST(testBoundingBox3fIntersectsTetrahedron);
    CPPUNIT_TEST_SUITE_END();

public:
    BoundingBox3f mBBox1;
    BoundingBox3f mBBox2;

    void setUp() {
        mBBox1 = BoundingBox3f(-10, 10, -12, 12, -14, 14);
        mBBox2 = BoundingBox3f(Vector3f(-10, -12, -14), Vector3f(10, 12, 14));
    }

    void tearDown() {
    }

    void testBoundingBox3fIntersectsBoundingBox3f() {
        mBBox1 = BoundingBox3f(-1, 1, -1, 1, -1, 1);
        mBBox2 = BoundingBox3f(-2, 2, -2, 2, -2, 2);
        CPPUNIT_ASSERT(BoundingBox3fIntersectsBoundingBox3f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsBoundingBox3f(mBBox2, mBBox1));

        mBBox1 = BoundingBox3f(-1, 1, -1, 1, -1, 1);
        mBBox2 = BoundingBox3f(0, 2, 0, 2, 0, 2);
        CPPUNIT_ASSERT(BoundingBox3fIntersectsBoundingBox3f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsBoundingBox3f(mBBox2, mBBox1));

        mBBox1 = BoundingBox3f(-1, 1, -1, 1, -1, 1);
        mBBox2 = BoundingBox3f(-2, 0, -2, 0, -2, 0);
        CPPUNIT_ASSERT(BoundingBox3fIntersectsBoundingBox3f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsBoundingBox3f(mBBox2, mBBox1));

        mBBox1 = BoundingBox3f(-1, 1, -1, 1, -1, 1);
        mBBox2 = BoundingBox3f(-3, -2, -3, -2, -3, -2);
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsBoundingBox3f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsBoundingBox3f(mBBox2, mBBox1));

        mBBox1 = BoundingBox3f(-1, 1, -1, 1, -1, 1);
        mBBox2 = BoundingBox3f(2, 3, 2, 3, 2, 3);
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsBoundingBox3f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsBoundingBox3f(mBBox2, mBBox1));
    }

    void testBoundingBox3fIntersectsRaySegment() {
        mBBox1 = BoundingBox3f(-5, 10, -6, 12, -7, 14);
        
        CPPUNIT_ASSERT(BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(0, 0, 0), Vector3f(100, 100, 100)));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(100, 100, 100), Vector3f(0, 0, 0)));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(100, 100, 100), Vector3f(-100, -100, -100)));
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(100, 100, 100), Vector3f(50, 50, 50)));
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(100, 100, 100), Vector3f(200, 200, 200)));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(8, 10, 100), Vector3f(8, 10, 0)));
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(8, 10, 100), Vector3f(8, 10, 50)));
        CPPUNIT_ASSERT(BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(8, 10, -100), Vector3f(8, 10, 0)));
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsRaySegment(mBBox1, 
                           Vector3f(8, 10, -100), Vector3f(8, 10, -50)));
    }

    void testBoundingBox3fIntersectsRaySegmentWithT() {
        mBBox1 = BoundingBox3f(5, 10, 15, 20, 25, 30);

        float t = 0.0;

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(0, 17, 27), Vector3f(100, 17, 27), &t);
        CPPUNIT_ASSERT(fabs(t - 0.05) < 0.0001);

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(100, 17, 27), Vector3f(0, 17, 27), &t);
        CPPUNIT_ASSERT(fabs(t - 0.9) < 0.0001);

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(7, 0, 27), Vector3f(7, 100, 27), &t);
        CPPUNIT_ASSERT(fabs(t - 0.15) < 0.0001);

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(7, 100, 27), Vector3f(7, 0, 27), &t);
        CPPUNIT_ASSERT(fabs(t - 0.8) < 0.0001);

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(7, 17, 0), Vector3f(7, 17, 100), &t);
        CPPUNIT_ASSERT(fabs(t - 0.25) < 0.0001);

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(7, 17, 100), Vector3f(7, 17, 0), &t);
        CPPUNIT_ASSERT(fabs(t - 0.7) < 0.0001);

        BoundingBox3fIntersectsRaySegment(mBBox1, 
            Vector3f(7, 17, 27), Vector3f(107, 17, 27), &t);
        CPPUNIT_ASSERT(fabs(t - 0.0) < 0.0001);
    }

    void testBoundingBox3fIntersectsPlaneSuccess() {
        BoundingBox3f bbox(2.0, 4.0, 10.0, 12.0, 20.0, 24.0);

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(3.0, 0.0, 0.0), cgmath::Vector3f(1.0, 0.0, 0.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(0.0, 11.0, 0.0), cgmath::Vector3f(0.0, 1.0, 0.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(0.0, 0.0, 21.0), cgmath::Vector3f(0.0, 0.0, 1.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(3.0, 11.0, 21.0), cgmath::Vector3f(1.0, 1.0, 1.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(3.0, 11.0, 21.0), cgmath::Vector3f(-1.0, 1.0, 1.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(3.0, 11.0, 21.0), cgmath::Vector3f(1.0, -1.0, 1.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(3.0, 11.0, 21.0), cgmath::Vector3f(1.0, 1.0, -1.0)));
    }

    void testBoundingBox3fIntersectsPlaneFailure() {
        BoundingBox3f bbox(2.0, 4.0, 10.0, 12.0, 20.0, 24.0);

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(1.0, 0.0, 0.0), cgmath::Vector3f(1.0, 0.0, 0.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(5.0, 0.0, 0.0), cgmath::Vector3f(1.0, 0.0, 0.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(0.0, 9.0, 0.0), cgmath::Vector3f(0.0, 1.0, 0.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(0.0, 13.0, 0.0), cgmath::Vector3f(0.0, 1.0, 0.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(0.0, 0.0, 19.0), cgmath::Vector3f(0.0, 0.0, 1.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(0.0, 0.0, 25.0), cgmath::Vector3f(0.0, 0.0, 1.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(1.0, 0.0, 0.0), cgmath::Vector3f(-1.0, 0.0, 0.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(5.0, 0.0, 0.0), cgmath::Vector3f(-1.0, 0.0, 0.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(5.0, 13.0, 25.0), cgmath::Vector3f(1.0, 1.0, 1.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsPlane(bbox, 
                cgmath::Vector3f(1.0, 9.0, 19.0), cgmath::Vector3f(1.0, 1.0, 1.0)));
    }

    void testBoundingBox3fIntersectsTriangleSuccess() {
        BoundingBox3f bbox(2.0, 4.0, 10.0, 12.0, 20.0, 24.0);

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTriangle(bbox,
                Vector3f(2.0, 10.0, 20.0),
                Vector3f(4.0, 12.0, 24.0),
                Vector3f(3.0, 10.0, 20.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTriangle(bbox,
                Vector3f(0.0, 10.0, 20.0),
                Vector3f(6.0, 12.0, 24.0),
                Vector3f(3.0, 10.0, 20.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTriangle(bbox,
                Vector3f(0.0, 14.0, 22.0),
                Vector3f(6.0, 14.0, 22.0),
                Vector3f(0.0, 9.0, 22.0)));
    }

    void testBoundingBox3fIntersectsTriangleFailure() {
        BoundingBox3f bbox(2.0, 4.0, 10.0, 12.0, 20.0, 24.0);
        
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTriangle(bbox,
                Vector3f(0.0, 14.0, 19.0),
                Vector3f(6.0, 14.0, 19.0),
                Vector3f(0.0, 9.0, 19.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTriangle(bbox,
                Vector3f(0.0, 14.0, 25.0),
                Vector3f(6.0, 14.0, 25.0),
                Vector3f(0.0, 9.0, 25.0)));
    }

    void testBoundingBox3fIntersectsTetrahedron() {
        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(2.0, 3.0, 2.0, 3.0, 2.0, 3.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 0.0, 0.0),
                           Vector3f(0.0, 12.0, 0.0),
                           Vector3f(0.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(5.0, 6.0, 5.0, 6.0, 5.0, 6.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 12.0, 0.0),
                           Vector3f(0.0, 12.0, 12.0),
                           Vector3f(12.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(-5.0, -6.0, -5.0, -6.0, -5.0, -6.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 12.0, 0.0),
                           Vector3f(0.0, 12.0, 12.0),
                           Vector3f(12.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(10.0, 11.0, 0.0, 1.0, 0.0, 1.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 12.0, 0.0),
                           Vector3f(0.0, 12.0, 12.0),
                           Vector3f(12.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(0.0, 1.0, 0.0, 1.0, 0.0, 1.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 12.0, 0.0),
                           Vector3f(0.0, 12.0, 12.0),
                           Vector3f(12.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(-1.0, 13.0, -1.0, 13.0, -1.0, 13.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 12.0, 0.0),
                           Vector3f(0.0, 12.0, 12.0),
                           Vector3f(12.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(5.0, 6.0, 5.0, 6.0, 13.0, 14.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(12.0, 12.0, 0.0),
                           Vector3f(0.0, 12.0, 12.0),
                           Vector3f(12.0, 0.0, 12.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(5.0, 6.0, 5.0, 6.0, 10.0, 11.0),
                           Vector3f(5.0, 5.0, 5.0),
                           Vector3f(0.0, 5.0, 10.0),
                           Vector3f(0.0, 0.0, 10.0),
                           Vector3f(10.0, 10.0, 10.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(5.0, 6.0, 5.0, 6.0, 10.1, 11.0),
                           Vector3f(5.0, 5.0, 0.0),
                           Vector3f(0.0, 5.0, 10.0),
                           Vector3f(0.0, 0.0, 10.0),
                           Vector3f(10.0, 10.0, 10.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(4.5, 5.5, 4.5, 5.5, 0.0, 1.0),
                           Vector3f(5.0, 5.0, 0.0),
                           Vector3f(0.0, 5.0, 10.0),
                           Vector3f(0.0, 0.0, 10.0),
                           Vector3f(10.0, 10.0, 10.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(0.0, 1.0, 4.5, 5.5, 0.0, 1.0),
                           Vector3f(5.0, 5.0, 0.0),
                           Vector3f(0.0, 5.0, 10.0),
                           Vector3f(0.0, 0.0, 10.0),
                           Vector3f(10.0, 10.0, 10.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(4.5, 5.5, 4.5, 5.5, -2.0, 14.0),
                           Vector3f(5.0, 5.0, 0.0),
                           Vector3f(0.0, 5.0, 10.0),
                           Vector3f(0.0, 0.0, 10.0),
                           Vector3f(10.0, 10.0, 10.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(4.5, 5.5, -1.1, -0.1, 4.0, 5.0),
                           Vector3f(5.0, 0.0, 0.0),
                           Vector3f(5.0, 0.0, 10.0),
                           Vector3f(0.0, 10.0, 5.0),
                           Vector3f(10.0, 10.0, 5.0)));

        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(0.0, 4.9, 0.0, 4.9, 0, 10.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 10.0),
                           Vector3f(0.0, 10.0, 5.0),
                           Vector3f(20.0, 10.0, 5.0)));

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(0.0, 5.1, 0.0, 5.1, 0, 10.0),
                           Vector3f(10.0, 0.0, 0.0),
                           Vector3f(10.0, 0.0, 10.0),
                           Vector3f(0.0, 10.0, 5.0),
                           Vector3f(20.0, 10.0, 5.0)));

#if 0
        CPPUNIT_ASSERT(!BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(-2.0, 2.0, 4.0, 8.0, -2.0, 2.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(6.0, 0.0, 0.0),
                           Vector3f(6.0, 6.0, 0.0),
                           Vector3f(6.0, 0.0, 6.0)));
#endif

        CPPUNIT_ASSERT(BoundingBox3fIntersectsTetrahedron(
                           BoundingBox3f(0.0, 4.0, 2.0, 6.0, -2.0, 2.0),
                           Vector3f(0.0, 0.0, 0.0),
                           Vector3f(6.0, 0.0, 0.0),
                           Vector3f(6.0, 6.0, 0.0),
                           Vector3f(6.0, 0.0, 6.0)));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoundingBox3fOperationsTest);
