// Copyright 2008 Drew Olbrich

#include "MeshConnected.h"

#include <cassert>
#include <iostream>
#include <set>

#include "Vertex.h"
#include "Edge.h"

namespace delaunay {

struct CompareVertexPtr {
    bool operator()(VertexPtr v1, VertexPtr v2) {
        return &*v1 < &*v2;
    }
};

typedef std::set<VertexPtr, CompareVertexPtr> VertexPtrSet;

static void FloodFillVertices(VertexPtr vertexPtr, VertexPtrSet &vertexPtrSet);

bool
MeshIsSingleConnectedRegion(MeshPtr meshPtr)
{
    if (meshPtr->vertexCount() < 1) {
        return true;
    }

    VertexPtr vertexPtr = meshPtr->vertexBegin();
    VertexPtrSet vertexPtrSet;
    FloodFillVertices(vertexPtr, vertexPtrSet);

    return vertexPtrSet.size() == meshPtr->vertexCount();
}

static void 
FloodFillVertices(VertexPtr vertexPtr, VertexPtrSet &vertexPtrSet)
{
    if (vertexPtrSet.find(vertexPtr) != vertexPtrSet.end()) {
        // We've already visited this vertex.
        return;
    }

    vertexPtrSet.insert(vertexPtr);

    for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = vertexPtr->adjacentEdge(index);
        assert(edgePtr->adjacentVertexCount() == 2);
        if (edgePtr->adjacentVertex(0) == vertexPtr) {
            FloodFillVertices(edgePtr->adjacentVertex(1), vertexPtrSet);
        } else {
            FloodFillVertices(edgePtr->adjacentVertex(0), vertexPtrSet);
        }
    }
}

} // namespace delaunay
