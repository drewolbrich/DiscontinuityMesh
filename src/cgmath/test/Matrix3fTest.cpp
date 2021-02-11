// Copyright 2008 Drew Olbrich

#include <sstream>
#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/Matrix4f.h>

using cgmath::Vector3f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;

class Matrix3fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Matrix3fTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testConstructorFromColumnVectors);
    CPPUNIT_TEST(testConstructorFromPointer);
    CPPUNIT_TEST(testConstructorFromArray);
    CPPUNIT_TEST(testAsFloatPtr);
    CPPUNIT_TEST(testConstParenthesisOperator);
    CPPUNIT_TEST(testParenthesisOperator);
    CPPUNIT_TEST(testEqualsOperator);
    CPPUNIT_TEST(testNotEqualsOperator);
    CPPUNIT_TEST(testAssignmentOperator);
    CPPUNIT_TEST(testSet);
    CPPUNIT_TEST(testPlusOperator);
    CPPUNIT_TEST(testPlusEqualsOperator);
    CPPUNIT_TEST(testMinusOperator);
    CPPUNIT_TEST(testMinusEqualsOperator);
    CPPUNIT_TEST(testUnaryMinusOperator);
    CPPUNIT_TEST(testScalarMultiplyOperator);
    CPPUNIT_TEST(testScalarMultiplyEqualsOperator);
    CPPUNIT_TEST(testVectorPremultiplyOperator);
    CPPUNIT_TEST(testVectorPostmultiplyOperator);
    CPPUNIT_TEST(testMatrixMultiplyOperator);
    CPPUNIT_TEST(testScalarDivideOperator);
    CPPUNIT_TEST(testScalarDivideEqualsOperator);
    CPPUNIT_TEST(testTranspose);
    CPPUNIT_TEST(testInverse);
    CPPUNIT_TEST(testAdjugate);
    CPPUNIT_TEST(testDeterminant);
    CPPUNIT_TEST(testLeftShiftOperator);
    CPPUNIT_TEST(testZero);
    CPPUNIT_TEST(testIdentity);
    CPPUNIT_TEST(testRotation);
    CPPUNIT_TEST(testScale);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructor() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        CPPUNIT_ASSERT(a == Matrix3f(1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    void testCopyConstructor() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(a);
        CPPUNIT_ASSERT(a == b);
    }

    void testConstructorFromColumnVectors() {
        const Matrix3f a(
            1,  2,  3, 
            5,  6,  7, 
            9, 10, 11);
        const Vector3f v1(1, 5, 9);
        const Vector3f v2(2, 6, 10);
        const Vector3f v3(3, 7, 11);
        const Matrix3f b(v1, v2, v3);
        CPPUNIT_ASSERT(a == b);
    }

    void testConstructorFromPointer() {
        const float data[9] = {
            1, 5, 9,
            2, 6, 10,
            3, 7, 11
        };
        const Matrix3f a(data);
        CPPUNIT_ASSERT(a == Matrix3f(1, 2, 3, 5, 6, 7, 9, 10, 11));
    }

    void testConstructorFromArray() {
        const float data[3][3] = {
            { 1, 5, 9 },
            { 2, 6, 10 },
            { 3, 7, 11 }
        };
        const Matrix3f a(data);
        CPPUNIT_ASSERT(a == Matrix3f(1, 2, 3, 5, 6, 7, 9, 10, 11));
    }

    void testAsFloatPtr() {
        const Matrix3f a(1, 2, 3, 5, 6, 7, 9, 10, 11);
        const float *fp = a.asFloatPtr();
        CPPUNIT_ASSERT(fp[0] == 1);
        CPPUNIT_ASSERT(fp[1] == 5);
        CPPUNIT_ASSERT(fp[2] == 9);
        CPPUNIT_ASSERT(fp[3] == 2);
        CPPUNIT_ASSERT(fp[4] == 6);
        CPPUNIT_ASSERT(fp[5] == 10);
        CPPUNIT_ASSERT(fp[6] == 3);
        CPPUNIT_ASSERT(fp[7] == 7);
        CPPUNIT_ASSERT(fp[8] == 11);
    }

    void testConstParenthesisOperator() {
        const Matrix3f a(
            1,  2,  3, 
            5,  6,  7, 
            9, 10, 11);
        CPPUNIT_ASSERT(a(0, 0) == 1);
        CPPUNIT_ASSERT(a(0, 1) == 2);
        CPPUNIT_ASSERT(a(0, 2) == 3);
        CPPUNIT_ASSERT(a(1, 0) == 5);
        CPPUNIT_ASSERT(a(1, 1) == 6);
        CPPUNIT_ASSERT(a(1, 2) == 7);
        CPPUNIT_ASSERT(a(2, 0) == 9);
        CPPUNIT_ASSERT(a(2, 1) == 10);
        CPPUNIT_ASSERT(a(2, 2) == 11);
    }

    void testParenthesisOperator() {
        Matrix3f a;
        a(0, 0) = 1;
        a(0, 1) = 2;
        a(0, 2) = 3;
        a(1, 0) = 5;
        a(1, 1) = 6;
        a(1, 2) = 7;
        a(2, 0) = 9;
        a(2, 1) = 10;
        a(2, 2) = 11;
        CPPUNIT_ASSERT(a == Matrix3f(1, 2, 3, 5, 6, 7, 9, 10, 11));
    }

    void testEqualsOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f c(0, 1, 2, 3, 4, 5, 6, 7, 8);
        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a == c));
    }

    void testNotEqualsOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f c(0, 1, 2, 3, 4, 5, 6, 7, 8);
        CPPUNIT_ASSERT(a != c);
        CPPUNIT_ASSERT(!(a != b));
    }

    void testAssignmentOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix3f b(0, 1, 2, 3, 4, 5, 6, 7, 8);
        CPPUNIT_ASSERT(b == Matrix3f(0, 1, 2, 3, 4, 5, 6, 7, 8));
        b = a;
        CPPUNIT_ASSERT(b == Matrix3f(1, 2, 3, 4, 5, 6, 7, 8, 9));
    }

    void testSet() {
        Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        CPPUNIT_ASSERT(a == Matrix3f(1, 2, 3, 4, 5, 6, 7, 8, 9));
        a.set(0, 1, 2, 3, 4, 5, 6, 7, 8);
        CPPUNIT_ASSERT(a == Matrix3f(0, 1, 2, 3, 4, 5, 6, 7, 8));
    }

    void testPlusOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(0, 1, 2, 3, 4, 5, 6, 7, 8);
        Matrix3f c = a + b;
        CPPUNIT_ASSERT(c == Matrix3f(1, 3, 5, 7, 9, 11, 13, 15, 17));
    }

    void testPlusEqualsOperator() {
        Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(0, 1, 2, 3, 4, 5, 6, 7, 8);
        a += b;
        CPPUNIT_ASSERT(a == Matrix3f(1, 3, 5, 7, 9, 11, 13, 15, 17));
    }

    void testMinusOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(0, 1, 2, 3, 4, 5, 6, 7, 8);
        Matrix3f c = a - b;
        CPPUNIT_ASSERT(c == Matrix3f(1, 1, 1, 1, 1, 1, 1, 1, 1));
    }

    void testMinusEqualsOperator() {
        Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const Matrix3f b(0, 1, 2, 3, 4, 5, 6, 7, 8);
        a -= b;
        CPPUNIT_ASSERT(a == Matrix3f(1, 1, 1, 1, 1, 1, 1, 1, 1));
    }

    void testUnaryMinusOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        CPPUNIT_ASSERT(-a == Matrix3f(-1, -2, -3, -4, -5, -6, -7, -8, -9));
    }

    void testScalarMultiplyOperator() {
        const Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        CPPUNIT_ASSERT(3*a == Matrix3f(3, 6, 9, 12, 15, 18, 21, 24, 27));
        CPPUNIT_ASSERT(a*3 == Matrix3f(3, 6, 9, 12, 15, 18, 21, 24, 27));
    }

    void testScalarMultiplyEqualsOperator() {
        Matrix3f a(1, 2, 3, 4, 5, 6, 7, 8, 9);
        a *= 3;
        CPPUNIT_ASSERT(a == Matrix3f(3, 6, 9, 12, 15, 18, 21, 24, 27));
    }

    void testVectorPremultiplyOperator() {
        const Matrix3f a(1, 5, 9, 13, 2, 6, 10, 14, 3);
        const Vector3f v3(1, 2, 3);
        CPPUNIT_ASSERT(a*v3 == Vector3f(38, 35, 47));
        CPPUNIT_ASSERT(Matrix3f::IDENTITY*Vector3f(1, 2, 3) == Vector3f(1, 2, 3));
    }

    void testVectorPostmultiplyOperator() {
        const Matrix3f a(1, 5, 9, 13, 2, 6, 10, 14, 3);
        const Vector3f v3(1, 2, 3);
        CPPUNIT_ASSERT(v3*a == Vector3f(57, 51, 30));
        CPPUNIT_ASSERT(Vector3f(1, 2, 3)*Matrix3f::IDENTITY == Vector3f(1, 2, 3));
    }

    void testMatrixMultiplyOperator() {
        const Matrix3f a(
            8, 5, 1,
            9, 3, 1,
            4, 2, 1);
        const Matrix3f b(
            5, 1, 4,
            1, 5, 8,
            1, 3, 4);
        const Matrix3f d(
            46, 36, 76,
            49, 27, 64,
            23, 17, 36);
        CPPUNIT_ASSERT(a*b == d);
    }

    void testScalarDivideOperator() {
        const Matrix3f a(2, 4, 6, 8, 10, 12, 14, 16, 18);
        const Matrix3f b(1, 2, 3, 4, 5, 6, 7, 8, 9);
        CPPUNIT_ASSERT(a/2 == b);
    }

    void testScalarDivideEqualsOperator() {
        Matrix3f a(2, 4, 6, 8, 10, 12, 14, 16, 18);
        const Matrix3f b(1, 2, 3, 4, 5, 6, 7, 8, 9);
        a /= 2;
        CPPUNIT_ASSERT(a == b);
    }

    void testTranspose() {
        const Matrix3f a(
            2,  4,  6,
            10, 12, 14,
            18, 20, 22);
        const Matrix3f b(
            2, 10, 18,
            4, 12, 20,
            6, 14, 22);
        CPPUNIT_ASSERT(a.transpose() == b);
        CPPUNIT_ASSERT(b.transpose() == a);
    }

    void testInverse() {
        const Matrix3f a(
            1, 2, 0,
            1, 4, 1,
            1, 4, 0);
        CPPUNIT_ASSERT(a.inverse() == Matrix3f(
                           2, 0, -1,
                           -0.5, 0, 0.5,
                           0, 1, -1));
    }

    void testAdjugate() {
        const Matrix3f a(
            8, 9, 4,
            5, 3, 2,
            1, 1, 1);
        CPPUNIT_ASSERT(a.adjugate() == Matrix3f(
                           1, -5, 6,
                           -3, 4, 4,
                           2, 1, -21));
    }

    void testDeterminant() {
        const Matrix3f a(
            8, 9, 4,
            5, 3, 2,
            1, 1, 1);
        CPPUNIT_ASSERT(a.determinant() == -11);
    }

    void testLeftShiftOperator() {
        const Matrix3f a(
            1,  2,  3, 
            5,  6,  7, 
            9, 10, 11);
        std::ostringstream ostr;
        ostr << a;
        CPPUNIT_ASSERT(ostr.str() == "1  2  3\n5  6  7\n9 10 11\n");
    }

    void testZero() {
        CPPUNIT_ASSERT(Matrix3f::ZERO == Matrix3f(0, 0, 0, 0, 0, 0, 0, 0, 0));
    }

    void testIdentity() {
        CPPUNIT_ASSERT(Matrix3f::IDENTITY == Matrix3f(1, 0, 0, 0, 1, 0, 0, 0, 1));
    }

    void testRotation() {
        const Matrix3f r(Matrix3f::fromRotation(Vector3f(0, 0, 1), 3.1415927F/2.0));
        const Vector3f v(1, 0, 0);
        const Vector3f w = r*v;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(w[0], 0, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(w[1], 1, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(w[2], 0, 0.0001);
    }

    void testScale() {
        const Matrix3f t(Matrix3f::fromScale(Vector3f(1, 2, 3)));
        CPPUNIT_ASSERT(t == Matrix3f(
                           1, 0, 0,
                           0, 2, 0,
                           0, 0, 3));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Matrix3fTest);
