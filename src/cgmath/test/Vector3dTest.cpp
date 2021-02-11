// Copyright 2008 Drew Olbrich

#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3d.h>

using cgmath::Vector3d;

class Vector3dTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Vector3dTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testArrayConstructor);
    CPPUNIT_TEST(testAsDoublePtr);
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
        const Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
    }

    void testCopyConstructor() {
        const Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        const Vector3d b(a);
        CPPUNIT_ASSERT(b == a);
    }

    void testArrayConstructor() {
        double doubleArray[] = { 1, 2, 3 };
        const Vector3d a(doubleArray);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
    }

    void testAsDoublePtr() {
        const Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        const double *fp = a.asDoublePtr();
        CPPUNIT_ASSERT(fp[0] == 1);
        CPPUNIT_ASSERT(fp[1] == 2);
        CPPUNIT_ASSERT(fp[2] == 3);
    }

    void testAssignmentOperator() {
        const Vector3d a(1, 2, 3);
        Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
        CPPUNIT_ASSERT(!(b == a));
        b = a;
        CPPUNIT_ASSERT(b == a);
    }

    void testSwap() {
        Vector3d a(1, 2, 3);
        Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
        a.swap(b);
        CPPUNIT_ASSERT(a == Vector3d(3, 4, 5));
        CPPUNIT_ASSERT(b == Vector3d(1, 2, 3));
    }

    void testSet() {
        Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        a.set(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(3, 4, 5));
    }

    void testConstArrayOperator() {
        const Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        CPPUNIT_ASSERT(a[2] == 3);
    }

    void testArrayOperator() {
        Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(a[0] == 1);
        CPPUNIT_ASSERT(a[1] == 2);
        CPPUNIT_ASSERT(a[2] == 3);
        a[0] = 3;
        a[1] = 4;
        a[2] = 5;
        CPPUNIT_ASSERT(a == Vector3d(3, 4, 5));
    }

    void testEqualsOperator() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(1, 2, 3);
        const Vector3d c(3, 4, 5);
        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a == c));
    }

    void testNotEqualsOperator() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(1, 2, 3);
        const Vector3d c(3, 4, 5);
        CPPUNIT_ASSERT(b != c);
        CPPUNIT_ASSERT(!(a != b));
    }

    void testEquivalent() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(1.01, 2.01, 3.01);
        const Vector3d c(3, 4, 5);
        CPPUNIT_ASSERT(a.equivalent(b, 0.1));
        CPPUNIT_ASSERT(!a.equivalent(c, 0.1));
    }

    void testLessThanOperator() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(1, 3, 6);
        const Vector3d c(3, 4, 5);
        CPPUNIT_ASSERT(a < b);
        CPPUNIT_ASSERT(b < c);
        CPPUNIT_ASSERT(a < c);
        CPPUNIT_ASSERT(!(c < b));
        CPPUNIT_ASSERT(!(c < a));
        CPPUNIT_ASSERT(!(b < a));
    }

    void testPlusOperator() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
        CPPUNIT_ASSERT(a + b == Vector3d(4, 6, 8));
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
    }

    void testPlusEqualsOperator() {
        Vector3d a(1, 2, 3);
        const Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
        a += b;
        CPPUNIT_ASSERT(a == Vector3d(4, 6, 8));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
    }

    void testMinusOperator() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
        CPPUNIT_ASSERT(a - b == Vector3d(-2, -2, -2));
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
    }

    void testMinusEqualsOperator() {
        Vector3d a(1, 2, 3);
        const Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
        a -= b;
        CPPUNIT_ASSERT(a == Vector3d(-2, -2, -2));
        CPPUNIT_ASSERT(b == Vector3d(3, 4, 5));
    }

    void testUnaryMinusOperator() {
        const Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(-a == Vector3d(-1, -2, -3));
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
    }

    void testScalarMultiplyOperator() {
        const Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(a*3 == Vector3d(3, 6, 9));
        CPPUNIT_ASSERT(3*a == Vector3d(3, 6, 9));
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
    }

    void testScalarMultiplyEqualsOperator() {
        Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        a *= 3;
        CPPUNIT_ASSERT(a == Vector3d(3, 6, 9));
    }

    void testVectorMultiplyOperator() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(a*b == Vector3d(3, 8, 15));
        CPPUNIT_ASSERT(b*a == Vector3d(3, 8, 15));
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
    }

    void testVectorMultiplyEqualsOperator() {
        Vector3d a(1, 2, 3);
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
        a *= Vector3d(3, 4, 5);
        CPPUNIT_ASSERT(a == Vector3d(3, 8, 15));
    }

    void testScalarDivideOperator() {
        const Vector3d a(3, 6, 9);
        CPPUNIT_ASSERT(a == Vector3d(3, 6, 9));
        CPPUNIT_ASSERT(a/3 == Vector3d(1, 2, 3));
        CPPUNIT_ASSERT(a == Vector3d(3, 6, 9));
    }

    void testScalarDivideEqualsOperator() {
        Vector3d a(3, 6, 9);
        CPPUNIT_ASSERT(a == Vector3d(3, 6, 9));
        a /= 3;
        CPPUNIT_ASSERT(a == Vector3d(1, 2, 3));
    }

    void testDot() {
        const Vector3d a(1, 2, 3);
        const Vector3d b(3, 4, 5);
        CPPUNIT_ASSERT(a.dot(b) == 26);
        CPPUNIT_ASSERT(b.dot(a) == 26);
    }

    void testCross() {
        const Vector3d a(2, 0, 0);
        const Vector3d b(0, 3, 0);
        CPPUNIT_ASSERT(a.cross(b) == Vector3d(0, 0, 6));
    }

    void testLength() {
        const Vector3d a(2, 3, 6);
        CPPUNIT_ASSERT(a.length() == 7);
    }

    void testLengthSquared() {
        const Vector3d a(2, 3, 6);
        CPPUNIT_ASSERT(a.lengthSquared() == 49);
    }

    void testNormalize() {
        Vector3d a(2, 0, 0);
        CPPUNIT_ASSERT(a == Vector3d(2, 0, 0));
        a.normalize();
        CPPUNIT_ASSERT(a == Vector3d(1, 0, 0));

        Vector3d b(0, 2, 0);
        CPPUNIT_ASSERT(b == Vector3d(0, 2, 0));
        b.normalize();
        CPPUNIT_ASSERT(b == Vector3d(0, 1, 0));

        Vector3d c(0, 0, 2);
        CPPUNIT_ASSERT(c == Vector3d(0, 0, 2));
        c.normalize();
        CPPUNIT_ASSERT(c == Vector3d(0, 0, 1));
    }

    void testNormalized() {
        Vector3d a(2, 0, 0);
        CPPUNIT_ASSERT(a.normalized() == Vector3d(1, 0, 0));
        Vector3d b(0, 2, 0);
        CPPUNIT_ASSERT(b.normalized() == Vector3d(0, 1, 0));
        Vector3d c(0, 0, 2);
        CPPUNIT_ASSERT(c.normalized() == Vector3d(0, 0, 1));
    }

    void testMaxAbs() {
        Vector3d a(3.5, 1, 1);
        CPPUNIT_ASSERT(a.maxAbs() == 3.5);
        Vector3d b(1, 3.5, 1);
        CPPUNIT_ASSERT(b.maxAbs() == 3.5);
        Vector3d c(1, 1, 3.5);
        CPPUNIT_ASSERT(c.maxAbs() == 3.5);
        Vector3d d(-3.5, 1, 1);
        CPPUNIT_ASSERT(d.maxAbs() == 3.5);
        Vector3d e(1, -3.5, 1);
        CPPUNIT_ASSERT(e.maxAbs() == 3.5);
        Vector3d f(1, 1, -3.5);
        CPPUNIT_ASSERT(f.maxAbs() == 3.5);
    }

    void testLeftShiftOperator() {
        const Vector3d a(1, 2, 3);
        std::ostringstream ostr;
        ostr << a;
        CPPUNIT_ASSERT(ostr.str() == "1 2 3");
    }

    void testZero() {
        CPPUNIT_ASSERT(Vector3d::ZERO == Vector3d(0, 0, 0));
    }

    void testUnit() {
        CPPUNIT_ASSERT(Vector3d::UNIT == Vector3d(1, 1, 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Vector3dTest);
