// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__VERTEX__INCLUDED
#define DELAUNAY__VERTEX__INCLUDED

#include <vector>
#include <cstddef>

#include <cgmath/Vector2f.h>

#include "MeshTypes.h"
#include "Mesh.h"

namespace delaunay {

// Vertex
//
// A vertex in a 2D mesh.

class Vertex
{
public:
    Vertex();
    ~Vertex();

    // Return a copy of the vertex as an element of the specified mesh.
    VertexPtr clone(MeshPtr meshPtr) const;

    // The position of the vertex.
    void setPosition(const cgmath::Vector2f &position);
    const cgmath::Vector2f &position() const;

    // The edges adjacent to this vertex.
    void addAdjacentEdge(EdgePtr edgePtr);
    void removeAdjacentEdge(EdgePtr edgePtr);
    int adjacentEdgeCount() const;
    EdgePtr adjacentEdge(int index) const;

    // The faces adjacent to this vertex.
    void addAdjacentFace(FacePtr facePtr);
    void removeAdjacentFace(FacePtr facePtr);
    int adjacentFaceCount() const;
    FacePtr adjacentFace(int index) const;

    // The input vertex index for delaunay::Triangulator.
    void setInputVertexIndex(const size_t &inputVertexIndex);
    const size_t &inputVertexIndex() const;
    bool inputVertexIndexIsDefined() const;

private:
    cgmath::Vector2f mPosition;

    std::vector<EdgePtr> mAdjacentEdgePtrVector;
    std::vector<FacePtr> mAdjacentFacePtrVector;

    size_t mInputVertexIndex;
};

} // namespace delaunay

#endif // DELAUNAY__VERTEX__INCLUDED
