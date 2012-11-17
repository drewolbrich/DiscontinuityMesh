// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3fOperations.h>
#include <cgmath/Vector3f.h>

using cgmath::Vector3f;
using cgmath::GetPrimaryAxesMostOrthogonalToVector;
using cgmath::GetLongestAxis;

class Vector3fOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Vector3fOperationsTest);
    CPPUNIT_TEST(testGetPrimaryAxesMostOrthogonalToVector0);
    CPPUNIT_TEST(testGetPrimaryAxesMostOrthogonalToVector1);
    CPPUNIT_TEST(testGetPrimaryAxesMostOrthogonalToVector2);
    CPPUNIT_TEST(testGetPrimaryAxesMostOrthogonalToVector0Negated);
    CPPUNIT_TEST(testGetPrimaryAxesMostOrthogonalToVector1Negated);
    CPPUNIT_TEST(testGetPrimaryAxesMostOrthogonalToVector2Negated);
    CPPUNIT_TEST(testGetLongestAxis);
    CPPUNIT_TEST(testInterpolateVector3f);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testGetPrimaryAxesMostOrthogonalToVector0() {
        Vector3f vector(1.0, 0.0, 0.0);
        unsigned axis1 = 0;
        unsigned axis2 = 0;
        unsigned axis3 = 0;
        GetPrimaryAxesMostOrthogonalToVector(vector, &axis1, &axis2, &axis3);
        CPPUNIT_ASSERT(axis1 == 1);
        CPPUNIT_ASSERT(axis2 == 2);
        CPPUNIT_ASSERT(axis3 == 0);
    }

    void testGetPrimaryAxesMostOrthogonalToVector1() {
        Vector3f vector(0.0, 1.0, 0.0);
        unsigned axis1 = 0;
        unsigned axis2 = 0;
        unsigned axis3 = 0;
        GetPrimaryAxesMostOrthogonalToVector(vector, &axis1, &axis2, &axis3);
        CPPUNIT_ASSERT(axis1 == 2);
        CPPUNIT_ASSERT(axis2 == 0);
        CPPUNIT_ASSERT(axis3 == 1);
    }

    void testGetPrimaryAxesMostOrthogonalToVector2() {
        Vector3f vector(0.0, 0.0, 1.0);
        unsigned axis1 = 0;
        unsigned axis2 = 0;
        unsigned axis3 = 0;
        GetPrimaryAxesMostOrthogonalToVector(vector, &axis1, &axis2, &axis3);
        CPPUNIT_ASSERT(axis1 == 0);
        CPPUNIT_ASSERT(axis2 == 1);
        CPPUNIT_ASSERT(axis3 == 2);
    }

    void testGetPrimaryAxesMostOrthogonalToVector0Negated() {
        Vector3f vector(-1.0, 0.0, 0.0);
        unsigned axis1 = 0;
        unsigned axis2 = 0;
        unsigned axis3 = 0;
        GetPrimaryAxesMostOrthogonalToVector(vector, &axis1, &axis2, &axis3);
        CPPUNIT_ASSERT(axis1 == 2);
        CPPUNIT_ASSERT(axis2 == 1);
        CPPUNIT_ASSERT(axis3 == 0);
    }

    void testGetPrimaryAxesMostOrthogonalToVector1Negated() {
        Vector3f vector(0.0, -1.0, 0.0);
        unsigned axis1 = 0;
        unsigned axis2 = 0;
        unsigned axis3 = 0;
        GetPrimaryAxesMostOrthogonalToVector(vector, &axis1, &axis2, &axis3);
        CPPUNIT_ASSERT(axis1 == 0);
        CPPUNIT_ASSERT(axis2 == 2);
        CPPUNIT_ASSERT(axis3 == 1);
    }

    void testGetPrimaryAxesMostOrthogonalToVector2Negated() {
        Vector3f vector(0.0, 0.0, -1.0);
        unsigned axis1 = 0;
        unsigned axis2 = 0;
        unsigned axis3 = 0;
        GetPrimaryAxesMostOrthogonalToVector(vector, &axis1, &axis2, &axis3);
        CPPUNIT_ASSERT(axis1 == 1);
        CPPUNIT_ASSERT(axis2 == 0);
        CPPUNIT_ASSERT(axis3 == 2);
    }

    void testGetLongestAxis() {
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(1.0, 0.0, 0.0)) == 0);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(0.0, 1.0, 0.0)) == 1);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(0.0, 0.0, 1.0)) == 2);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(-1.0, 0.0, 0.0)) == 0);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(0.0, -1.0, 0.0)) == 1);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(0.0, 0.0, -1.0)) == 2);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(1.0, 1.0, 1.0)) == 0);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(1.0, 2.0, 2.0)) == 1);
        CPPUNIT_ASSERT(GetLongestAxis(Vector3f(1.0, 1.0, 2.0)) == 2);
    }

    void testInterpolateVector3f() {
        Vector3f a(2.0, 4.0, 6.0);
        Vector3f b(4.0, 8.0, 12.0);

        CPPUNIT_ASSERT(InterpolateVector3f(a, b, 0.0) == Vector3f(2.0, 4.0, 6.0));
        CPPUNIT_ASSERT(InterpolateVector3f(a, b, 0.5) == Vector3f(3.0, 6.0, 9.0));
        CPPUNIT_ASSERT(InterpolateVector3f(a, b, 1.0) == Vector3f(4.0, 8.0, 12.0));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Vector3fOperationsTest);
