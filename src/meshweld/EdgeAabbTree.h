// Copyright 2009 Retroactive Fiasco.

#ifndef MESHWELD__EDGE_AABB_TREE__INCLUDED
#define MESHWELD__EDGE_AABB_TREE__INCLUDED

#include <cgmath/AabbTree.h>

#include "Edge.h"

namespace meshweld {

// EdgeAabbTree
//
// Axis-aligned bounding box tree of edges.

typedef cgmath::AabbTree<Edge> EdgeAabbTree;

} // namespace meshweld

#endif // MESHWELD__EDGE_AABB_TREE__INCLUDED
