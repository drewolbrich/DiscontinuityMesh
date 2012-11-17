// Copyright 2009 Retroactive Fiasco.

#ifndef MESHRETRI__FACE_SPLITTER__INCLUDED
#define MESHRETRI__FACE_SPLITTER__INCLUDED

#include <cstdlib>
#include <set>

#include <mesh/Mesh.h>
#include <mesh/AttributeKey.h>
#include <cgmath/Vector2f.h>
#include <delaunay/PolygonTriangulator.h>

#include "RetriangulatorFace.h"
#include "TriangleVector.h"

namespace meshretri {

class Retriangulator;
class RetriangulatorEdge;

// FaceSplitter
//
// A class that splits a mesh face using constrained Delaunay triangulation,
// given a RetriangulatorFace.

class FaceSplitter
{
public:
    FaceSplitter();
    ~FaceSplitter();

    // The mesh containing the face to be split.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // The Retriangulator that owns the FaceSplitter.
    void setRetriangulator(Retriangulator *retriangulator);
    Retriangulator *retriangulator() const;

    // The RetriangulatorFace corresponding to the face to be split.
    void setRetriangulatorFace(RetriangulatorFace *retriangulatorFace);
    RetriangulatorFace *retriangulatorFace() const;

    // Split the specified face.
    void splitFace();

    // Set the array of three RetriangulatorEdges surrounding the
    // single face that is split for backprojection.
    void setRetriangulatorEdgeArray(const RetriangulatorEdge *retriangulatorEdgeArray);
    const RetriangulatorEdge *retriangulatorEdgeArray() const;

    // Split a face for use in a backprojection. The mesh face is not 
    // actually split. Rather, the resulting CDT is returned as a vector of triangles.
    void splitBackprojectionFace(TriangleVector *triangleVector);

private:
    // Given a 3D vector, return the 2D vector in the coordinate space
    // of the CDT calculation.
    cgmath::Vector2f createVector2f(const cgmath::Vector3f &vector3f);

    // Add a CDT point, returning its CDT point index. If a point
    // already exists at this location, its index is returned.
    // A mesh vertex is created at this position if it does not already exist.
    size_t addPointFromVector3f(const cgmath::Vector3f &point, 
        bool isDegreeZeroDiscontinuity);

    // Add a CDT point constructed from a specified mesh vertex.
    size_t addPointFromVertex(mesh::VertexPtr vertexPtr);

    // Add a CDT point without creating a mesh vertex.
    size_t addBackprojectionPointFromVector3f(const cgmath::Vector3f &point);

    // Add a CDT point for a backprojection.
    size_t addBackprojectionPointFromVertex(mesh::VertexPtr vertexPtr);

    // Returns true if the specified point has already been encountered.
    bool findPoint(const cgmath::Vector2f &point2f, size_t *index);

    // Add a CDT point, given a Vector2f and a VertexPtr.
    size_t addPoint(const cgmath::Vector2f &point2f, mesh::VertexPtr vertexPtr);

    // Add a point to the Vector2ftoPointIndexMap only.
    size_t addPointToVector2fToPointIndexMap(const cgmath::Vector2f &point2f);

    void initializePolygonTriangulator();
    void createPolygonFromFaceVertices(mesh::FacePtr facePtr);
    void createPolygonFromRetriangulatorEdgeArray();
    void createEdges();
    void triangulate();

    mesh::Mesh *mMesh;
    Retriangulator *mRetriangulator;
    mesh::AttributeKey mIsDegreeZeroDiscontinuityAttributeKey;
    RetriangulatorFace *mRetriangulatorFace;
    const RetriangulatorEdge *mRetriangulatorEdgeArray;

    // Map of 2D points to CDT point indices.
    typedef std::map<cgmath::Vector2f, size_t> Vector2fToPointIndexMap;
    Vector2fToPointIndexMap mVector2fToPointIndexMap;

    // Context for constrained Delaunay triangulation.
    delaunay::PolygonTriangulator mPolygonTriangulator;
    delaunay::PolygonTriangulator::PointVector mPointVector;
    delaunay::PolygonTriangulator::PolygonVector mPolygonVector;
    delaunay::PolygonTriangulator::EdgeVector mEdgeVector;

    // Vector that maps CDT point indices to VertexPtrs.
    typedef std::vector<mesh::VertexPtr> PointIndexToVertexPtrVector;
    PointIndexToVertexPtrVector mPointIndexToVertexPtrVector;

    // Vector that maps CDT point indices to Vector3fs.
    typedef std::vector<cgmath::Vector3f> PointIndexToVector3fVector;
    PointIndexToVector3fVector mPointIndexToVector3fVector;

    // Vector that maps CDT edge indices to EdgePtrs.
    typedef std::vector<mesh::EdgePtr> EdgeIndexToEdgePtrVector;
    EdgeIndexToEdgePtrVector mEdgeIndexToEdgePtrVector;

    typedef std::pair<size_t, size_t> PointIndexPair;
    std::set<PointIndexPair> mEdgesAlreadySubmitted;
};

} // namespace meshretri

#endif // MESHRETRI__FACE_SPLITTER__INCLUDED
