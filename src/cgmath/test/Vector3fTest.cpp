// Copyright 2008 Drew Olbrich

#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>

using cgmath::Vector3f;
using cgmath::Vector4f;

class Vector3fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Vector3fTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testConstructorFromVector4f);
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
    CPPUNIT_TEST(testCross);
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
        const Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
    }

    void testCopyConstructor() {
        const Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        const Vector3f b(a);
        CPPUNIT_ASSERT(b == a);
    }

    void testConstructorFromVector4f() {
        const Vector4f a(1, 2, 3, 4);
        const Vector3f b(a);
        CPPUNIT_ASSERT(a == Vector4f(1, 2, 3, 4));
        CPPUNIT_ASSERT(b == Vector3f(1, 2, 3));
    }

    void testArrayConstructor() {
        float floatArray[] = { 1, 2, 3 };
        const Vector3f a(floatArray);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
    }

    void testAsFloatPtr() {
        const Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        const float *fp = a.asFloatPtr();
        CPPUNIT_ASSERT(fp[0] == 1);
        CPPUNIT_ASSERT(fp[1] == 2);
        CPPUNIT_ASSERT(fp[2] == 3);
    }

    void testAssignmentOperator() {
        const Vector3f a(1, 2, 3);
        Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
        CPPUNIT_ASSERT(!(b == a));
        b = a;
        CPPUNIT_ASSERT(b == a);
    }

    void testSwap() {
        Vector3f a(1, 2, 3);
        Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
        a.swap(b);
        CPPUNIT_ASSERT(a == Vector3f(3, 4, 5));
        CPPUNIT_ASSERT(b == Vector3f(1, 2, 3));
    }

    void testSet() {
        Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        a.set(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(3, 4, 5));
    }

    void testConstArrayOperator() {
        const Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        CPPUNIT_ASSERT(a[2] == 3);
    }

    void testArrayOperator() {
        Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        CPPUNIT_ASSERT(a[2] == 3);
        a[0] = 3;
        a[1] = 4;
        a[2] = 5;
        CPPUNIT_ASSERT(a == Vector3f(3, 4, 5));
    }

    void testEqualsOperator() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(1, 2, 3);
        const Vector3f c(3, 4, 5);
        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a == c));
    }

    void testNotEqualsOperator() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(1, 2, 3);
        const Vector3f c(3, 4, 5);
        CPPUNIT_ASSERT(b != c);
        CPPUNIT_ASSERT(!(a != b));
    }

    void testEquivalent() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(1.01, 2.01, 3.01);
        const Vector3f c(3, 4, 5);
        CPPUNIT_ASSERT(a.equivalent(b, 0.1));
        CPPUNIT_ASSERT(!a.equivalent(c, 0.1));
    }

    void testLessThanOperator() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(1, 3, 6);
        const Vector3f c(3, 4, 5);
        CPPUNIT_ASSERT(a < b);
        CPPUNIT_ASSERT(b < c);
        CPPUNIT_ASSERT(a < c);
        CPPUNIT_ASSERT(!(c < b));
        CPPUNIT_ASSERT(!(c < a));
        CPPUNIT_ASSERT(!(b < a));
    }

    void testPlusOperator() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
        CPPUNIT_ASSERT(a + b == Vector3f(4, 6, 8));
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
    }

    void testPlusEqualsOperator() {
        Vector3f a(1, 2, 3);
        const Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
        a += b;
        CPPUNIT_ASSERT(a == Vector3f(4, 6, 8));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
    }

    void testMinusOperator() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
        CPPUNIT_ASSERT(a - b == Vector3f(-2, -2, -2));
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
    }

    void testMinusEqualsOperator() {
        Vector3f a(1, 2, 3);
        const Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
        a -= b;
        CPPUNIT_ASSERT(a == Vector3f(-2, -2, -2));
        CPPUNIT_ASSERT(b == Vector3f(3, 4, 5));
    }

    void testUnaryMinusOperator() {
        const Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(-a == Vector3f(-1, -2, -3));
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
    }

    void testScalarMultiplyOperator() {
        const Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(a*3 == Vector3f(3, 6, 9));
        CPPUNIT_ASSERT(3*a == Vector3f(3, 6, 9));
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
    }

    void testScalarMultiplyEqualsOperator() {
        Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        a *= 3;
        CPPUNIT_ASSERT(a == Vector3f(3, 6, 9));
    }

    void testVectorMultiplyOperator() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(a*b == Vector3f(3, 8, 15));
        CPPUNIT_ASSERT(b*a == Vector3f(3, 8, 15));
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
    }

    void testVectorMultiplyEqualsOperator() {
        Vector3f a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
        a *= Vector3f(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3f(3, 8, 15));
    }

    void testScalarDivideOperator() {
        const Vector3f a(3, 6, 9);
        CPPUNIT_ASSERT(a == Vector3f(3, 6, 9));
        CPPUNIT_ASSERT(a/3 == Vector3f(1, 2, 3));
        CPPUNIT_ASSERT(a == Vector3f(3, 6, 9));
    }

    void testScalarDivideEqualsOperator() {
        Vector3f a(3, 6, 9);
        CPPUNIT_ASSERT(a == Vector3f(3, 6, 9));
        a /= 3;
        CPPUNIT_ASSERT(a == Vector3f(1, 2, 3));
    }

    void testDot() {
        const Vector3f a(1, 2, 3);
        const Vector3f b(3, 4, 5);
        CPPUNIT_ASSERT(a.dot(b) == 26);
        CPPUNIT_ASSERT(b.dot(a) == 26);
    }

    void testCross() {
        const Vector3f a(2, 0, 0);
        const Vector3f b(0, 3, 0);
        CPPUNIT_ASSERT(a.cross(b) == Vector3f(0, 0, 6));
    }

    void testLength() {
        const Vector3f a(2, 3, 6);
        CPPUNIT_ASSERT(a.length() == 7);
    }

    void testLengthSquared() {
        const Vector3f a(2, 3, 6);
        CPPUNIT_ASSERT(a.lengthSquared() == 49);
    }

    void testNormalize() {
        Vector3f a(2, 0, 0);
        CPPUNIT_ASSERT(a == Vector3f(2, 0, 0));
        a.normalize();
        CPPUNIT_ASSERT(a == Vector3f(1, 0, 0));

        Vector3f b(0, 2, 0);
        CPPUNIT_ASSERT(b == Vector3f(0, 2, 0));
        b.normalize();
        CPPUNIT_ASSERT(b == Vector3f(0, 1, 0));

        Vector3f c(0, 0, 2);
        CPPUNIT_ASSERT(c == Vector3f(0, 0, 2));
        c.normalize();
        CPPUNIT_ASSERT(c == Vector3f(0, 0, 1));
    }

    void testNormalized() {
        Vector3f a(2, 0, 0);
        CPPUNIT_ASSERT(a.normalized() == Vector3f(1, 0, 0));
        Vector3f b(0, 2, 0);
        CPPUNIT_ASSERT(b.normalized() == Vector3f(0, 1, 0));
        Vector3f c(0, 0, 2);
        CPPUNIT_ASSERT(c.normalized() == Vector3f(0, 0, 1));
    }

    void testMaxAbs() {
        Vector3f a(3.5, 1, 1);
        CPPUNIT_ASSERT(a.maxAbs() == 3.5);
        Vector3f b(1, 3.5, 1);
        CPPUNIT_ASSERT(b.maxAbs() == 3.5);
        Vector3f c(1, 1, 3.5);
        CPPUNIT_ASSERT(c.maxAbs() == 3.5);
        Vector3f d(-3.5, 1, 1);
        CPPUNIT_ASSERT(d.maxAbs() == 3.5);
        Vector3f e(1, -3.5, 1);
        CPPUNIT_ASSERT(e.maxAbs() == 3.5);
        Vector3f f(1, 1, -3.5);
        CPPUNIT_ASSERT(f.maxAbs() == 3.5);
    }

    void testLeftShiftOperator() {
        const Vector3f a(1, 2, 3);
        std::ostringstream ostr;
        ostr << a;
        CPPUNIT_ASSERT(ostr.str() == "1 2 3");
    }

    void testZero() {
        CPPUNIT_ASSERT(Vector3f::ZERO == Vector3f(0, 0, 0));
    }

    void testUnit() {
        CPPUNIT_ASSERT(Vector3f::UNIT == Vector3f(1, 1, 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Vector3fTest);
