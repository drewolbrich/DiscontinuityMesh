// Copyright 2009 Retroactive Fiasco.

#ifndef MESHISECT__FACE_INTERSECTOR_AABB_TREE__INCLUDED
#define MESHISECT__FACE_INTERSECTOR_AABB_TREE__INCLUDED

#include <cgmath/AabbTree.h>

#include "FaceIntersectorAabbTreeNode.h"

namespace meshisect {

typedef cgmath::AabbTree<FaceIntersectorAabbTreeNode> FaceIntersectorAabbTree;

} // namespace meshisect

#endif // MESHISECT__FACE_INTERSECTOR_AABB_TREE__INCLUDED
