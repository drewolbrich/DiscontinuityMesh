// Copyright 2008 Retroactive Fiasco.

#include "Vertex.h"

#include <cassert>
#include <algorithm>

#include "Edge.h"

using cgmath::Vector3f;

namespace mesh {

Vertex::Vertex()
    : AttributePossessor(),
      mPosition(Vector3f::ZERO),
      mEdgePtrVector(),
      mFacePtrVector()
{
}

Vertex::~Vertex()
{
}

void 
Vertex::setPosition(const Vector3f &position)
{
    mPosition = position;
}

const Vector3f &
Vertex::position() const
{
    return mPosition;
}

void
Vertex::addAdjacentEdge(EdgePtr edgePtr)
{
    // The vertex must not already have this edge.
    assert(!hasAdjacentEdge(edgePtr));

    // Add the edge to the vertex's edge vector.
    mEdgePtrVector.push_back(edgePtr);
}

void 
Vertex::removeAdjacentEdge(EdgePtr edgePtr)
{
    // The vertex must already have this edge.
    assert(hasAdjacentEdge(edgePtr));

    // Remove the edge from the vertex's edge vector.
    mEdgePtrVector.erase(std::find(mEdgePtrVector.begin(), mEdgePtrVector.end(), edgePtr));
}

bool 
Vertex::hasAdjacentEdge(ConstEdgePtr edgePtr) const
{
    // Return true if the edge is in the vertex's edge list.
    return std::find(mEdgePtrVector.begin(), mEdgePtrVector.end(), edgePtr) 
        != mEdgePtrVector.end();
}

unsigned int
Vertex::adjacentEdgeCount() const
{
    return mEdgePtrVector.size();
}

AdjacentEdgeIterator
Vertex::adjacentEdgeBegin()
{
    return mEdgePtrVector.begin();
}

AdjacentEdgeIterator
Vertex::adjacentEdgeEnd()
{
    return mEdgePtrVector.end();
}

AdjacentEdgeConstIterator
Vertex::adjacentEdgeBegin() const
{
    return mEdgePtrVector.begin();
}

AdjacentEdgeConstIterator
Vertex::adjacentEdgeEnd() const
{
    return mEdgePtrVector.end();
}

void
Vertex::addAdjacentFace(FacePtr face)
{
    // The vertex must not already have this face.
    assert(!hasAdjacentFace(face));

    // Add the face to the vertex's face list.
    mFacePtrVector.push_back(face);
}

void 
Vertex::removeAdjacentFace(FacePtr face)
{
    // The vertex must already have this face.
    assert(hasAdjacentFace(face));

    // Remove the face from the vertex's face list.
    mFacePtrVector.erase(find(mFacePtrVector.begin(), mFacePtrVector.end(), face));
}

bool 
Vertex::hasAdjacentFace(ConstFacePtr face) const
{
    // Return true if the face is in the vertex's face list.
    return find(mFacePtrVector.begin(), mFacePtrVector.end(), face) != mFacePtrVector.end();
}

unsigned int
Vertex::adjacentFaceCount() const
{
    return mFacePtrVector.size();
}

AdjacentFaceIterator
Vertex::adjacentFaceBegin()
{
    return mFacePtrVector.begin();
}

AdjacentFaceIterator
Vertex::adjacentFaceEnd()
{
    return mFacePtrVector.end();
}

AdjacentFaceConstIterator
Vertex::adjacentFaceBegin() const
{
    return mFacePtrVector.begin();
}

AdjacentFaceConstIterator
Vertex::adjacentFaceEnd() const
{
    return mFacePtrVector.end();
}

AdjacentEdgeIterator 
Vertex::findAdjacentEdgeByVertex(ConstVertexPtr vertexPtr)
{
    for (AdjacentEdgeIterator adjEdge = adjacentEdgeBegin();
         adjEdge != adjacentEdgeEnd(); ++adjEdge) {
        EdgePtr edgePtr(*adjEdge);

        if (edgePtr->hasAdjacentVertex(vertexPtr)) {
            return adjEdge;
        }
    }

    return adjacentEdgeEnd();
}

AdjacentEdgeConstIterator 
Vertex::findAdjacentEdgeByVertex(ConstVertexPtr vertexPtr) const
{
    // I'm hoping this implements the const version of this
    // function in terms of the non-const version. If the code
    // loops endlessly here, it's not working as I expected.
    return (const_cast<Vertex *>(this))->findAdjacentEdgeByVertex(vertexPtr);
}

} // namespace mesh
