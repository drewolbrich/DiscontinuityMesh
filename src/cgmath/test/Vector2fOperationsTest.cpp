// Copyright 2009 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector2fOperations.h>
#include <cgmath/Vector2f.h>

using cgmath::Vector2f;

class Vector2fOperationsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Vector2fOperationsTest);
    CPPUNIT_TEST(testInterpolateVector2f);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testInterpolateVector2f() {
        Vector2f a(2.0, 4.0);
        Vector2f b(4.0, 8.0);

        CPPUNIT_ASSERT(InterpolateVector2f(a, b, 0.0) == Vector2f(2.0, 4.0));
        CPPUNIT_ASSERT(InterpolateVector2f(a, b, 0.5) == Vector2f(3.0, 6.0));
        CPPUNIT_ASSERT(InterpolateVector2f(a, b, 1.0) == Vector2f(4.0, 8.0));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Vector2fOperationsTest);
