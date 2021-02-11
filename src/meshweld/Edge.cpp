// Copyright 2009 Drew Olbrich

#include "Edge.h"

#include <mesh/Edge.h>
#include <mesh/Vertex.h>
#include <mesh/EdgeOperations.h>

namespace meshweld {

Edge::Edge()
    : mEdgePtr()
{
}

Edge::~Edge()
{
}

void
Edge::setEdgePtr(mesh::EdgePtr edgePtr)
{
    mEdgePtr = edgePtr;

    mBoundingBox = mesh::GetEdgeBoundingBox(edgePtr);
}

mesh::EdgePtr
Edge::edgePtr() const
{
    return mEdgePtr;
}

const cgmath::BoundingBox3f &
Edge::boundingBox() const
{
    return mBoundingBox;
}

} // namespace meshweld
