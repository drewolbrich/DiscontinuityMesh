// Copyright 2008 Drew Olbrich

#include "Face.h"

#include <cstdlib>
#include <cassert>

#include "Mesh.h"

namespace delaunay {

Face::Face()
    : mAdjacentVertexCount(0),
      mAdjacentEdgeCount(0)
{
}

Face::~Face()
{
#ifdef DEBUG
    mAdjacentVertexCount = 0;
    mAdjacentEdgeCount = 0;
#endif
}

FacePtr
Face::clone(MeshPtr meshPtr) const
{
    FacePtr facePtr = meshPtr->createFace();

    return facePtr;
}

void
Face::addAdjacentVertex(VertexPtr vertexPtr)
{
    assert(mAdjacentVertexCount >= 0);
    assert(mAdjacentVertexCount < MAX_ADJACENT_VERTICES);
    mAdjacentVertexPtr[mAdjacentVertexCount] = vertexPtr;
    ++mAdjacentVertexCount;
}

void
Face::removeAdjacentVertex(VertexPtr vertexPtr)
{
    assert((mAdjacentVertexCount > 0 && mAdjacentVertexPtr[0] == vertexPtr)
        || (mAdjacentVertexCount > 1 && mAdjacentVertexPtr[1] == vertexPtr)
        || (mAdjacentVertexCount > 2 && mAdjacentVertexPtr[2] == vertexPtr));
    for (int index = 0; index < mAdjacentVertexCount; ++index) {
        if (mAdjacentVertexPtr[index] == vertexPtr) {
            for (; index + 1 < mAdjacentVertexCount; ++index) {
                mAdjacentVertexPtr[index] = mAdjacentVertexPtr[index + 1];
            }
            --mAdjacentVertexCount;
            return;
        }
    }

    // The vertex was not found.
    assert(0);
}

int
Face::adjacentVertexCount() const
{
    return mAdjacentVertexCount;
}

VertexPtr
Face::adjacentVertex(int index) const
{
    assert(index >= 0);
    assert(index < mAdjacentVertexCount);

    return mAdjacentVertexPtr[index];
}

void
Face::addAdjacentEdge(EdgePtr edgePtr)
{
    assert(mAdjacentEdgeCount >= 0);
    assert(mAdjacentEdgeCount < MAX_ADJACENT_EDGES);
    mAdjacentEdgePtr[mAdjacentEdgeCount] = edgePtr;
    ++mAdjacentEdgeCount;
}

void
Face::removeAdjacentEdge(EdgePtr edgePtr)
{
    assert((mAdjacentEdgeCount > 0 && mAdjacentEdgePtr[0] == edgePtr)
        || (mAdjacentEdgeCount > 1 && mAdjacentEdgePtr[1] == edgePtr)
        || (mAdjacentEdgeCount > 2 && mAdjacentEdgePtr[2] == edgePtr));
    for (int index = 0; index < mAdjacentEdgeCount; ++index) {
        if (mAdjacentEdgePtr[index] == edgePtr) {
            for (; index + 1 < mAdjacentEdgeCount; ++index) {
                mAdjacentEdgePtr[index] = mAdjacentEdgePtr[index + 1];
            }
            --mAdjacentEdgeCount;
            return;
        }
    }

    // The edge was not found.
    assert(0);
}

int
Face::adjacentEdgeCount() const
{
    return mAdjacentEdgeCount;
}

EdgePtr
Face::adjacentEdge(int index) const
{
    assert(index >= 0);
    assert(index < mAdjacentEdgeCount);

    return mAdjacentEdgePtr[index];
}

} // namespace delaunay
