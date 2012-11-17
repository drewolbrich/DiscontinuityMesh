// Copyright 2009 Retroactive Fiasco.

#include "EdgeIntersectorAabbTreeNode.h"

#include <mesh/EdgeOperations.h>

namespace meshisect {

EdgeIntersectorAabbTreeNode::EdgeIntersectorAabbTreeNode()
    : mEdgePtr(),
      mBoundingBox()
{
}

EdgeIntersectorAabbTreeNode::~EdgeIntersectorAabbTreeNode()
{
}

void
EdgeIntersectorAabbTreeNode::setEdgePtr(mesh::EdgePtr &edgePtr)
{
    mEdgePtr = edgePtr;

    mBoundingBox = mesh::GetEdgeBoundingBox(edgePtr);
}

mesh::EdgePtr
EdgeIntersectorAabbTreeNode::edgePtr() const
{
    return mEdgePtr;
}

const cgmath::BoundingBox3f &
EdgeIntersectorAabbTreeNode::boundingBox() const
{
    return mBoundingBox;
}

} // namespace meshisect
