// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/PointOperations.h>
#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>

using cgmath::Vector2f;
using cgmath::Vector3f;

class PointOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PointOperationsTest);
    CPPUNIT_TEST(testSignedDistanceFromPointToLine);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testSignedDistanceFromPointToLine() {
        Vector2f p(3.0, 0.0);
        Vector2f a(0.0, 1.0);
        Vector2f b(0.0, 0.0);
        CPPUNIT_ASSERT(SignedDistanceFromPointToLine(p, a, b) == 3.0);

        a = Vector2f(0.0, -1.0);
        CPPUNIT_ASSERT(SignedDistanceFromPointToLine(p, a, b) == -3.0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PointOperationsTest);
