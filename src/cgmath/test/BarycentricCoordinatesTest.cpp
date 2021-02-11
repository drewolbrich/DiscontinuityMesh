// Copyright 2010 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/BarycentricCoordinates.h>

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::GetBarycentricCoordinatesOfPointOnTriangle2f;
using cgmath::GetBarycentricCoordinatesOfPointOnTriangle3f;

class BarycentricCoordinatesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BarycentricCoordinatesTest);
    CPPUNIT_TEST(testGetBarycentricCoordinatesOfPointOnTriangle2f);
    CPPUNIT_TEST(testGetBarycentricCoordinatesOfPointOnTriangle3f);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testGetBarycentricCoordinatesOfPointOnTriangle2f() {
        Vector2f r0(0, 0);
        Vector2f r1(10, 0);
        Vector2f r2(0, 10);

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle2f(
                           Vector2f(0, 0), r0, r1, r2)
            .equivalent(Vector3f(1, 0, 0), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle2f(
                           Vector2f(10, 0), r0, r1, r2)
            .equivalent(Vector3f(0, 1, 0), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle2f(
                           Vector2f(0, 10), r0, r1, r2)
            .equivalent(Vector3f(0, 0, 1), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle2f(
                           Vector2f(5, 0), r0, r1, r2)
            .equivalent(Vector3f(0.5, 0.5, 0), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle2f(
                           Vector2f(0, 5), r0, r1, r2)
            .equivalent(Vector3f(0.5, 0, 0.5), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle2f(
                           Vector2f(5, 5), r0, r1, r2)
            .equivalent(Vector3f(0, 0.5, 0.5), 0.0001));
    }

    void testGetBarycentricCoordinatesOfPointOnTriangle3f() {
        Vector3f r0(0, 0, 3);
        Vector3f r1(10, 0, 4);
        Vector3f r2(0, 10, 5);

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle3f(
                           Vector3f(0, 0, 3), r0, r1, r2)
            .equivalent(Vector3f(1, 0, 0), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle3f(
                           Vector3f(10, 0, 4), r0, r1, r2)
            .equivalent(Vector3f(0, 1, 0), 0.0001));

        CPPUNIT_ASSERT(GetBarycentricCoordinatesOfPointOnTriangle3f(
                           Vector3f(0, 10, 5), r0, r1, r2)
            .equivalent(Vector3f(0, 0, 1), 0.0001));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(BarycentricCoordinatesTest);
