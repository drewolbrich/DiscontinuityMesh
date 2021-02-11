// Copyright 2009 Drew Olbrich

#include "FaceIntersectorAabbTreeNode.h"

#include <mesh/FaceOperations.h>

namespace meshisect {

FaceIntersectorAabbTreeNode::FaceIntersectorAabbTreeNode()
    : mFacePtr(),
      mBoundingBox()
{
}

FaceIntersectorAabbTreeNode::~FaceIntersectorAabbTreeNode()
{
}

void
FaceIntersectorAabbTreeNode::setFacePtr(const mesh::FacePtr &facePtr)
{
    mFacePtr = facePtr;

    mBoundingBox = mesh::GetFaceBoundingBox(facePtr);
}

const mesh::FacePtr &
FaceIntersectorAabbTreeNode::facePtr() const
{
    return mFacePtr;
}

const cgmath::BoundingBox3f &
FaceIntersectorAabbTreeNode::boundingBox() const
{
    return mBoundingBox;
}

} // namespace meshisect
