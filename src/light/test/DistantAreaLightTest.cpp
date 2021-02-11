// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <light/DistantAreaLight.h>

using light::DistantAreaLight;
using cgmath::Vector3f;

class DistantAreaLightTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DistantAreaLightTest);
    CPPUNIT_TEST(testSetPositionAsAzimuthAndElevation);
    CPPUNIT_TEST(testCalculateVertex);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    bool equivalent(const Vector3f &lhs, const Vector3f &rhs) const {
        return (lhs - rhs).length() < 0.001;
    }

    void testSetPositionAsAzimuthAndElevation() {
        DistantAreaLight distantAreaLight;

        // South.
        distantAreaLight.setPositionFromAzimuthAndElevation(0.0, 0.0);
        CPPUNIT_ASSERT(equivalent(distantAreaLight.position(), Vector3f(0.0, 0.0, -1.0)));

        // East.
        distantAreaLight.setPositionFromAzimuthAndElevation(90.0, 0.0);
        CPPUNIT_ASSERT(equivalent(distantAreaLight.position(), Vector3f(1.0, 0.0, 0.0)));

        // North.
        distantAreaLight.setPositionFromAzimuthAndElevation(180.0, 0.0);
        CPPUNIT_ASSERT(equivalent(distantAreaLight.position(), Vector3f(0.0, 0.0, 1.0)));

        // West.
        distantAreaLight.setPositionFromAzimuthAndElevation(270.0, 0.0);
        CPPUNIT_ASSERT(equivalent(distantAreaLight.position(), Vector3f(-1.0, 0.0, 0.0)));

        // Up.
        distantAreaLight.setPositionFromAzimuthAndElevation(0.0, 90.0);
        CPPUNIT_ASSERT(equivalent(distantAreaLight.position(), Vector3f(0.0, 1.0, 0.0)));

        // Down.
        distantAreaLight.setPositionFromAzimuthAndElevation(0.0, -90.0);
        CPPUNIT_ASSERT(equivalent(distantAreaLight.position(), Vector3f(0.0, -1.0, 0.0)));
    }

    void testCalculateVertex() {
        DistantAreaLight distantAreaLight;
        distantAreaLight.setPosition(Vector3f(1.0, 0.0, 0.0));
        distantAreaLight.setAngularDiameter(90.0*60.0);
        distantAreaLight.setSides(4000);
        distantAreaLight.setSceneDiameter(10.0);
        distantAreaLight.prepareForVertexCalculation();
        CPPUNIT_ASSERT(equivalent(distantAreaLight.calculateVertex(Vector3f::ZERO, 0),
                Vector3f(10, 10, 0)));
        CPPUNIT_ASSERT(equivalent(distantAreaLight.calculateVertex(Vector3f::ZERO, 1000),
                Vector3f(10, 0, 10)));
        CPPUNIT_ASSERT(equivalent(distantAreaLight.calculateVertex(Vector3f::ZERO, 2000),
                Vector3f(10, -10, 0)));
        CPPUNIT_ASSERT(equivalent(distantAreaLight.calculateVertex(Vector3f::ZERO, 3000),
                Vector3f(10, 0, -10)));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DistantAreaLightTest);
