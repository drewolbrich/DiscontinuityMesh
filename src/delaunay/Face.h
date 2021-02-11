// Copyright 2008 Drew Olbrich

#ifndef DELAUNAY__FACE__INCLUDED
#define DELAUNAY__FACE__INCLUDED

#include "MeshTypes.h"
#include "Mesh.h"

namespace delaunay {

// Face
//
// A triangular face in a 2D mesh.
//
// The vertices and edges must be defined in counterclockwise order.
// The first and second vertices must be adjacent to the first edge.
// The second and third vertices must be adjacent to the second edge.
// The third and first vertices must be adjacent to the third edge.

class Face
{
public:
    Face();
    ~Face();

    // Return a copy of the edge as an element of the specified mesh.
    FacePtr clone(MeshPtr meshPtr) const;

    // The adjacent vertices.
    void addAdjacentVertex(VertexPtr vertexPtr);
    void removeAdjacentVertex(VertexPtr vertexPtr);
    int adjacentVertexCount() const;
    VertexPtr adjacentVertex(int index) const;

    // The adjacent edges.
    void addAdjacentEdge(EdgePtr edgePtr);
    void removeAdjacentEdge(EdgePtr edgePtr);
    int adjacentEdgeCount() const;
    EdgePtr adjacentEdge(int index) const;

private:
    short mAdjacentVertexCount;
    short mAdjacentEdgeCount;

    static const int MAX_ADJACENT_VERTICES = 3;
    static const int MAX_ADJACENT_EDGES = 3;

    VertexPtr mAdjacentVertexPtr[MAX_ADJACENT_VERTICES];
    EdgePtr mAdjacentEdgePtr[MAX_ADJACENT_EDGES];
};

} // namespace delaunay

#endif // DELAUNAY__FACE__INCLUDED
