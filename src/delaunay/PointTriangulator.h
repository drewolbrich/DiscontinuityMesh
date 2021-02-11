// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__POINT_TRIANGULATOR__INCLUDED
#define DELAUNAY__POINT_TRIANGULATOR__INCLUDED

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include <cgmath/Vector2f.h>
#include <cgmath/BoundingBox2f.h>

#include "Mesh.h"
#include "MeshHistory.h"
#include "MeshSnapshot.h"

namespace delaunay {

class ValidationDetails;

// PointTriangulator
//
// A class for creating a mesh of triangles from a set of 2D points.

class PointTriangulator
{
public:
    PointTriangulator();
    ~PointTriangulator();

    // The vector of 2D points to triangulate.
    // None of the points may be coincident.
    // It must not be the case that the points are all colinear.
    typedef std::vector<cgmath::Vector2f> PointVector;
    void setInputPointVector(const PointVector *inputPointVector);
    const PointVector *inputPointVector() const;

    // This function specifies a set of edges that must exist in the output,
    // even if this means violating the Delaunay criterion.
    // If this function is called, constrained Delaunay triangulation is performed.
    // The edges must not intersect each other or any of the input points,
    // except at the endpoints of the edges.
    // The edges must all be unique.
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

    // If true, write out the entire history of the triangulation
    // into the SVG file, instead of the final mesh configuration.
    // Defaults to false.
    void setWriteEntireSvgHistory(bool writeEntireSvgHistory);
    bool writeEntireSvgHistory() const;

    // If true, shuffle the input points before inserting them
    // into the mesh, for better performance.
    // Defaults to true.
    void setShouldShuffleInputPoints(bool shouldShuffleInputPoints);
    bool shouldShuffleInputPoints() const;

    // Returns true if the input data is valid.
    bool validate(ValidationDetails *validationDetails = NULL) const;

    // Triangulate the points using incremental Delaunay triangulation.
    // If the validate function, above, returns false, this function
    // should not be called because it may crash.
    void triangulate();

    // Return the calculated vector of edges. Each edge is represented
    // as two indices into the input point vector.
    // The N input edges will always be the first N output edges.
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
    // Create the first triangle by finding three noncoincident, noncolinear
    // vertices.
    void createInitialTriangle();

    // Returns true if the specified point is exactly coincident with one of
    // the existing vertices of the mesh.
    bool findCoincidentVertex(FacePtr nearestFacePtr, 
        const cgmath::Vector2f &point, VertexPtr *resultVertexPtr);

    // Returns true if the specified point intersects one of the edges of the mesh,
    // and returns that edge as the final argument.
    bool findIntersectingEdge(FacePtr nearestFacePtr, 
        const cgmath::Vector2f &point, EdgePtr *resultEdgePtr);

    // Returns true if the specified point intersects one of the faces of the mesh,
    // and returns that face as the final argument.
    bool findIntersectingFace(FacePtr nearestFacePtr, 
        const cgmath::Vector2f &point, FacePtr *resultFacePtr);

    // Add a vertex to the mesh outside the mesh's perimeter, adding new
    // edges and faces to connect it with the existing mesh, enforcing the
    // constraint that the mesh's perimeter is convex.
    VertexPtr addVertexOutsidePerimeter(FacePtr nearestFacePtr,
        const cgmath::Vector2f &point);

    // Keep track of a new vertex that was added to the list,
    // test for mesh consistency, and then correct neighboring triangles
    // to ensure that the Delaunay criterion is maintained.
    void processNewVertex(size_t unshuffledIndex, VertexPtr vertexPtr);

    // Flip edges of all faces neighboring the specified vertex that don't
    // satisfy the Delaunay criterion.
    void correctNonDelaunayTrianglesAdjacentToVertex(VertexPtr vertexPtr);

    // Return the vertex on the mesh's perimeter that's nearest to the specified point.
    VertexPtr findNearestPerimeterVertex(FacePtr nearestFacePtr, 
        const cgmath::Vector2f &point);

    // Create the resulting vector of triangles.
    void createTriangleVector();

    // Create the resulting vector of edges.
    void createEdgeVector();

    // Create a snapshot of the mesh if mSvgFilename is defined
    // and mWriteEntireSvgHistory is true.
    // Returns true if a mesh snapshot was created.
    bool createMeshSnapshot();

    // Create the vector of shuffled indices, used to process the input
    // points in a random order.
    void createShuffledPointIndexVector();

    // Free up the vector of shuffled indices.
    void destroyShuffledPointIndexVector();

    // Given an unshuffled index, return a point from the input vector
    // after the index has been shuffled.
    const cgmath::Vector2f &getShuffledPoint(size_t unshuffledIndex) const;

    // Return the face nearest to, or containing, or possessing an edge that
    // intersects with, the specified point.
    FacePtr findNearestFace(const cgmath::Vector2f &point) const;

    // Insert constrained edges.
    void insertConstrainedEdges();

    // Insert a single edge.
    EdgePtr insertConstrainedEdge(VertexPtr v0, VertexPtr v1);

    // Given a range of vertices bounding a possibly concave half-region, 
    // such that all of the vertices within 'first' and 'last' lie between
    // the two vertices when projected onto the vector between them.
    // triangulate the region by adding new faces to the mesh.
    typedef std::vector<VertexPtr> VertexPtrVector;
    void triangulateConstrainedEdgeHalfRegion(const VertexPtrVector &vertexPtrVector,
        size_t first, size_t last);

    const PointVector *mInputPointVector;
    const EdgeVector *mInputEdgeVector;
    std::string mSvgFilename;
    bool mWriteEntireSvgHistory;

    bool mShouldShuffleInputPoints;
    std::vector<size_t> mShuffledPointIndexVector;

    EdgeVector mOutputEdgeVector;
    TriangleVector mOutputTriangleVector;

    MeshPtr mMeshPtr;
    boost::shared_ptr<MeshHistory> mMeshHistoryPtr;
    MeshSnapshotPtr mMeshSnapshotPtr;

    // A vector of VertexPtr's corresponding to the vertices
    // that have been added to the mesh so far,
    // in the order that they have been added.
    VertexPtrVector mNearestFaceVertexPtrVector;

    // A vector of VertexPtr's in an order that matches the
    // order of the vertices in mInputPointVector.
    VertexPtrVector mInputPointVectorVertexPtrVector;

    mutable unsigned mFindNearestFaceRandomContext;

    size_t mUnshuffledTriangleIndexArray[3];
};

} // namespace delaunay

#endif // DELAUNAY__POINT_TRIANGULATOR__INCLUDED
