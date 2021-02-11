// Copyright 2010 Drew Olbrich

#ifndef RFM_INDIRECT__VERTEX_AABB_TREE_NODE__INCLUDED
#define RFM_INDIRECT__VERTEX_AABB_TREE_NODE__INCLUDED

#include <cgmath/BoundingBox3f.h>
#include <mesh/Types.h>

// VertexAabbTreeNode
//
// AABB tree node for VertexAabbTree.

class VertexAabbTreeNode
{
public:
    VertexAabbTreeNode();
    ~VertexAabbTreeNode();

    // The vertex at the node in the AABB tree.
    void setVertexPtr(mesh::VertexPtr vertexPtr);
    mesh::VertexPtr vertexPtr() const;

    // Required by the cgmath::AabbTree template.
    const cgmath::BoundingBox3f boundingBox() const;

private:
    mesh::VertexPtr mVertexPtr;
};

#endif // RFM_INDIRECT__VERTEX_AABB_TREE_NODE__INCLUDED
