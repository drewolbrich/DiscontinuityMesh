// Copyright 2008 Drew Olbrich

#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>

using cgmath::Vector2f;
using cgmath::Vector3f;

class Vector2fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Vector2fTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testConstructorVector3f);
    CPPUNIT_TEST(testArrayConstructor);
    CPPUNIT_TEST(testAsFloatPtr);
    CPPUNIT_TEST(testAssignmentOperator);
    CPPUNIT_TEST(testSwap);
    CPPUNIT_TEST(testSet);
    CPPUNIT_TEST(testConstArrayOperator);
    CPPUNIT_TEST(testArrayOperator);
    CPPUNIT_TEST(testEqualsOperator);
    CPPUNIT_TEST(testNotEqualsOperator);
    CPPUNIT_TEST(testEquivalent);
    CPPUNIT_TEST(testLessThanOperator);
    CPPUNIT_TEST(testPlusOperator);
    CPPUNIT_TEST(testPlusEqualsOperator);
    CPPUNIT_TEST(testMinusOperator);
    CPPUNIT_TEST(testMinusEqualsOperator);
    CPPUNIT_TEST(testUnaryMinusOperator);
    CPPUNIT_TEST(testScalarMultiplyOperator);
    CPPUNIT_TEST(testScalarMultiplyEqualsOperator);
    CPPUNIT_TEST(testVectorMultiplyOperator);
    CPPUNIT_TEST(testVectorMultiplyEqualsOperator);
    CPPUNIT_TEST(testScalarDivideOperator);
    CPPUNIT_TEST(testScalarDivideEqualsOperator);
    CPPUNIT_TEST(testDot);
    CPPUNIT_TEST(testLength);
    CPPUNIT_TEST(testLengthSquared);
    CPPUNIT_TEST(testNormalize);
    CPPUNIT_TEST(testNormalized);
    CPPUNIT_TEST(testMaxAbs);
    CPPUNIT_TEST(testLeftShiftOperator);
    CPPUNIT_TEST(testZero);
    CPPUNIT_TEST(testUnit);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructor() {
        const Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
    }

    void testCopyConstructor() {
        const Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        const Vector2f b(a);
        CPPUNIT_ASSERT(b == a);
    }

    void testConstructorVector3f() {
        const Vector3f a(10, 11, 12);
        const Vector2f b(a, 0, 1);
        CPPUNIT_ASSERT(b == Vector2f(10, 11));
        const Vector2f c(a, 1, 2);
        CPPUNIT_ASSERT(c == Vector2f(11, 12));
    }

    void testArrayConstructor() {
        float floatArray[] = { 1, 2 };
        const Vector2f a(floatArray);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
    }

    void testAsFloatPtr() {
        const Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        const float *fp = a.asFloatPtr();
        CPPUNIT_ASSERT(fp[0] == 1);
        CPPUNIT_ASSERT(fp[1] == 2);
    }

    void testAssignmentOperator() {
        const Vector2f a(1, 2);
        Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
        CPPUNIT_ASSERT(!(b == a));
        b = a;
        CPPUNIT_ASSERT(b == a);
    }

    void testSwap() {
        Vector2f a(1, 2);
        Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
        a.swap(b);
        CPPUNIT_ASSERT(a == Vector2f(3, 4));
        CPPUNIT_ASSERT(b == Vector2f(1, 2));
    }

    void testSet() {
        Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        a.set(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(3, 4));
    }

    void testConstArrayOperator() {
        const Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
    }

    void testArrayOperator() {
        Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        a[0] = 3;
        a[1] = 4;
        CPPUNIT_ASSERT(a == Vector2f(3, 4));
    }

    void testEqualsOperator() {
        const Vector2f a(1, 2);
        const Vector2f b(1, 2);
        const Vector2f c(3, 4);
        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a == c));
    }

    void testNotEqualsOperator() {
        const Vector2f a(1, 2);
        const Vector2f b(1, 2);
        const Vector2f c(3, 4);
        CPPUNIT_ASSERT(b != c);
        CPPUNIT_ASSERT(!(a != b));
    }

    void testEquivalent() {
        const Vector2f a(1, 2);
        const Vector2f b(1.01, 2.01);
        const Vector2f c(3, 4);
        CPPUNIT_ASSERT(a.equivalent(b, 0.1));
        CPPUNIT_ASSERT(!a.equivalent(c, 0.1));
    }

    void testLessThanOperator() {
        const Vector2f a(1, 2);
        const Vector2f b(1, 3);
        const Vector2f c(3, 4);
        CPPUNIT_ASSERT(a < b);
        CPPUNIT_ASSERT(b < c);
        CPPUNIT_ASSERT(a < c);
        CPPUNIT_ASSERT(!(c < b));
        CPPUNIT_ASSERT(!(c < a));
        CPPUNIT_ASSERT(!(b < a));
    }

    void testPlusOperator() {
        const Vector2f a(1, 2);
        const Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
        CPPUNIT_ASSERT(a + b == Vector2f(4, 6));
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
    }

    void testPlusEqualsOperator() {
        Vector2f a(1, 2);
        const Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
        a += b;
        CPPUNIT_ASSERT(a == Vector2f(4, 6));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
    }

    void testMinusOperator() {
        const Vector2f a(1, 2);
        const Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
        CPPUNIT_ASSERT(a - b == Vector2f(-2, -2));
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
    }

    void testMinusEqualsOperator() {
        Vector2f a(1, 2);
        const Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
        a -= b;
        CPPUNIT_ASSERT(a == Vector2f(-2, -2));
        CPPUNIT_ASSERT(b == Vector2f(3, 4));
    }

    void testUnaryMinusOperator() {
        const Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(-a == Vector2f(-1, -2));
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
    }

    void testScalarMultiplyOperator() {
        const Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(a*3 == Vector2f(3, 6));
        CPPUNIT_ASSERT(3*a == Vector2f(3, 6));
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
    }

    void testScalarMultiplyEqualsOperator() {
        Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        a *= 3;
        CPPUNIT_ASSERT(a == Vector2f(3, 6));
    }

    void testVectorMultiplyOperator() {
        const Vector2f a(1, 2);
        const Vector2f b(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        CPPUNIT_ASSERT(a*b == Vector2f(3, 8));
        CPPUNIT_ASSERT(b*a == Vector2f(3, 8));
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
    }

    void testVectorMultiplyEqualsOperator() {
        Vector2f a(1, 2);
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
        a *= Vector2f(3, 4);
        CPPUNIT_ASSERT(a == Vector2f(3, 8));
    }

    void testScalarDivideOperator() {
        const Vector2f a(3, 6);
        CPPUNIT_ASSERT(a == Vector2f(3, 6));
        CPPUNIT_ASSERT(a/3 == Vector2f(1, 2));
        CPPUNIT_ASSERT(a == Vector2f(3, 6));
    }

    void testScalarDivideEqualsOperator() {
        Vector2f a(3, 6);
        CPPUNIT_ASSERT(a == Vector2f(3, 6));
        a /= 3;
        CPPUNIT_ASSERT(a == Vector2f(1, 2));
    }

    void testDot() {
        const Vector2f a(1, 2);
        const Vector2f b(3, 4);
        CPPUNIT_ASSERT(a.dot(b) == 11);
        CPPUNIT_ASSERT(b.dot(a) == 11);
    }

    void testLength() {
        const Vector2f a(3, 4);
        CPPUNIT_ASSERT(a.length() == 5);
    }

    void testLengthSquared() {
        const Vector2f a(3, 4);
        CPPUNIT_ASSERT(a.lengthSquared() == 25);
    }

    void testNormalize() {
        Vector2f a(2, 0);
        CPPUNIT_ASSERT(a == Vector2f(2, 0));
        a.normalize();
        CPPUNIT_ASSERT(a == Vector2f(1, 0));

        Vector2f b(0, 2);
        CPPUNIT_ASSERT(b == Vector2f(0, 2));
        b.normalize();
        CPPUNIT_ASSERT(b == Vector2f(0, 1));
    }

    void testNormalized() {
        Vector2f a(2, 0);
        CPPUNIT_ASSERT(a.normalized() == Vector2f(1, 0));
        Vector2f b(0, 2);
        CPPUNIT_ASSERT(b.normalized() == Vector2f(0, 1));
    }

    void testMaxAbs() {
        Vector2f a(3.5, 1);
        CPPUNIT_ASSERT(a.maxAbs() == 3.5);
        Vector2f b(1, 3.5);
        CPPUNIT_ASSERT(b.maxAbs() == 3.5);
        Vector2f c(-3.5, 1);
        CPPUNIT_ASSERT(c.maxAbs() == 3.5);
        Vector2f d(1, -3.5);
        CPPUNIT_ASSERT(d.maxAbs() == 3.5);
    }

    void testLeftShiftOperator() {
        const Vector2f a(1, 2);
        std::ostringstream ostr;
        ostr << a;
        CPPUNIT_ASSERT(ostr.str() == "1 2");
    }

    void testZero() {
        CPPUNIT_ASSERT(Vector2f::ZERO == Vector2f(0, 0));
    }

    void testUnit() {
        CPPUNIT_ASSERT(Vector2f::UNIT == Vector2f(1, 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Vector2fTest);
