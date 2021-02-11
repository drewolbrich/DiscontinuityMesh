// Copyright 2008 Drew Olbrich

#include <sstream>
#include <cppunit/extensions/HelperMacros.h>

#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/Matrix4f.h>

using cgmath::Vector3f;
using cgmath::Vector4f;
using cgmath::Matrix3f;
using cgmath::Matrix4f;

class Matrix4fTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Matrix4fTest);
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
    CPPUNIT_TEST(testVector3fPremultiplyOperator);
    CPPUNIT_TEST(testVector3fPremultiplyOperatorWithDivide);
    CPPUNIT_TEST(testVector4fPremultiplyOperator);
    CPPUNIT_TEST(testVector3fPostmultiplyOperator);
    CPPUNIT_TEST(testVector4fPostmultiplyOperator);
    CPPUNIT_TEST(testMatrixMultiplyOperator);
    CPPUNIT_TEST(testScalarDivideOperator);
    CPPUNIT_TEST(testScalarDivideEqualsOperator);
    CPPUNIT_TEST(testTranspose);
    CPPUNIT_TEST(testInverse);
    CPPUNIT_TEST(testAffineInverse);
    CPPUNIT_TEST(testAdjugate);
    CPPUNIT_TEST(testDeterminant);
    CPPUNIT_TEST(testLeftShiftOperator);
    CPPUNIT_TEST(testZero);
    CPPUNIT_TEST(testIdentity);
    CPPUNIT_TEST(testTranslation);
    CPPUNIT_TEST(testRotation);
    CPPUNIT_TEST(testScale);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructor() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        CPPUNIT_ASSERT(a == Matrix4f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    }

    void testCopyConstructor() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(a);
        CPPUNIT_ASSERT(a == b);
    }

    void testConstructorFromColumnVectors() {
        const Matrix4f a(
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,
            13, 14, 15, 16);
        const Vector4f v1(1, 5, 9, 13);
        const Vector4f v2(2, 6, 10, 14);
        const Vector4f v3(3, 7, 11, 15);
        const Vector4f v4(4, 8, 12, 16);
        const Matrix4f b(v1, v2, v3, v4);
        CPPUNIT_ASSERT(a == b);
    }

    void testConstructorFromPointer() {
        const float data[16] = {
            1, 5,  9, 13,
            2, 6, 10, 14,
            3, 7, 11, 15,
            4, 8, 12, 16
        };
        const Matrix4f a(data);
        CPPUNIT_ASSERT(a == Matrix4f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    }

    void testConstructorFromArray() {
        const float data[4][4] = {
            { 1, 5,  9, 13 },
            { 2, 6, 10, 14 },
            { 3, 7, 11, 15 },
            { 4, 8, 12, 16 }
        };
        const Matrix4f a(data);
        CPPUNIT_ASSERT(a == Matrix4f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    }

    void testAsFloatPtr() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const float *fp = a.asFloatPtr();
        CPPUNIT_ASSERT(fp[0] == 1);
        CPPUNIT_ASSERT(fp[1] == 5);
        CPPUNIT_ASSERT(fp[2] == 9);
        CPPUNIT_ASSERT(fp[3] == 13);
        CPPUNIT_ASSERT(fp[4] == 2);
        CPPUNIT_ASSERT(fp[5] == 6);
        CPPUNIT_ASSERT(fp[6] == 10);
        CPPUNIT_ASSERT(fp[7] == 14);
        CPPUNIT_ASSERT(fp[8] == 3);
        CPPUNIT_ASSERT(fp[9] == 7);
        CPPUNIT_ASSERT(fp[10] == 11);
        CPPUNIT_ASSERT(fp[11] == 15);
        CPPUNIT_ASSERT(fp[12] == 4);
        CPPUNIT_ASSERT(fp[13] == 8);
        CPPUNIT_ASSERT(fp[14] == 12);
        CPPUNIT_ASSERT(fp[15] == 16);
    }

    void testConstParenthesisOperator() {
        const Matrix4f a(
             1,  2,  3,  4,
             5,  6,  7,  8, 
             9, 10, 11, 12,
            13, 14, 15, 16);
        CPPUNIT_ASSERT(a(0, 0) == 1);
        CPPUNIT_ASSERT(a(0, 1) == 2);
        CPPUNIT_ASSERT(a(0, 2) == 3);
        CPPUNIT_ASSERT(a(0, 3) == 4);
        CPPUNIT_ASSERT(a(1, 0) == 5);
        CPPUNIT_ASSERT(a(1, 1) == 6);
        CPPUNIT_ASSERT(a(1, 2) == 7);
        CPPUNIT_ASSERT(a(1, 3) == 8);
        CPPUNIT_ASSERT(a(2, 0) == 9);
        CPPUNIT_ASSERT(a(2, 1) == 10);
        CPPUNIT_ASSERT(a(2, 2) == 11);
        CPPUNIT_ASSERT(a(2, 3) == 12);
        CPPUNIT_ASSERT(a(3, 0) == 13);
        CPPUNIT_ASSERT(a(3, 1) == 14);
        CPPUNIT_ASSERT(a(3, 2) == 15);
        CPPUNIT_ASSERT(a(3, 3) == 16);
    }

    void testParenthesisOperator() {
        Matrix4f a;
        a(0, 0) = 1;
        a(0, 1) = 2;
        a(0, 2) = 3;
        a(0, 3) = 4;
        a(1, 0) = 5;
        a(1, 1) = 6;
        a(1, 2) = 7;
        a(1, 3) = 8;
        a(2, 0) = 9;
        a(2, 1) = 10;
        a(2, 2) = 11;
        a(2, 3) = 12;
        a(3, 0) = 13;
        a(3, 1) = 14;
        a(3, 2) = 15;
        a(3, 3) = 16;
        CPPUNIT_ASSERT(a == Matrix4f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    }

    void testEqualsOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f c(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        CPPUNIT_ASSERT(a == b);
        CPPUNIT_ASSERT(!(a == c));
    }

    void testNotEqualsOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f c(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        CPPUNIT_ASSERT(a != c);
        CPPUNIT_ASSERT(!(a != b));
    }

    void testAssignmentOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        Matrix4f b(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        CPPUNIT_ASSERT(b == Matrix4f(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
        b = a;
        CPPUNIT_ASSERT(b == Matrix4f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    }

    void testSet() {
        Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        CPPUNIT_ASSERT(a == Matrix4f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
        a.set(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        CPPUNIT_ASSERT(a == Matrix4f(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
    }

    void testPlusOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        Matrix4f c = a + b;
        CPPUNIT_ASSERT(c == Matrix4f(1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31));
    }

    void testPlusEqualsOperator() {
        Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        a += b;
        CPPUNIT_ASSERT(a == Matrix4f(1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31));
    }

    void testMinusOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        Matrix4f c = a - b;
        CPPUNIT_ASSERT(c == Matrix4f(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1));
    }

    void testMinusEqualsOperator() {
        Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        const Matrix4f b(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        a -= b;
        CPPUNIT_ASSERT(a == Matrix4f(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1));
    }

    void testUnaryMinusOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        CPPUNIT_ASSERT(-a == Matrix4f(-1, -2, -3, -4, -5, -6, -7, -8, -9, -10, 
                -11, -12, -13, -14, -15, -16));
    }

    void testScalarMultiplyOperator() {
        const Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        CPPUNIT_ASSERT(3*a == Matrix4f(3, 6, 9, 12, 15, 18, 21, 24, 27, 
                30, 33, 36, 39, 42, 45, 48));
        CPPUNIT_ASSERT(a*3 == Matrix4f(3, 6, 9, 12, 15, 18, 21, 24, 27,
                30, 33, 36, 39, 42, 45, 48));
    }

    void testScalarMultiplyEqualsOperator() {
        Matrix4f a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        a *= 3;
        CPPUNIT_ASSERT(a == Matrix4f(3, 6, 9, 12, 15, 18, 21, 24, 27,
                30, 33, 36, 39, 42, 45, 48));
    }

    void testVector3fPremultiplyOperator() {
        const Matrix4f a(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 0, 0, 0, 1);
        const Vector3f v3(1, 2, 3);
        CPPUNIT_ASSERT(a*v3 == Vector3f(51, 58, 65));
    }

    void testVector3fPremultiplyOperatorWithDivide() {
        const Matrix4f a(
            1, 0, 0, 0,
            0, 0, -1, 0,
            0, 1, 0, 0,
            0, 0, 0, 2);
        const Vector3f v3(2, 4, 6);
        // This assumes that after the multiplication, the X, Y, and Z components
        // of the resulting 4-vector is divided by its homogeneous component W,
        // to form a 3-vector.
        CPPUNIT_ASSERT(a*v3 == Vector3f(1, -3, 2));
    }

    void testVector4fPremultiplyOperator() {
        const Matrix4f a(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 0, 0, 0, 1);
        const Vector4f v4(1, 2, 3, 4);
        CPPUNIT_ASSERT(a*v4 == Vector4f(90, 100, 110, 4));
    }

    void testVector3fPostmultiplyOperator() {
        const Matrix4f a(1, 5, 9, 0, 2, 6, 10, 0, 3, 7, 11, 0, 4, 8, 12, 1);
        const Vector3f v3(1, 2, 3);
        CPPUNIT_ASSERT(v3*a == Vector3f(18, 46, 74));
    }

    void testVector4fPostmultiplyOperator() {
        const Matrix4f a(1, 5, 9, 0, 2, 6, 10, 0, 3, 7, 11, 0, 4, 8, 12, 1);
        const Vector4f v4(1, 2, 3, 4);
        CPPUNIT_ASSERT(v4*a == Vector4f(30, 70, 110, 4));
    }

    void testMatrixMultiplyOperator() {
        const Matrix4f a(
            8, 5, 1, 3, 
            9, 3, 1, 5, 
            4, 2, 1, 3, 
            1, 5, 4, 3);
        const Matrix4f b(
            5, 1, 4, 2, 
            1, 5, 8, 2, 
            1, 3, 4, 3, 
            2, 5, 2, 3);
        const Matrix4f d(
            52, 51, 82, 38, 
            59, 52, 74, 42,
            29, 32, 42, 24, 
            20, 53, 66, 33);
        CPPUNIT_ASSERT(a*b == d);
    }

    void testScalarDivideOperator() {
        const Matrix4f a(2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32);
        const Matrix4f b(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        CPPUNIT_ASSERT(a/2 == b);
    }

    void testScalarDivideEqualsOperator() {
        Matrix4f a(2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32);
        const Matrix4f b(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
        a /= 2;
        CPPUNIT_ASSERT(a == b);
    }

    void testTranspose() {
        const Matrix4f a(
            2,  4,  6,  8, 
            10, 12, 14, 16, 
            18, 20, 22, 24,
            26, 28, 30, 32);
        const Matrix4f b(
            2, 10, 18, 26, 
            4, 12, 20, 28,
            6, 14, 22, 30, 
            8, 16, 24, 32);
        CPPUNIT_ASSERT(a.transpose() == b);
        CPPUNIT_ASSERT(b.transpose() == a);
    }

    void testInverse() {
        const Matrix4f a(
            1, 2, 0, 0,
            1, 1, 1, 1,
            0, 2, 0, 2,
            2, 1, 1, 1);
        CPPUNIT_ASSERT(a.inverse() == Matrix4f(
                           0, -1, 0, 1,
                           0.5, 0.5, 0, -0.5,
                           0, 2, -0.5, -1,
                           -0.5, -0.5, 0.5, 0.5));
    }

    void testAffineInverse() {
        const Matrix4f a(
            1, 2, 0, 0,
            1, 1, 1, 1,
            0, 2, 0, 2,
            0, 0, 0, 1);
        CPPUNIT_ASSERT(a.affineInverse() == Matrix4f(
                           1, 0, -1, 2,
                           0, 0, 0.5, -1,
                           -1, 1, 0.5, -2,
                           0, 0, 0, 1));
    }

    void testAdjugate() {
        const Matrix4f a(
            8, 9, 4, 1, 
            5, 3, 2, 5, 
            1, 1, 1, 4, 
            3, 5, 3, 3);
        CPPUNIT_ASSERT(a.adjugate()
            == Matrix4f(
                 -3,  15,  -18,   0,
                 27, -36,   63, -33,
                -48,  53, -112,  77,
                 6,  -8,   25, -11));
    }

    void testDeterminant() {
        const Matrix4f a(
            8, 9, 4, 1, 
            5, 3, 2, 5, 
            1, 1, 1, 4, 
            3, 5, 3, 3);
        CPPUNIT_ASSERT(a.determinant() == 33);
    }

    void testLeftShiftOperator() {
        const Matrix4f a(
             1,  2,  3,  4,
             5,  6,  7,  8, 
             9, 10, 11, 12,
            13, 14, 15, 16);
        std::ostringstream ostr;
        ostr << a;
        CPPUNIT_ASSERT(ostr.str() == " 1  2  3  4\n 5  6  7  8\n 9 10 11 12\n13 14 15 16\n");
    }

    void testZero() {
        CPPUNIT_ASSERT(Matrix4f::ZERO 
            == Matrix4f(
                0, 0, 0, 0,
                0, 0, 0, 0, 
                0, 0, 0, 0, 
                0, 0, 0, 0));
    }

    void testIdentity() {
        CPPUNIT_ASSERT(Matrix4f::IDENTITY
            == Matrix4f(
                1, 0, 0, 0,
                0, 1, 0, 0, 
                0, 0, 1, 0, 
                0, 0, 0, 1));
    }

    void testTranslation() {
        const Matrix4f t(Matrix4f::fromTranslation(Vector3f(1, 2, 3)));
        CPPUNIT_ASSERT(t == Matrix4f(
                           1, 0, 0, 1, 
                           0, 1, 0, 2, 
                           0, 0, 1, 3,
                           0, 0, 0, 1));
    }

    void testRotation() {
        const Matrix4f r(Matrix4f::fromRotation(Vector3f(0, 0, 1), 3.1415927F/2.0));
        const Vector3f v(1, 0, 0);
        const Vector3f w = r*v;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(w[0], 0, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(w[1], 1, 0.0001);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(w[2], 0, 0.0001);
    }

    void testScale() {
        const Matrix4f t(Matrix4f::fromScale(Vector3f(1, 2, 3)));
        CPPUNIT_ASSERT(t == Matrix4f(
                        1, 0, 0, 0, 
                        0, 2, 0, 0, 
                        0, 0, 3, 0,
                        0, 0, 0, 1));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Matrix4fTest);
