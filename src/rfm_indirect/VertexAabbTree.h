// Copyright 2010 Drew Olbrich

#ifndef RFM_INDIRECT__VERTEX_AABB_TREE__INCLUDED
#define RFM_INDIRECT__VERTEX_AABB_TREE__INCLUDED

#include <cgmath/AabbTree.h>

#include "VertexAabbTreeNode.h"

// VertexAabbTree
//
// AABB tree of mesh vertices

typedef cgmath::AabbTree<VertexAabbTreeNode> VertexAabbTree;

#endif // RFM_INDIRECT__VERTEX_AABB_TREE__INCLUDED
