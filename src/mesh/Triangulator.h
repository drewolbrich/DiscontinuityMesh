// Copyright 2009 Retroactive Fiasco.

#ifndef MESH__TRIANGULATOR__INCLUDED
#define MESH__TRIANGULATOR__INCLUDED

#include <vector>

#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <delaunay/PolygonTriangulator.h>

namespace mesh {

// Triangulator
//
// Triangulates all non-triangulator faces of a mesh.
//
// This uses Delaunay triangulation, and so cannot handle degenerate 
// or self-intersecting faces.

class Triangulator
{
public:
    Triangulator();
    ~Triangulator();

    // The mesh to triangulte.
    void setMesh(Mesh *mesh);
    Mesh *mesh() const;

    // Triangulate the mesh.
    void triangulate();

    // Return the number of faces that were triangulated.
    unsigned triangulatedFaces() const;

private:
    void initializePolygonTriangulator();

    void triangulateFace(FacePtr facePtr);

    void loadPolygonTriangulatorWithFace(FacePtr facePtr);

    void replaceMeshFaceWithTriangles(FacePtr oldFacePtr);

    Mesh *mMesh;

    // Context for constrained Delaunay triangulation.
    delaunay::PolygonTriangulator mPolygonTriangulator;
    delaunay::PolygonTriangulator::PointVector mPointVector;
    delaunay::PolygonTriangulator::PolygonVector mPolygonVector;
    delaunay::PolygonTriangulator::EdgeVector mEdgeVector;

    // Vector that maps CDT point indices to VertexPtrs.
    typedef std::vector<VertexPtr> PointIndexToVertexPtrVector;
    PointIndexToVertexPtrVector mPointIndexToVertexPtrVector;

    // Vector that maps CDT edge indices to EdgePtrs.
    typedef std::vector<EdgePtr> EdgeIndexToEdgePtrVector;
    EdgeIndexToEdgePtrVector mEdgeIndexToEdgePtrVector;

    unsigned mTriangulatedFaces;
};

} // namespace mesh

#endif // MESH__TRIANGULATOR__INCLUDED
