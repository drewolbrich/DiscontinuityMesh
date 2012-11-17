// Copyright 2008 Retroactive Fiasco.

#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>

using cgmath::Vector3f;
using cgmath::Vector4f;

class Vector4fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Vector4fTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testConstructorFromVector3f);
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
        const Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
    }

    void testCopyConstructor() {
        const Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        const Vector4f b(a);
        CPPUNIT_ASSERT(b == a);
    }

    void testConstructorFromVector3f() {
        const Vector3f a(1, 2, 3);
        const Vector4f b(a);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector4f(1, 2, 3, 1));
    }

    void testArrayConstructor() {
        float floatArray[] = { 1, 2, 3, 4 };
        const Vector4f a(floatArray);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
    }

    void testAsFloatPtr() {
        const Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        const float *fp = a.asFloatPtr();
        CPPUNIT_ASSERT(fp[0] == 1);
        CPPUNIT_ASSERT(fp[1] == 2);
        CPPUNIT_ASSERT(fp[2] == 3);
        CPPUNIT_ASSERT(fp[3] == 4);
    }

    void testAssignmentOperator() {
        const Vector4f a(1, 2, 3, 4);
        Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
        CPPUNIT_ASSERT(!(b == a));
        b = a;
        CPPUNIT_ASSERT(b == a);
    }

    void testSwap() {
        Vector4f a(1, 2, 3, 4);
        Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
        a.swap(b);
        CPPUNIT_ASSERT(a == Vector4f(3, 4, 5, 6));
        CPPUNIT_ASSERT(b == Vector4f(1, 2, 3, 4));
    }

    void testSet() {
        Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        a.set(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(3, 4, 5, 6));
    }

    void testConstArrayOperator() {
        const Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        CPPUNIT_ASSERT(a[2] == 3);
        CPPUNIT_ASSERT(a[3] == 4);
    }

    void testArrayOperator() {
        Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        CPPUNIT_ASSERT(a[2] == 3);
        CPPUNIT_ASSERT(a[3] == 4);
        a[0] = 3;
        a[1] = 4;
        a[2] = 5;
        a[3] = 6;
        CPPUNIT_ASSERT(a == Vector4f(3, 4, 5, 6));
    }

    void testEqualsOperator() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(1, 2, 3, 4);
        const Vector4f c(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a == c));
    }

    void testNotEqualsOperator() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(1, 2, 3, 4);
        const Vector4f c(3, 4, 5, 6);
        CPPUNIT_ASSERT(b != c);
        CPPUNIT_ASSERT(!(a != b));
    }

    void testEquivalent() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(1.01, 2.01, 3.01, 4.01);
        const Vector4f c(3, 4, 5, 6);
        CPPUNIT_ASSERT(a.equivalent(b, 0.1));
        CPPUNIT_ASSERT(!a.equivalent(c, 0.1));
    }

    void testLessThanOperator() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(1, 3, 6, 9);
        const Vector4f c(3, 4, 5, 6);
        CPPUNIT_ASSERT(a < b);
        CPPUNIT_ASSERT(b < c);
        CPPUNIT_ASSERT(a < c);
        CPPUNIT_ASSERT(!(c < b));
        CPPUNIT_ASSERT(!(c < a));
        CPPUNIT_ASSERT(!(b < a));
    }

    void testPlusOperator() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
        CPPUNIT_ASSERT(a + b == Vector4f(4, 6, 8, 10));
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
    }

    void testPlusEqualsOperator() {
        Vector4f a(1, 2, 3, 4);
        const Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
        a += b;
        CPPUNIT_ASSERT(a == Vector4f(4, 6, 8, 10));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
    }

    void testMinusOperator() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
        CPPUNIT_ASSERT(a - b == Vector4f(-2, -2, -2, -2));
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
    }

    void testMinusEqualsOperator() {
        Vector4f a(1, 2, 3, 4);
        const Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
        a -= b;
        CPPUNIT_ASSERT(a == Vector4f(-2, -2, -2, -2));
        CPPUNIT_ASSERT(b == Vector4f(3, 4, 5, 6));
    }

    void testUnaryMinusOperator() {
        const Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(-a == Vector4f(-1, -2, -3, -4));
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
    }

    void testScalarMultiplyOperator() {
        const Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(a*3 == Vector4f(3, 6, 9, 12));
        CPPUNIT_ASSERT(3*a == Vector4f(3, 6, 9, 12));
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
    }

    void testScalarMultiplyEqualsOperator() {
        Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        a *= 3;
        CPPUNIT_ASSERT(a == Vector4f(3, 6, 9, 12));
    }

    void testVectorMultiplyOperator() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(a*b == Vector4f(3, 8, 15, 24));
        CPPUNIT_ASSERT(b*a == Vector4f(3, 8, 15, 24));
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
    }

    void testVectorMultiplyEqualsOperator() {
        Vector4f a(1, 2, 3, 4);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        a *= Vector4f(3, 4, 5, 6);
        CPPUNIT_ASSERT(a == Vector4f(3, 8, 15, 24));
    }

    void testScalarDivideOperator() {
        const Vector4f a(3, 6, 9, 12);
        CPPUNIT_ASSERT(a == Vector4f(3, 6, 9, 12));
        CPPUNIT_ASSERT(a/3 == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(a == Vector4f(3, 6, 9, 12));
    }

    void testScalarDivideEqualsOperator() {
        Vector4f a(3, 6, 9, 12);
        CPPUNIT_ASSERT(a == Vector4f(3, 6, 9, 12));
        a /= 3;
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
    }

    void testDot() {
        const Vector4f a(1, 2, 3, 4);
        const Vector4f b(3, 4, 5, 6);
        CPPUNIT_ASSERT(a.dot(b) == 50);
        CPPUNIT_ASSERT(b.dot(a) == 50);
    }

    void testLength() {
        const Vector4f a(2, 4, 5, 6);
        CPPUNIT_ASSERT(a.length() == 9);
    }

    void testLengthSquared() {
        const Vector4f a(2, 4, 5, 6);
        CPPUNIT_ASSERT(a.lengthSquared() == 81);
    }

    void testNormalize() {
        Vector4f a(2, 0, 0, 0);
        CPPUNIT_ASSERT(a == Vector4f(2, 0, 0, 0));
        a.normalize();
        CPPUNIT_ASSERT(a == Vector4f(1, 0, 0, 0));

        Vector4f b(0, 2, 0, 0);
        CPPUNIT_ASSERT(b == Vector4f(0, 2, 0, 0));
        b.normalize();
        CPPUNIT_ASSERT(b == Vector4f(0, 1, 0, 0));

        Vector4f c(0, 0, 2, 0);
        CPPUNIT_ASSERT(c == Vector4f(0, 0, 2, 0));
        c.normalize();
        CPPUNIT_ASSERT(c == Vector4f(0, 0, 1, 0));

        Vector4f d(0, 0, 0, 2);
        CPPUNIT_ASSERT(d == Vector4f(0, 0, 0, 2));
        d.normalize();
        CPPUNIT_ASSERT(d == Vector4f(0, 0, 0, 1));
    }

    void testNormalized() {
        Vector4f a(2, 0, 0, 0);
        CPPUNIT_ASSERT(a.normalized() == Vector4f(1, 0, 0, 0));
        Vector4f b(0, 2, 0, 0);
        CPPUNIT_ASSERT(b.normalized() == Vector4f(0, 1, 0, 0));
        Vector4f c(0, 0, 2, 0);
        CPPUNIT_ASSERT(c.normalized() == Vector4f(0, 0, 1, 0));
        Vector4f d(0, 0, 0, 2);
        CPPUNIT_ASSERT(d.normalized() == Vector4f(0, 0, 0, 1));
    }

    void testMaxAbs() {
        Vector4f a(3.5, 1, 1, 1);
        CPPUNIT_ASSERT(a.maxAbs() == 3.5);
        Vector4f b(1, 3.5, 1, 1);
        CPPUNIT_ASSERT(b.maxAbs() == 3.5);
        Vector4f c(1, 1, 3.5, 1);
        CPPUNIT_ASSERT(c.maxAbs() == 3.5);
        Vector4f d(1, 1, 1, 3.5);
        CPPUNIT_ASSERT(d.maxAbs() == 3.5);
        Vector4f e(-3.5, 1, 1, 1);
        CPPUNIT_ASSERT(e.maxAbs() == 3.5);
        Vector4f f(1, -3.5, 1, 1);
        CPPUNIT_ASSERT(f.maxAbs() == 3.5);
        Vector4f g(1, 1, -3.5, 1);
        CPPUNIT_ASSERT(g.maxAbs() == 3.5);
        Vector4f h(1, 1, 1, -3.5);
        CPPUNIT_ASSERT(h.maxAbs() == 3.5);
    }

    void testLeftShiftOperator() {
        const Vector4f a(1, 2, 3, 4);
        std::ostringstream ostr;
        ostr << a;
        CPPUNIT_ASSERT(ostr.str() == "1 2 3 4");
    }

    void testZero() {
        CPPUNIT_ASSERT(Vector4f::ZERO == Vector4f(0, 0, 0, 0));
    }

    void testUnit() {
        CPPUNIT_ASSERT(Vector4f::UNIT == Vector4f(1, 1, 1, 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Vector4fTest);
