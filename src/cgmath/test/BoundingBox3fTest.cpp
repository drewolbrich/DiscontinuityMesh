// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/BoundingBox3f.h>
#include <cgmath/BoundingBox3fOperations.h>

using cgmath::BoundingBox3f;
using cgmath::Vector3f;

class BoundingBox3fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BoundingBox3fTest);
    CPPUNIT_TEST(testElementAccess);
    CPPUNIT_TEST(testEquality);
    CPPUNIT_TEST(testElementAssignment);
    CPPUNIT_TEST(testReset);
    CPPUNIT_TEST(testExtendByVector3f);
    CPPUNIT_TEST(testExtendByBoundingBox3f);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testCenter);
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

    void testElementAccess() {
        CPPUNIT_ASSERT(mBBox2(0, 0) == -10);
        CPPUNIT_ASSERT(mBBox2(0, 1) == -12);
        CPPUNIT_ASSERT(mBBox2(0, 2) == -14);
        CPPUNIT_ASSERT(mBBox2(1, 0) == 10);
        CPPUNIT_ASSERT(mBBox2(1, 1) == 12);
        CPPUNIT_ASSERT(mBBox2(1, 2) == 14);
        
        CPPUNIT_ASSERT(mBBox2.minAxis(0) == -10);
        CPPUNIT_ASSERT(mBBox2.minAxis(1) == -12);
        CPPUNIT_ASSERT(mBBox2.minAxis(2) == -14);
        CPPUNIT_ASSERT(mBBox2.maxAxis(0) == 10);
        CPPUNIT_ASSERT(mBBox2.maxAxis(1) == 12);
        CPPUNIT_ASSERT(mBBox2.maxAxis(2) == 14);
        
        CPPUNIT_ASSERT(mBBox1.minX() == mBBox2.minX());
        CPPUNIT_ASSERT(mBBox1.maxX() == mBBox2.maxX());
        
        CPPUNIT_ASSERT(mBBox1.minY() == mBBox2.minY());
        CPPUNIT_ASSERT(mBBox1.maxY() == mBBox2.maxY());
        
        CPPUNIT_ASSERT(mBBox1.minZ() == mBBox2.minZ());
        CPPUNIT_ASSERT(mBBox1.maxZ() == mBBox2.maxZ());
        
        CPPUNIT_ASSERT(mBBox1.min() == mBBox2.min());
        CPPUNIT_ASSERT(mBBox1.max() == mBBox2.max());
        
        CPPUNIT_ASSERT(mBBox1.minX() == -10);
        CPPUNIT_ASSERT(mBBox1.maxX() == 10);
        
        CPPUNIT_ASSERT(mBBox1.minY() == -12);
        CPPUNIT_ASSERT(mBBox1.maxY() == 12);
        
        CPPUNIT_ASSERT(mBBox1.minZ() == -14);
        CPPUNIT_ASSERT(mBBox1.maxZ() == 14);
        
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(-10, -12, -14));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(10, 12, 14));
    }

    void testEquality() {
        CPPUNIT_ASSERT(mBBox1 == mBBox2);

        BoundingBox3f bbox3(-13, 13, -13, 13, -13, 13);
        CPPUNIT_ASSERT(mBBox1 != bbox3);
    }

    void testElementAssignment() {
        mBBox1.setMinX(-20);
        mBBox1.setMaxX(20);
        
        mBBox1.setMinY(-22);
        mBBox1.setMaxY(22);
        
        mBBox1.setMinZ(-24);
        mBBox1.setMaxZ(24);
        
        mBBox2.setMin(Vector3f(-20, -22, -24));
        mBBox2.setMax(Vector3f(20, 22, 24));
        
        CPPUNIT_ASSERT(mBBox1.min() == mBBox2.min());
        CPPUNIT_ASSERT(mBBox1.max() == mBBox2.max());
    }

    void testReset() {
        mBBox1.reset();
        
        CPPUNIT_ASSERT(mBBox1.empty());
    }

    void testExtendByVector3f() {
        mBBox1.reset();

        mBBox1.extendByVector3f(Vector3f(100, 200, 300));
        CPPUNIT_ASSERT(!mBBox1.empty());
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(100, 200, 300));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(100, 200, 300));
        
        mBBox1.extendByVector3f(Vector3f(-100, -200, -300));
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(-100, -200, -300));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(100, 200, 300));
        
        mBBox1.extendByVector3f(Vector3f(-500, -500, -500));
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(-500, -500, -500));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(100, 200, 300));
    }

    void testExtendByBoundingBox3f() {
        mBBox1.reset();

        mBBox1.extendByBoundingBox3f(
            BoundingBox3f(Vector3f(50, 100, 150), Vector3f(100, 200, 300)));
        CPPUNIT_ASSERT(!mBBox1.empty());
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(50, 100, 150));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(100, 200, 300));
        
        mBBox1.extendByBoundingBox3f(
            BoundingBox3f(Vector3f(-100, -200, -300), Vector3f(-50, -150, -200)));
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(-100, -200, -300));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(100, 200, 300));
        
        mBBox1.extendByBoundingBox3f(
            BoundingBox3f(Vector3f(-500, -500, -500), Vector3f(-400, -400, -400)));
        CPPUNIT_ASSERT(mBBox1.min() == Vector3f(-500, -500, -500));
        CPPUNIT_ASSERT(mBBox1.max() == Vector3f(100, 200, 300));
    }

    void testSize() {
        CPPUNIT_ASSERT(mBBox1.sizeX() == 20.0);
        CPPUNIT_ASSERT(mBBox1.sizeY() == 24.0);
        CPPUNIT_ASSERT(mBBox1.sizeZ() == 28.0);
        CPPUNIT_ASSERT(mBBox1.size() == Vector3f(20.0, 24.0, 28.0));
    }

    void testCenter() {
        mBBox1 = BoundingBox3f(0, 10, 0, 12, 0, 14);
        CPPUNIT_ASSERT(mBBox1.center() == Vector3f(5, 6, 7));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoundingBox3fTest);
