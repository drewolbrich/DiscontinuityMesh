// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <exact/GeometricPredicates.h>

#include <rfm_direct/LineSegment.h>

using cgmath::Vector2f;

class LineSegmentTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(LineSegmentTest);
    CPPUNIT_TEST(testOccludesSharedEndpoint);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testOccludesSharedEndpoint() {
        Endpoint a0;
        Endpoint a1;
        Endpoint b0;
        Endpoint b1;
        LineSegment a;
        LineSegment b;

        a0.setWedgePosition(Vector2f(-0.00335475, -6.36314));
        a1.setWedgePosition(Vector2f(0.0704629, -6.32829));
        b0.setWedgePosition(Vector2f(0.0704629, -6.32829));
        b1.setWedgePosition(Vector2f(0.0267091, -6.77033));

        a.setPoint0(a0);
        a.setPoint1(a1);
        b.setPoint0(b0);
        b.setPoint1(b1);

        CPPUNIT_ASSERT(a.occludes(b));
        CPPUNIT_ASSERT(!b.occludes(a));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LineSegmentTest);
