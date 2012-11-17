// Copyright 2009 Retroactive Fiasco.

#ifndef MESHISECT__EDGE_INTERSECTOR_AABB_TREE_NODE__INCLUDED
#define MESHISECT__EDGE_INTERSECTOR_AABB_TREE_NODE__INCLUDED

#include <cgmath/BoundingBox3f.h>
#include <mesh/Types.h>

namespace meshisect {

// EdgeIntersectorAabbTreeNode
//
// The AABB tree node type of the AABB tree maintained by EdgeIntersector.

class EdgeIntersectorAabbTreeNode
{
public:
    EdgeIntersectorAabbTreeNode();
    ~EdgeIntersectorAabbTreeNode();

    // The edge at the node in the AABB tree.
    void setEdgePtr(mesh::EdgePtr &edgePtr);
    mesh::EdgePtr edgePtr() const;

    // Required by the cgmath::AabbTree template.
    const cgmath::BoundingBox3f &boundingBox() const;

private:
    mesh::EdgePtr mEdgePtr;
    cgmath::BoundingBox3f mBoundingBox;
};

} // namespace meshisect

#endif // MESHISECT__EDGE_INTERSECTOR_AABB_TREE_NODE__INCLUDED
