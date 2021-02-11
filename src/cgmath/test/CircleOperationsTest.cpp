// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cmath>

#include <cgmath/CircleOperations.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Constants.h>

using cgmath::PI;
using cgmath::Vector2f;
using cgmath::MapConcentricSquareToConcentricCircle;

class CircleOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CircleOperationsTest);
    CPPUNIT_TEST(testCalculateCircleFromThreePoints);
    CPPUNIT_TEST(testCalculateCircleFromThreePointsColinear);
    CPPUNIT_TEST(testCalculateCircleFromThreePointsCoincident);
    CPPUNIT_TEST(testMapConcentricSquareToConcentricCircle);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testCalculateCircleFromThreePoints() {
        const Vector2f a(0, 1);
        const Vector2f b(1, 0);
        const Vector2f c(2, 1);
        Vector2f center(0, 0);
        float radius = 0;
        bool result = CalculateCircleFromThreePoints(a, b, c, center, radius);
        CPPUNIT_ASSERT(result);
        CPPUNIT_ASSERT(center == Vector2f(1, 1));
        CPPUNIT_ASSERT(radius == 1);
    }

    void testCalculateCircleFromThreePointsColinear() {
        const Vector2f a(0, 0);
        const Vector2f b(1, 0);
        const Vector2f c(2, 0);
        Vector2f center(0, 0);
        float radius = 0;
        bool result = CalculateCircleFromThreePoints(a, b, c, center, radius);
        CPPUNIT_ASSERT(!result);
    }

    void testCalculateCircleFromThreePointsCoincident() {
        const Vector2f a(2, 1);
        const Vector2f b(2, 1);
        const Vector2f c(2, 1);
        Vector2f center(0, 0);
        float radius = 0;
        bool result = CalculateCircleFromThreePoints(a, b, c, center, radius);
        CPPUNIT_ASSERT(result);
        CPPUNIT_ASSERT(center == Vector2f(2, 1));
        CPPUNIT_ASSERT(radius == 0);
    }

    void testMapConcentricSquareToConcentricCircle() {
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(0.5, 0.5))
            .equivalent(Vector2f(0.0, 0.0), 0.0001));

        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(0.0, 0.5))
            .equivalent(Vector2f(-1.0, 0.0), 0.0001));
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(1.0, 0.5))
            .equivalent(Vector2f(1.0, 0.0), 0.0001));
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(0.5, 0.0))
            .equivalent(Vector2f(0.0, -1.0), 0.0001));
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(0.5, 1.0))
            .equivalent(Vector2f(0.0, 1.0), 0.0001));

        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(1.0, 1.0))
            .equivalent(Vector2f(cosf(1.0*PI/4.0), sinf(1.0*PI/4.0)), 0.0001));
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(0.0, 1.0))
            .equivalent(Vector2f(cosf(3.0*PI/4.0), sinf(3.0*PI/4.0)), 0.0001));
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(0.0, 0.0))
            .equivalent(Vector2f(cosf(5.0*PI/4.0), sinf(5.0*PI/4.0)), 0.0001));
        CPPUNIT_ASSERT(MapConcentricSquareToConcentricCircle(Vector2f(1.0, 0.0))
            .equivalent(Vector2f(cosf(7.0*PI/4.0), sinf(7.0*PI/4.0)), 0.0001));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(CircleOperationsTest);
