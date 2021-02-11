// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/BoundingBox2f.h>

using cgmath::BoundingBox2f;
using cgmath::Vector2f;

class BoundingBox2fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BoundingBox2fTest);
    CPPUNIT_TEST(testElementAccess);
    CPPUNIT_TEST(testEquality);
    CPPUNIT_TEST(testElementAssignment);
    CPPUNIT_TEST(testReset);
    CPPUNIT_TEST(testExtendByVector2f);
    CPPUNIT_TEST(testExtendByBoundingBox2f);
    CPPUNIT_TEST(testSize);
    CPPUNIT_TEST(testCenter);
    CPPUNIT_TEST_SUITE_END();

public:
    BoundingBox2f mBBox1;
    BoundingBox2f mBBox2;

    void setUp() {
        mBBox1 = BoundingBox2f(-10, 10, -12, 12);
        mBBox2 = BoundingBox2f(Vector2f(-10, -12), Vector2f(10, 12));
    }

    void tearDown() {
    }

    void testElementAccess() {
        CPPUNIT_ASSERT(mBBox2(0, 0) == -10);
        CPPUNIT_ASSERT(mBBox2(0, 1) == -12);
        CPPUNIT_ASSERT(mBBox2(1, 0) == 10);
        CPPUNIT_ASSERT(mBBox2(1, 1) == 12);
        
        CPPUNIT_ASSERT(mBBox2.minAxis(0) == -10);
        CPPUNIT_ASSERT(mBBox2.minAxis(1) == -12);
        CPPUNIT_ASSERT(mBBox2.maxAxis(0) == 10);
        CPPUNIT_ASSERT(mBBox2.maxAxis(1) == 12);
        
        CPPUNIT_ASSERT(mBBox1.minX() == mBBox2.minX());
        CPPUNIT_ASSERT(mBBox1.maxX() == mBBox2.maxX());
        
        CPPUNIT_ASSERT(mBBox1.minY() == mBBox2.minY());
        CPPUNIT_ASSERT(mBBox1.maxY() == mBBox2.maxY());
        
        CPPUNIT_ASSERT(mBBox1.min() == mBBox2.min());
        CPPUNIT_ASSERT(mBBox1.max() == mBBox2.max());
        
        CPPUNIT_ASSERT(mBBox1.minX() == -10);
        CPPUNIT_ASSERT(mBBox1.maxX() == 10);
        
        CPPUNIT_ASSERT(mBBox1.minY() == -12);
        CPPUNIT_ASSERT(mBBox1.maxY() == 12);
        
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(-10, -12));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(10, 12));
    }

    void testEquality() {
        CPPUNIT_ASSERT(mBBox1 == mBBox2);

        BoundingBox2f bbox3(-13, 13, -13, 13);
        CPPUNIT_ASSERT(mBBox1 != bbox3);
    }

    void testElementAssignment() {
        mBBox1.setMinX(-20);
        mBBox1.setMaxX(20);
        
        mBBox1.setMinY(-22);
        mBBox1.setMaxY(22);
        
        mBBox2.setMin(Vector2f(-20, -22));
        mBBox2.setMax(Vector2f(20, 22));
        
        CPPUNIT_ASSERT(mBBox1.min() == mBBox2.min());
        CPPUNIT_ASSERT(mBBox1.max() == mBBox2.max());
    }

    void testReset() {
        mBBox1.reset();
        
        CPPUNIT_ASSERT(mBBox1.empty());
    }

    void testExtendByVector2f() {
        mBBox1.reset();

        mBBox1.extendByVector2f(Vector2f(100, 200));
        CPPUNIT_ASSERT(!mBBox1.empty());
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(100, 200));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(100, 200));
        
        mBBox1.extendByVector2f(Vector2f(-100, -200));
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(-100, -200));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(100, 200));
        
        mBBox1.extendByVector2f(Vector2f(-500, -500));
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(-500, -500));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(100, 200));
    }

    void testExtendByBoundingBox2f() {
        mBBox1.reset();

        mBBox1.extendByBoundingBox2f(
            BoundingBox2f(Vector2f(50, 100), Vector2f(100, 200)));
        CPPUNIT_ASSERT(!mBBox1.empty());
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(50, 100));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(100, 200));
        
        mBBox1.extendByBoundingBox2f(
            BoundingBox2f(Vector2f(-100, -200), Vector2f(-50, -150)));
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(-100, -200));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(100, 200));
        
        mBBox1.extendByBoundingBox2f(
            BoundingBox2f(Vector2f(-500, -500), Vector2f(-400, -400)));
        CPPUNIT_ASSERT(mBBox1.min() == Vector2f(-500, -500));
        CPPUNIT_ASSERT(mBBox1.max() == Vector2f(100, 200));
    }

    void testSize() {
        CPPUNIT_ASSERT(mBBox1.sizeX() == 20.0);
        CPPUNIT_ASSERT(mBBox1.sizeY() == 24.0);
        CPPUNIT_ASSERT(mBBox1.size() == Vector2f(20.0, 24.0));
    }

    void testCenter() {
        mBBox1 = BoundingBox2f(0, 10, 0, 12);
        CPPUNIT_ASSERT(mBBox1.center() == Vector2f(5, 6));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoundingBox2fTest);
