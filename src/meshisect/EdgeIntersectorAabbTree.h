// Copyright 2009 Retroactive Fiasco.

#ifndef MESHISECT__EDGE_INTERSECTOR_AABB_TREE__INCLUDED
#define MESHISECT__EDGE_INTERSECTOR_AABB_TREE__INCLUDED

#include <cgmath/AabbTree.h>

#include "EdgeIntersectorAabbTreeNode.h"

namespace meshisect {

typedef cgmath::AabbTree<EdgeIntersectorAabbTreeNode> EdgeIntersectorAabbTree;

} // namespace meshisect

#endif // MESHISECT__EDGE_INTERSECTOR_AABB_TREE__INCLUDED
