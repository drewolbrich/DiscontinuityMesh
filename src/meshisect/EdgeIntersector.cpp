// Copyright 2009 Retroactive Fiasco.

#include "EdgeIntersector.h"

#include <mesh/Mesh.h>

namespace meshisect {

EdgeIntersector::EdgeIntersector()
    : mMesh(NULL),
      mEdgeIntersectorAabbTree(),
      mEdgeIntersectorAabbTreeNodeVector()
{
}

EdgeIntersector::~EdgeIntersector()
{
}

void
EdgeIntersector::setMesh(mesh::Mesh *meshisect)
{
    mMesh = meshisect;
}

mesh::Mesh *
EdgeIntersector::meshisect() const
{
    return mMesh;
}

void
EdgeIntersector::initialize()
{
    mEdgeIntersectorAabbTree.clear();

    mEdgeIntersectorAabbTreeNodeVector.clear();
    mEdgeIntersectorAabbTreeNodeVector.reserve(mMesh->edgeCount());

    for (mesh::EdgePtr edgePtr = mMesh->edgeBegin(); edgePtr != mMesh->edgeEnd(); ++edgePtr) {
        EdgeIntersectorAabbTreeNode edgeIntersectorAabbTreeNode;
        edgeIntersectorAabbTreeNode.setEdgePtr(edgePtr);
        mEdgeIntersectorAabbTreeNodeVector.push_back(edgeIntersectorAabbTreeNode);
    }

    mEdgeIntersectorAabbTree.initialize(mEdgeIntersectorAabbTreeNodeVector);
}

bool
EdgeIntersector::applyToTetrahedronIntersection(const cgmath::Vector3f &v0,
    const cgmath::Vector3f &v1, const cgmath::Vector3f &v2, const cgmath::Vector3f &v3,
    TetrahedronListener *tetrahedronListener) const
{
    return mEdgeIntersectorAabbTree.applyToTetrahedronIntersection(v0, v1, v2, v3,
        tetrahedronListener);
}

bool
EdgeIntersector::applyToBoundingBoxIntersection(const cgmath::BoundingBox3f &boundingBox,
    BoundingBoxListener *boundingBoxListener) const
{
    return mEdgeIntersectorAabbTree.applyToBoundingBoxIntersection(boundingBox, 
        boundingBoxListener);
}

} // namespace meshisect
