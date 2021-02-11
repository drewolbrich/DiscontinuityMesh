// Copyright 2009 Drew Olbrich

#ifndef MESHISECT__FACE_INTERSECTOR_AABB_TREE_NODE__INCLUDED
#define MESHISECT__FACE_INTERSECTOR_AABB_TREE_NODE__INCLUDED

#include <cgmath/BoundingBox3f.h>
#include <mesh/Types.h>

namespace meshisect {

// FaceIntersectorAabbTreeNode
//
// The AABB tree node type of the AABB tree maintained by FaceIntersector.

class FaceIntersectorAabbTreeNode
{
public:
    FaceIntersectorAabbTreeNode();
    ~FaceIntersectorAabbTreeNode();

    // The face at the node in the AABB tree.
    void setFacePtr(const mesh::FacePtr &facePtr);
    const mesh::FacePtr &facePtr() const;

    // Required by the cgmath::AabbTree template.
    const cgmath::BoundingBox3f &boundingBox() const;

private:
    mesh::FacePtr mFacePtr;
    cgmath::BoundingBox3f mBoundingBox;
};

} // namespace meshisect

#endif // MESHISECT__FACE_INTERSECTOR_AABB_TREE_NODE__INCLUDED
