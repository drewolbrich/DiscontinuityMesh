// Copyright 2008 Retroactive Fiasco.

#include "Edge.h"

#include <cassert>
#include <algorithm>

namespace mesh {

Edge::Edge()
    : AttributePossessor(),
      mVertexPtrVector(),
      mFacePtrVector()
{
}

Edge::~Edge()
{
}

void
Edge::addAdjacentVertex(VertexPtr vertexPtr)
{
    // The edge must not already have this vertex.
    assert(!hasAdjacentVertex(vertexPtr));

    // Add the vertex to the edge's vertex vector.
    mVertexPtrVector.push_back(vertexPtr);
}

void 
Edge::removeAdjacentVertex(VertexPtr vertexPtr)
{
    // The edge must already have this vertex.
    assert(hasAdjacentVertex(vertexPtr));

    // Remove the vertex from the edge's vertex vector.
    mVertexPtrVector.erase(std::find(mVertexPtrVector.begin(), 
            mVertexPtrVector.end(), vertexPtr));
}

bool 
Edge::hasAdjacentVertex(ConstVertexPtr vertexPtr) const
{
    // Return true if the vertex is in the edge's vertex list.
    return std::find(mVertexPtrVector.begin(), mVertexPtrVector.end(), vertexPtr) 
        != mVertexPtrVector.end();
}

unsigned int
Edge::adjacentVertexCount() const
{
    return mVertexPtrVector.size();
}

AdjacentVertexIterator
Edge::adjacentVertexBegin()
{
    return mVertexPtrVector.begin();
}

AdjacentVertexIterator
Edge::adjacentVertexEnd()
{
    return mVertexPtrVector.end();
}

AdjacentVertexConstIterator
Edge::adjacentVertexBegin() const
{
    return mVertexPtrVector.begin();
}

AdjacentVertexConstIterator
Edge::adjacentVertexEnd() const
{
    return mVertexPtrVector.end();
}

void
Edge::addAdjacentFace(FacePtr face)
{
    // The edge must not already have this face.
    assert(!hasAdjacentFace(face));

    // Add the face to the edge's face list.
    mFacePtrVector.push_back(face);
}

void 
Edge::removeAdjacentFace(FacePtr face)
{
    // The edge must already have this face.
    assert(hasAdjacentFace(face));

    // Remove the face from the edge's face list.
    mFacePtrVector.erase(find(mFacePtrVector.begin(), mFacePtrVector.end(), face));
}

bool 
Edge::hasAdjacentFace(ConstFacePtr face) const
{
    // Return true if the face is in the edge's face list.
    return find(mFacePtrVector.begin(), mFacePtrVector.end(), face) != mFacePtrVector.end();
}

unsigned int
Edge::adjacentFaceCount() const
{
    return mFacePtrVector.size();
}

AdjacentFaceIterator
Edge::adjacentFaceBegin()
{
    return mFacePtrVector.begin();
}

AdjacentFaceIterator
Edge::adjacentFaceEnd()
{
    return mFacePtrVector.end();
}

AdjacentFaceConstIterator
Edge::adjacentFaceBegin() const
{
    return mFacePtrVector.begin();
}

AdjacentFaceConstIterator
Edge::adjacentFaceEnd() const
{
    return mFacePtrVector.end();
}

} // namespace mesh
