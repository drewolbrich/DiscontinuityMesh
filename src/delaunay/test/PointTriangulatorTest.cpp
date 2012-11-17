// Copyright 2008 Retroactive Fiasco.

#include <cmath>
#include <cppunit/extensions/HelperMacros.h>

#include <delaunay/PointTriangulator.h>
#include <delaunay/ValidationDetails.h>
#include <cgmath/Constants.h>

using delaunay::PointTriangulator;
using cgmath::Vector2f;

class TriangulatorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TriangulatorTest);
    CPPUNIT_TEST(testThreePoints);
    CPPUNIT_TEST(testFourPoints);
    CPPUNIT_TEST(testFourCocircularPoints);
    CPPUNIT_TEST(testFourPointsThreeColinear);
    CPPUNIT_TEST(testAlignedGrid);
    CPPUNIT_TEST(testRotatedGrid);
    CPPUNIT_TEST(testPreexistingConstrainedEdges);
    CPPUNIT_TEST(testOneConstrainedEdge);
    CPPUNIT_TEST(testSeveralConstrainedEdges);
    CPPUNIT_TEST(testValidateIsValid);
    CPPUNIT_TEST(testValidateHasLessThanThreePoints);
    CPPUNIT_TEST(testValidateSomePointsAreCoincident);
    CPPUNIT_TEST(testValidateAllPointsAreColinear);
    CPPUNIT_TEST(testValidateSomePointsAreNaN);
    CPPUNIT_TEST(testValidateSomeEdgesIntersect);
    CPPUNIT_TEST_SUITE_END();

private:
    PointTriangulator::PointVector mPointVector;
    PointTriangulator::EdgeVector mEdgeVector;
    PointTriangulator mPointTriangulator;

public:
    void setUp() {
        mPointVector.clear();
        mPointTriangulator.setInputPointVector(&mPointVector);
        mPointTriangulator.setInputEdgeVector(&mEdgeVector);
        mPointTriangulator.setShouldShuffleInputPoints(false);
    }

    void tearDown() {
    }

    void testThreePoints() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(0, 10));
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 3);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 1);
    }

    void testFourPoints() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(0, 10));
        mPointVector.push_back(Vector2f(15, 15));
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 5);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 2);
    }

    void testFourCocircularPoints() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(0, 10));
        mPointVector.push_back(Vector2f(10, 10));
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 5);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 2);
    }

    void testFourPointsThreeColinear() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(20, 0));
        mPointVector.push_back(Vector2f(10, 10));
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 5);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 2);
    }

    void testAlignedGrid() {
        for (int x = 0; x < 4; ++x) {
            for (int y = 0; y < 4; ++y) {
                mPointVector.push_back(Vector2f(x, y));
            }
        }
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 33);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 18);
    }

    void testRotatedGrid() {
        float degrees = 3.0;
        float radians = degrees*cgmath::PI/180.0;
        static const int COLUMNS = 4;
        static const int ROWS = 4;
        for (int x = 0; x < COLUMNS; ++x) {
            for (int y = 0; y < ROWS; ++y) {
                float fx = cos(radians)*x - sin(radians)*y;
                float fy = sin(radians)*x + cos(radians)*y;
                mPointVector.push_back(Vector2f(fx, fy));
            }
        }
        mPointTriangulator.triangulate();
        // We can't really predict how many edges or triangles
        // there will be, because of floating point roundoff error.
        // A number of sliver polygons will be created around the perimeter
        // of the mesh.
        // Mostly, we are simply checking to see if the call
        // to PointTriangulator::triangulate crashes.
        // We can, however, test the Euler characteristic of the resulting mesh,
        // which must be 1.
        CPPUNIT_ASSERT(ROWS*COLUMNS - mPointTriangulator.outputEdgeVector().size()
            + mPointTriangulator.outputTriangleVector().size() == 1);
    }

    void testPreexistingConstrainedEdges() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 0));
        mPointVector.push_back(Vector2f(0, 15));
        addEdge(0, 1);
        addEdge(1, 2);
        addEdge(2, 0);
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 3);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 1);
    }

    void testOneConstrainedEdge() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(20, 0));
        mPointVector.push_back(Vector2f(10, 5));
        mPointVector.push_back(Vector2f(10, -5));
        addEdge(0, 1);
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 5);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 2);

        // Check to see if the edge we requested actually exists in the output,
        // even though it's not an edge that would satisfy the
        // Delaunay criterion.
        bool found = false;
        const PointTriangulator::EdgeVector &edgeVector = mPointTriangulator.outputEdgeVector();
        for (size_t index = 0; index < edgeVector.size(); ++index) {
            const PointTriangulator::Edge &edge = edgeVector[index];
            if ((edge.mPointIndexArray[0] == 0
                    && edge.mPointIndexArray[1] == 1)
                || (edge.mPointIndexArray[0] == 1
                    && edge.mPointIndexArray[1] == 0)) {
                found = true;
            }
        }
        assert(found);
    }

    void testSeveralConstrainedEdges() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 0));
        mPointVector.push_back(Vector2f(0, 15));
        mPointVector.push_back(Vector2f(30, 15));
        mPointVector.push_back(Vector2f(0, -15));
        mPointVector.push_back(Vector2f(30, -15));
        mPointVector.push_back(Vector2f(7, 3));
        mPointVector.push_back(Vector2f(22, 8));
        mPointVector.push_back(Vector2f(9, -2));
        mPointVector.push_back(Vector2f(13, 13));
        addEdge(2, 3);
        addEdge(4, 5);
        addEdge(0, 2);
        addEdge(0, 4);
        addEdge(1, 3);
        addEdge(1, 5);
        addEdge(2, 5);
        mPointTriangulator.setShouldShuffleInputPoints(true);
        mPointTriangulator.triangulate();
        CPPUNIT_ASSERT(mPointTriangulator.outputEdgeVector().size() == 21);
        CPPUNIT_ASSERT(mPointTriangulator.outputTriangleVector().size() == 12);

        const delaunay::PointTriangulator::EdgeVector &edgeVector 
            = mPointTriangulator.outputEdgeVector();
        CPPUNIT_ASSERT(edgeVector[0].mPointIndexArray[0] == 2);
        CPPUNIT_ASSERT(edgeVector[0].mPointIndexArray[1] == 3);
        CPPUNIT_ASSERT(edgeVector[1].mPointIndexArray[0] == 4);
        CPPUNIT_ASSERT(edgeVector[1].mPointIndexArray[1] == 5);
        CPPUNIT_ASSERT(edgeVector[2].mPointIndexArray[0] == 0);
        CPPUNIT_ASSERT(edgeVector[2].mPointIndexArray[1] == 2);
        CPPUNIT_ASSERT(edgeVector[3].mPointIndexArray[0] == 0);
        CPPUNIT_ASSERT(edgeVector[3].mPointIndexArray[1] == 4);
        CPPUNIT_ASSERT(edgeVector[4].mPointIndexArray[0] == 1);
        CPPUNIT_ASSERT(edgeVector[4].mPointIndexArray[1] == 3);
        CPPUNIT_ASSERT(edgeVector[5].mPointIndexArray[0] == 1);
        CPPUNIT_ASSERT(edgeVector[5].mPointIndexArray[1] == 5);
        CPPUNIT_ASSERT(edgeVector[6].mPointIndexArray[0] == 2);
        CPPUNIT_ASSERT(edgeVector[6].mPointIndexArray[1] == 5);
    }

    void testValidateIsValid() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 0));
        mPointVector.push_back(Vector2f(0, 15));
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(mPointTriangulator.validate(&validationDetails));
        CPPUNIT_ASSERT(validationDetails.isValid());
    }

    void testValidateHasLessThanThreePoints() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 0));
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(!mPointTriangulator.validate(&validationDetails));
        CPPUNIT_ASSERT(!validationDetails.isValid());
        CPPUNIT_ASSERT(validationDetails.hasLessThanThreePoints());
    }

    void testValidateSomePointsAreCoincident() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 0));
        mPointVector.push_back(Vector2f(30, 0));
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(!mPointTriangulator.validate(&validationDetails));
        CPPUNIT_ASSERT(!validationDetails.isValid());
        CPPUNIT_ASSERT(validationDetails.somePointsAreCoincident());
    }

    void testValidateAllPointsAreColinear() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 5));
        mPointVector.push_back(Vector2f(60, 10));
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(!mPointTriangulator.validate(&validationDetails));
        CPPUNIT_ASSERT(!validationDetails.isValid());
        CPPUNIT_ASSERT(validationDetails.allPointsAreColinear());
    }

    void testValidateSomePointsAreNaN() {
        float negativeOne = -1.0;
        float nan = sqrt(negativeOne);
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(30, 5));
        mPointVector.push_back(Vector2f(60, nan));
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(!mPointTriangulator.validate(&validationDetails));
        CPPUNIT_ASSERT(!validationDetails.isValid());
        CPPUNIT_ASSERT(validationDetails.somePointsAreNaN());
    }

    void testValidateSomeEdgesIntersect() {
        mPointVector.push_back(Vector2f(0, 0));
        mPointVector.push_back(Vector2f(10, 0));
        mPointVector.push_back(Vector2f(10, 10));
        mPointVector.push_back(Vector2f(0, 10));
        addEdge(0, 1);
        addEdge(1, 3);
        addEdge(3, 2);
        addEdge(2, 0);
        delaunay::ValidationDetails validationDetails;
        CPPUNIT_ASSERT(!mPointTriangulator.validate(&validationDetails));
        CPPUNIT_ASSERT(!validationDetails.isValid());
        CPPUNIT_ASSERT(validationDetails.someEdgesIntersect());
    }

private:
    void addEdge(int i, int j) {
        PointTriangulator::Edge edge;
        edge.mPointIndexArray[0] = i;
        edge.mPointIndexArray[1] = j;
        mEdgeVector.push_back(edge);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TriangulatorTest);
