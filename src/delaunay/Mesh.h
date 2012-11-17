// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH__INCLUDED
#define DELAUNAY__MESH__INCLUDED

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "MeshTypes.h"

namespace delaunay {

class Mesh;
typedef boost::shared_ptr<Mesh> MeshPtr;

// Mesh
//
// A 2D mesh of triangles for Delaunay triangulation.

class Mesh : public boost::noncopyable
{
public:
    Mesh();
    ~Mesh();

    // Make a copy of the mesh.
    MeshPtr clone();

    // Vertices.
    VertexPtr createVertex();
    void destroyVertex(VertexPtr vertexPtr);
    VertexPtr vertexBegin();
    VertexPtr vertexEnd();
    VertexList::size_type vertexCount() const;

    // Edges.
    EdgePtr createEdge();
    void destroyEdge(EdgePtr edgePtr);
    EdgePtr edgeBegin();
    EdgePtr edgeEnd();
    EdgeList::size_type edgeCount() const;

    // Faces.
    FacePtr createFace();
    void destroyFace(FacePtr facePtr);
    FacePtr faceBegin();
    FacePtr faceEnd();
    FaceList::size_type faceCount() const;

private:
    VertexList mVertexList;
    EdgeList mEdgeList;
    FaceList mFaceList;

    int mVertexCount;
    int mEdgeCount;
    int mFaceCount;
};

} // namespace delaunay

#endif // DELAUNAY__MESH__INCLUDED
