// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__POLYGON_TRIANGULATOR__INCLUDED
#define DELAUNAY__POLYGON_TRIANGULATOR__INCLUDED

#include <vector>
#include <string>

#include <cgmath/Vector2f.h>

#include "PointTriangulator.h"

namespace delaunay {

class ValidationDetails;

// PolygonTriangulator
//
// Triangulates a set of arbitrary 2D polygons with holes.

class PolygonTriangulator
{
public:
    PolygonTriangulator();
    ~PolygonTriangulator();

    // The vector of 2D points to triangulate.
    // None of the points may be coincident.
    // It must not be the case that the points are all colinear.
    // It's legal for one or more of the vertices to not be referenced
    // by the input polygons. These vertices will be triangulated as well.
    typedef std::vector<cgmath::Vector2f> PointVector;
    void setInputPointVector(const PointVector *inputPointVector);
    const PointVector *inputPointVector() const;

    // Each polygon is defined as a vector of indices into the
    // input point vector. Polygon edges must not self-intersect.
    // Polygon exterior edges must be defined as a sequence of counterclockwise 
    // points. Polygon interior edges (holes) are defined as a sequence
    // of clockwise edges.
    struct Polygon {
        std::vector<size_t> mPointIndexVector;
    };
    typedef std::vector<Polygon> PolygonVector;
    void setInputPolygonVector(const PolygonVector *inputPolygonVector);
    const PolygonVector *inputPolygonVector() const;

    // Set an optional vector of additional input edges, interior to
    // the polygons, to further constrain the triangulation.
    struct Edge {
        size_t mPointIndexArray[2];

        bool operator==(const Edge &rhs) {
            return mPointIndexArray[0] == rhs.mPointIndexArray[0]
                && mPointIndexArray[1] == rhs.mPointIndexArray[1];
        }
    };
    typedef std::vector<Edge> EdgeVector;
    void setInputEdgeVector(const EdgeVector *inputEdgeVector);
    const EdgeVector *inputEdgeVector() const;

    // Optional name of an SVG file to write a debugging image to.
    void setSvgFilename(const std::string &svgFilename);
    const std::string &svgFilename() const;

    // Returns true if the input data is valid.
    bool validate(ValidationDetails *validationDetails = NULL);

    // Triangulate the polygons.
    void triangulate();

    // Return the calculated vector of edges. Each edge is represented
    // as two indices into the input point vector.
    const EdgeVector &outputEdgeVector() const;

    // Return the calculated vector of triangles. Each triangle is represented
    // as three indices into the input point vector.
    struct Triangle { 
        size_t mPointIndexArray[3];
        size_t mEdgeIndexArray[3];
    };
    typedef std::vector<Triangle> TriangleVector;
    const TriangleVector &outputTriangleVector() const;

private:
    // Create the vector of input edges to the PointTriangulator.
    void createAggregateInputEdgeVector();

    // Create the vector of edges that contain references
    // to adjacent faces.
    void createConnectivityEdgeVector();

    // Create the output triangle vector.
    void createOutputTriangleVector();

    // Create the output edge vector.
    void createOutputEdgeVector();

    // Return true if the edge index references one of the input polygon edges.
    bool edgeIsInputPolygonEdge(size_t index) const;

    // Set the orientation of a triangle and all of its neighbors via a flood fill
    // operation, stopping at constraining edges.
    enum TriangleOrientation {
        UNDETERMINED,
        INSIDE,
        OUTSIDE
    };
    void setTriangleNeighborhoodOrientation(size_t triangleIndex, 
        TriangleOrientation triangleOrientation);

    // Write the output mesh to an SVG file.
    void writeToSvgFile();

    const PointVector *mInputPointVector;
    const PolygonVector *mInputPolygonVector;
    const EdgeVector *mInputEdgeVector;

    std::string mSvgFilename;
    PointTriangulator::EdgeVector mAggregateInputEdgeVector;
    PointTriangulator mPointTriangulator;

    struct ConnectivityEdge {
        ConnectivityEdge() : mTriangleCount(0) {
            mTriangleIndexArray[0] = 0;
            mTriangleIndexArray[1] = 0;
        }
        size_t mTriangleCount;
        size_t mTriangleIndexArray[2];
    };
    typedef std::vector<ConnectivityEdge> ConnectivityEdgeVector;
    ConnectivityEdgeVector mConnectivityEdgeVector;

    std::vector<TriangleOrientation> mTriangleOrientationVector;

    EdgeVector mOutputEdgeVector;
    TriangleVector mOutputTriangleVector;

    // An array of boolean values indicating that an
    // input edge should appear in the vector of output edges.
    std::vector<bool> mShouldOutputPointTriangulatorOutputEdgeVector;

    // An array of indices that map
    // edges output from the point triangulator to edges
    // that will be output from the polygon triangulator.
    std::vector<size_t> mPointTriangulatorOutputEdgeToPolygonTriangularOutputEdgeVector;
};

} // namespace delaunay

#endif // DELAUNAY__POLYGON_TRIANGULATOR__INCLUDED
