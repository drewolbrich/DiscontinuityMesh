// Copyright 2010 Drew Olbrich

#ifndef RFM_INDIRECT__SAMPLE_AABB_TREE__INCLUDED
#define RFM_INDIRECT__SAMPLE_AABB_TREE__INCLUDED

#include <cgmath/AabbTree.h>

#include "Sample.h"

// SampleAabbTree
//
// Axis aligned bounding box tree of Sample objects.

typedef cgmath::AabbTree<Sample> SampleAabbTree;

#endif // RFM_INDIRECT__SAMPLE_AABB_TREE__INCLUDED
