// Copyright 2010 Retroactive Fiasco.

#include "VertexAabbTreeNode.h"

#include <mesh/Vertex.h>

VertexAabbTreeNode::VertexAabbTreeNode()
    : mVertexPtr()
{
}

VertexAabbTreeNode::~VertexAabbTreeNode()
{
}

void
VertexAabbTreeNode::setVertexPtr(mesh::VertexPtr vertexPtr)
{
    mVertexPtr = vertexPtr;
}

mesh::VertexPtr
VertexAabbTreeNode::vertexPtr() const
{
    return mVertexPtr;
}

const cgmath::BoundingBox3f
VertexAabbTreeNode::boundingBox() const
{
    return cgmath::BoundingBox3f(mVertexPtr->position(), mVertexPtr->position());
}

