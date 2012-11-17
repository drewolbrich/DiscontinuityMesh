// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>

#include <boost/shared_ptr.hpp>

#include <cgmath/Vector2f.h>
#include <delaunay/PolygonTriangulator.h>
#include <delaunay/ValidationDetails.h>

using delaunay::PolygonTriangulator;
using cgmath::Vector2f;

class PolygonTriangulatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(PolygonTriangulatorTest);
    CPPUNIT_TEST(testConvexPolygon);
    CPPUNIT_TEST(testConcavePolygon);
    CPPUNIT_TEST(testMultiplePolygons);
    CPPUNIT_TEST(testConvexPolygonWithHoles);
    CPPUNIT_TEST(testValidateIsValid);
    CPPUNIT_TEST(testValidateExteriorPerimeterIsNotCounterclockwise);
    CPPUNIT_TEST_SUITE_END();

private:
    boost::shared_ptr<PolygonTriangulator> mPolygonTriangulator;
    PolygonTriangulator::PointVector mPointVector;
    PolygonTriangulator::PolygonVector mPolygonVector;

public:
    void setUp() {
        mPolygonTriangulator.reset(new PolygonTriangulator);
        mPointVector.clear();
        mPolygonTriangulator->setInputPointVector(&mPointVector);
        mPolygonVector.clear();
        mPolygonTriangulator->setInputPolygonVector(&mPolygonVector);
    }

    void tearDown() {
        mPolygonTriangulator.reset();
    }

    void testConvexPolygon() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(10, 10));
        mPointVector.push_back(Vector2f(0, 10));
        PolygonTriangulator::Polygon polygon;
        polygon.mPointIndexVector.push_back(0);
        polygon.mPointIndexVector.push_back(1);
        polygon.mPointIndexVector.push_back(2);
        polygon.mPointIndexVector.push_back(3);
        mPolygonVector.push_back(polygon);
        mPolygonTriangulator->triangulate();
        CPPUNIT_ASSERT(mPolygonTriangulator->outputEdgeVector().size() == 5);
        CPPUNIT_ASSERT(mPolygonTriangulator->outputTriangleVector().size() == 2);
    }

    void testConcavePolygon() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(3, 3));
        mPointVector.push_back(Vector2f(0, 10));
        PolygonTriangulator::Polygon polygon;
        polygon.mPointIndexVector.push_back(0);
        polygon.mPointIndexVector.push_back(1);
        polygon.mPointIndexVector.push_back(2);
        polygon.mPointIndexVector.push_back(3);
        mPolygonVector.push_back(polygon);
        mPolygonTriangulator->triangulate();
        CPPUNIT_ASSERT(mPolygonTriangulator->outputEdgeVector().size() == 5);
        CPPUNIT_ASSERT(mPolygonTriangulator->outputTriangleVector().size() == 2);
    }

    void testMultiplePolygons() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(0, 10));
        mPointVector.push_back(Vector2f(20, 0));
        mPointVector.push_back(Vector2f(30, 0));
        mPointVector.push_back(Vector2f(20, 10));
        PolygonTriangulator::Polygon polygon;
        polygon.mPointIndexVector.push_back(0);
        polygon.mPointIndexVector.push_back(1);
        polygon.mPointIndexVector.push_back(2);
        mPolygonVector.push_back(polygon);
        polygon.mPointIndexVector.clear();
        polygon.mPointIndexVector.push_back(3);
        polygon.mPointIndexVector.push_back(4);
        polygon.mPointIndexVector.push_back(5);
        mPolygonVector.push_back(polygon);
        mPolygonTriangulator->triangulate();
        CPPUNIT_ASSERT(mPolygonTriangulator->outputEdgeVector().size() == 6);
        CPPUNIT_ASSERT(mPolygonTriangulator->outputTriangleVector().size() == 2);
    }

    void testConvexPolygonWithHoles() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(10, 10));
        mPointVector.push_back(Vector2f(0, 10));
        mPointVector.push_back(Vector2f(4, 4));
        mPointVector.push_back(Vector2f(6, 4));
        mPointVector.push_back(Vector2f(6, 6));
        mPointVector.push_back(Vector2f(4, 6));
        PolygonTriangulator::Polygon polygon;
        polygon.mPointIndexVector.push_back(0);
        polygon.mPointIndexVector.push_back(1);
        polygon.mPointIndexVector.push_back(2);
        polygon.mPointIndexVector.push_back(3);
        mPolygonVector.push_back(polygon);
        polygon.mPointIndexVector.clear();
        polygon.mPointIndexVector.push_back(7);
        polygon.mPointIndexVector.push_back(6);
        polygon.mPointIndexVector.push_back(5);
        polygon.mPointIndexVector.push_back(4);
        mPolygonVector.push_back(polygon);
        mPolygonTriangulator->triangulate();
        CPPUNIT_ASSERT(mPolygonTriangulator->outputEdgeVector().size() == 16);
        CPPUNIT_ASSERT(mPolygonTriangulator->outputTriangleVector().size() == 8);
    }

    void testValidateIsValid() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(10, 10));
        mPointVector.push_back(Vector2f(0, 10));
        PolygonTriangulator::Polygon polygon;
        polygon.mPointIndexVector.push_back(0);
        polygon.mPointIndexVector.push_back(1);
        polygon.mPointIndexVector.push_back(2);
        polygon.mPointIndexVector.push_back(3);
        mPolygonVector.push_back(polygon);
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(mPolygonTriangulator->validate(&validationDetails));
        CPPUNIT_ASSERT(validationDetails.isValid());
    }

    void testValidateExteriorPerimeterIsNotCounterclockwise() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(10, 10));
        mPointVector.push_back(Vector2f(0, 10));
        PolygonTriangulator::Polygon polygon;
        polygon.mPointIndexVector.push_back(3);
        polygon.mPointIndexVector.push_back(2);
        polygon.mPointIndexVector.push_back(1);
        polygon.mPointIndexVector.push_back(0);
        mPolygonVector.push_back(polygon);
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(!mPolygonTriangulator->validate(&validationDetails));
        CPPUNIT_ASSERT(!validationDetails.isValid());
        CPPUNIT_ASSERT(validationDetails.exteriorPerimeterIsNotCounterclockwise());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PolygonTriangulatorTest);
