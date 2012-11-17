// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <cmath>

#include <cgmath/PlaneOperations.h>

using cgmath::Vector3f;

class PlaneOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PlaneOperationsTest);
    CPPUNIT_TEST(testClosestPointOnPlane);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testClosestPointOnPlane() {
        Vector3f closestPoint;
        bool result = GetClosestPointOnPlane(
            Vector3f(2.0, 3.0, -4.0),
            Vector3f(0.0, 0.0, 1.0),
            Vector3f(7.0, 8.0, 9.0),
            &closestPoint);
        CPPUNIT_ASSERT(result);
        CPPUNIT_ASSERT(fabs((closestPoint - Vector3f(7.0, 8.0, -4.0)).length()) < 0.001);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PlaneOperationsTest);
