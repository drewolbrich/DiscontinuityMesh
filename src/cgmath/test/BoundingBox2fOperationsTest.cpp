// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/BoundingBox2fOperations.h>
#include <cgmath/BoundingBox2f.h>

using cgmath::BoundingBox2f;
using cgmath::Vector2f;

class BoundingBox2fOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BoundingBox2fOperationsTest);
    CPPUNIT_TEST(testBoundingBox2fIntersectsBoundingBox2f);
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

    void testBoundingBox2fIntersectsBoundingBox2f() {
        mBBox1 = BoundingBox2f(-1, 1, -1, 1);
        mBBox2 = BoundingBox2f(-2, 2, -2, 2);
        CPPUNIT_ASSERT(BoundingBox2fIntersectsBoundingBox2f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(BoundingBox2fIntersectsBoundingBox2f(mBBox2, mBBox1));

        mBBox1 = BoundingBox2f(-1, 1, -1, 1);
        mBBox2 = BoundingBox2f(0, 2, 0, 2);
        CPPUNIT_ASSERT(BoundingBox2fIntersectsBoundingBox2f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(BoundingBox2fIntersectsBoundingBox2f(mBBox2, mBBox1));

        mBBox1 = BoundingBox2f(-1, 1, -1, 1);
        mBBox2 = BoundingBox2f(-2, 0, -2, 0);
        CPPUNIT_ASSERT(BoundingBox2fIntersectsBoundingBox2f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(BoundingBox2fIntersectsBoundingBox2f(mBBox2, mBBox1));

        mBBox1 = BoundingBox2f(-1, 1, -1, 1);
        mBBox2 = BoundingBox2f(-3, -2, -3, -2);
        CPPUNIT_ASSERT(!BoundingBox2fIntersectsBoundingBox2f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(!BoundingBox2fIntersectsBoundingBox2f(mBBox2, mBBox1));

        mBBox1 = BoundingBox2f(-1, 1, -1, 1);
        mBBox2 = BoundingBox2f(2, 3, 2, 3);
        CPPUNIT_ASSERT(!BoundingBox2fIntersectsBoundingBox2f(mBBox1, mBBox2));
        CPPUNIT_ASSERT(!BoundingBox2fIntersectsBoundingBox2f(mBBox2, mBBox1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BoundingBox2fOperationsTest);
