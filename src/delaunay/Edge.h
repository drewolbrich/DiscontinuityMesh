// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__EDGE__INCLUDED
#define DELAUNAY__EDGE__INCLUDED

#include "Mesh.h"
#include "MeshTypes.h"

namespace delaunay {

// Edge
//
// An edge in a 2D mesh.

class Edge
{
public:
    Edge();
    ~Edge();

    // Return a copy of the edge as an element of the specified mesh.
    EdgePtr clone(MeshPtr meshPtr) const;

    // The adjacent vertices.
    void addAdjacentVertex(VertexPtr vertex);
    void removeAdjacentVertex(VertexPtr vertex);
    int adjacentVertexCount() const;
    VertexPtr adjacentVertex(int index) const;

    // The adjacent faces.
    void addAdjacentFace(FacePtr face);
    void removeAdjacentFace(FacePtr face);
    int adjacentFaceCount() const;
    FacePtr adjacentFace(int index) const;

    // The index into the array of output edges.
    void setOutputEdgeIndex(size_t outputEdgeIndex);
    size_t outputEdgeIndex() const;
    bool outputEdgeIndexIsDefined() const;

    // The index into the original input array of input edges.
    void setInputEdgeIndex(size_t inputEdgeIndex);
    size_t inputEdgeIndex() const;
    bool inputEdgeIndexIsDefined() const;

private:
    short mAdjacentVertexCount;
    short mAdjacentFaceCount;

    static const int MAX_ADJACENT_VERTICES = 2;
    static const int MAX_ADJACENT_FACES = 2;

    VertexPtr mAdjacentVertexPtr[MAX_ADJACENT_VERTICES];
    FacePtr mAdjacentFacePtr[MAX_ADJACENT_FACES];

    size_t mOutputEdgeIndex;
    size_t mInputEdgeIndex;
};

} // namespace delaunay

#endif // DELAUNAY__EDGE__INCLUDED
