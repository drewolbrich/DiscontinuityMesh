// Copyright 2008 Retroactive Fiasco.

#include "Edge.h"

#include <cstdlib>
#include <cassert>
#include <limits>

#include "Mesh.h"

namespace delaunay {

static const size_t UNDEFINED_EDGE = std::numeric_limits<size_t>::max();

Edge::Edge()
    : mAdjacentVertexCount(0),
      mAdjacentFaceCount(0),
      mOutputEdgeIndex(UNDEFINED_EDGE),
      mInputEdgeIndex(UNDEFINED_EDGE)
{
}

Edge::~Edge()
{
#ifdef DEBUG
    mAdjacentVertexCount = 0;
    mAdjacentFaceCount = 0;
#endif
}

EdgePtr
Edge::clone(MeshPtr meshPtr) const
{
    EdgePtr edgePtr = meshPtr->createEdge();

    return edgePtr;
}

void
Edge::addAdjacentVertex(VertexPtr vertexPtr)
{
    assert(mAdjacentVertexCount >= 0);
    assert(mAdjacentVertexCount < MAX_ADJACENT_VERTICES);
    mAdjacentVertexPtr[mAdjacentVertexCount] = vertexPtr;
    ++mAdjacentVertexCount;
}

void
Edge::removeAdjacentVertex(VertexPtr vertexPtr)
{
    assert((mAdjacentVertexCount > 0 && mAdjacentVertexPtr[0] == vertexPtr)
        || (mAdjacentVertexCount > 1 && mAdjacentVertexPtr[1] == vertexPtr));
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
Edge::adjacentVertexCount() const
{
    return mAdjacentVertexCount;
}

VertexPtr
Edge::adjacentVertex(int index) const
{
    assert(index >= 0);
    assert(index < mAdjacentVertexCount);

    return mAdjacentVertexPtr[index];
}

void
Edge::addAdjacentFace(FacePtr facePtr)
{
    assert(mAdjacentFaceCount >= 0);
    assert(mAdjacentFaceCount < MAX_ADJACENT_FACES);
    mAdjacentFacePtr[mAdjacentFaceCount] = facePtr;
    ++mAdjacentFaceCount;
}

void
Edge::removeAdjacentFace(FacePtr facePtr)
{
    assert((mAdjacentFaceCount > 0 && mAdjacentFacePtr[0] == facePtr)
        || (mAdjacentFaceCount > 1 && mAdjacentFacePtr[1] == facePtr));
    for (int index = 0; index < mAdjacentFaceCount; ++index) {
        if (mAdjacentFacePtr[index] == facePtr) {
            for (; index + 1 < mAdjacentFaceCount; ++index) {
                mAdjacentFacePtr[index] = mAdjacentFacePtr[index + 1];
            }
            --mAdjacentFaceCount;
            return;
        }
    }

    // The face was not found.
    assert(0);
}

int
Edge::adjacentFaceCount() const
{
    return mAdjacentFaceCount;
}

FacePtr
Edge::adjacentFace(int index) const
{
    assert(index >= 0);
    assert(index < mAdjacentFaceCount);

    return mAdjacentFacePtr[index];
}

void
Edge::setOutputEdgeIndex(size_t outputEdgeIndex)
{
    mOutputEdgeIndex = outputEdgeIndex;
}

size_t
Edge::outputEdgeIndex() const
{
    assert(mOutputEdgeIndex != UNDEFINED_EDGE);

    return mOutputEdgeIndex;
}

bool
Edge::outputEdgeIndexIsDefined() const
{
    return mOutputEdgeIndex != UNDEFINED_EDGE;
}

void
Edge::setInputEdgeIndex(size_t inputEdgeIndex)
{
    mInputEdgeIndex = inputEdgeIndex;
}

size_t
Edge::inputEdgeIndex() const
{
    assert(mInputEdgeIndex != UNDEFINED_EDGE);

    return mInputEdgeIndex;
}

bool
Edge::inputEdgeIndexIsDefined() const
{
    return mInputEdgeIndex != UNDEFINED_EDGE;
}

} // namespace delaunay
