// Copyright 2008 Retroactive Fiasco.

#include "Vertex.h"

#include <cassert>
#include <algorithm>
#include <limits>

#include "Mesh.h"

namespace delaunay {

static const size_t UNDEFINED_VERTEX = std::numeric_limits<size_t>::max();

Vertex::Vertex()
    : mPosition(cgmath::Vector2f::ZERO),
      mAdjacentEdgePtrVector(),
      mAdjacentFacePtrVector(),
      mInputVertexIndex(UNDEFINED_VERTEX)
{
}

Vertex::~Vertex()
{
}

VertexPtr
Vertex::clone(MeshPtr meshPtr) const
{
    VertexPtr vertexPtr = meshPtr->createVertex();
    vertexPtr->setPosition(mPosition);

    return vertexPtr;
}

void
Vertex::setPosition(const cgmath::Vector2f &position)
{
    mPosition = position;
}

const cgmath::Vector2f &
Vertex::position() const
{
    return mPosition;
}

void
Vertex::addAdjacentEdge(EdgePtr edgePtr)
{
    // The edge must not already be adjacent to the vertex.
    assert(std::count(mAdjacentEdgePtrVector.begin(), 
            mAdjacentEdgePtrVector.end(), edgePtr) == 0);

    mAdjacentEdgePtrVector.push_back(edgePtr);

    assert(std::count(mAdjacentEdgePtrVector.begin(), 
            mAdjacentEdgePtrVector.end(), edgePtr) == 1);
}

void
Vertex::removeAdjacentEdge(EdgePtr edgePtr)
{
    // The edge must already be adjacent to the vertex.
    assert(std::count(mAdjacentEdgePtrVector.begin(), 
            mAdjacentEdgePtrVector.end(), edgePtr) == 1);

    // Remove all instances of edgePtr from mAdjacentEdgePtrVector.
    mAdjacentEdgePtrVector.erase(
        std::remove(mAdjacentEdgePtrVector.begin(),
            mAdjacentEdgePtrVector.end(), edgePtr),
        mAdjacentEdgePtrVector.end());

    assert(std::count(mAdjacentEdgePtrVector.begin(), 
            mAdjacentEdgePtrVector.end(), edgePtr) == 0);
}

int
Vertex::adjacentEdgeCount() const
{
    return int(mAdjacentEdgePtrVector.size());
}

EdgePtr
Vertex::adjacentEdge(int index) const
{
    assert(index >= 0);
    assert(index < int(mAdjacentEdgePtrVector.size()));

    return mAdjacentEdgePtrVector[index];
}

void
Vertex::addAdjacentFace(FacePtr facePtr)
{
    // This face must not already be adjacent to the vertex.
    assert(std::count(mAdjacentFacePtrVector.begin(), 
            mAdjacentFacePtrVector.end(), facePtr) == 0);

    mAdjacentFacePtrVector.push_back(facePtr);

    assert(std::count(mAdjacentFacePtrVector.begin(), 
            mAdjacentFacePtrVector.end(), facePtr) == 1);
}

void
Vertex::removeAdjacentFace(FacePtr facePtr)
{
    assert(std::count(mAdjacentFacePtrVector.begin(), 
            mAdjacentFacePtrVector.end(), facePtr) == 1);

    // Remove all instances of facePtr from mAdjacentFacePtrVector.
    mAdjacentFacePtrVector.erase(
        std::remove(mAdjacentFacePtrVector.begin(),
            mAdjacentFacePtrVector.end(), facePtr),
        mAdjacentFacePtrVector.end());

    assert(std::count(mAdjacentFacePtrVector.begin(), 
            mAdjacentFacePtrVector.end(), facePtr) == 0);
}

int
Vertex::adjacentFaceCount() const
{
    return int(mAdjacentFacePtrVector.size());
}

FacePtr
Vertex::adjacentFace(int index) const
{
    assert(index >= 0);
    assert(index < int(mAdjacentFacePtrVector.size()));

    return mAdjacentFacePtrVector[index];
}

void
Vertex::setInputVertexIndex(const size_t &inputVertexIndex)
{
    mInputVertexIndex = inputVertexIndex;
}

const size_t &
Vertex::inputVertexIndex() const
{
    assert(mInputVertexIndex != UNDEFINED_VERTEX);

    return mInputVertexIndex;
}

bool
Vertex::inputVertexIndexIsDefined() const
{
    return mInputVertexIndex != UNDEFINED_VERTEX;
}

} // namespace delaunay
