// Copyright 2009 Retroactive Fiasco.

#include "FaceIntersector.h"

#include <cstdlib>

#include <mesh/Mesh.h>
#include <mesh/FaceOperations.h>

namespace meshisect {

FaceIntersector::FaceIntersector()
    : mMesh(NULL),
      mFaceIntersectorAabbTree(),
      mFaceIntersectorAabbTreeNodeVector(),
      mFaceIntersectorListener(NULL)
{
}

FaceIntersector::~FaceIntersector()
{
}

void
FaceIntersector::setMesh(mesh::Mesh *meshi)
{
    mMesh = meshi;
}

mesh::Mesh *
FaceIntersector::mesh() const
{
    return mMesh;
}

void
FaceIntersector::setIntersectorFaceListener(
    FaceIntersectorListener *faceIntersectorListener)
{
    mFaceIntersectorListener = faceIntersectorListener;
}

void
FaceIntersector::initialize()
{
    mFaceIntersectorAabbTree.clear();

    mFaceIntersectorAabbTreeNodeVector.clear();
    mFaceIntersectorAabbTreeNodeVector.reserve(mMesh->faceCount());

    for (mesh::FacePtr facePtr = mMesh->faceBegin(); facePtr != mMesh->faceEnd(); ++facePtr) {
        FaceIntersectorAabbTreeNode faceIntersectorAabbTreeNode;
        faceIntersectorAabbTreeNode.setFacePtr(facePtr);
        mFaceIntersectorAabbTreeNodeVector.push_back(faceIntersectorAabbTreeNode);
    }

    mFaceIntersectorAabbTree.initialize(mFaceIntersectorAabbTreeNodeVector);
}

bool
FaceIntersector::occludesRaySegment(const cgmath::Vector3f &origin, 
    const cgmath::Vector3f &endpoint) const
{
    return mFaceIntersectorAabbTree.occludesRaySegment(origin, endpoint, this);
}

bool
FaceIntersector::objectOccludesRaySegment(
    const FaceIntersectorAabbTreeNode &faceIntersectorAabbTreeNode,
    const cgmath::Vector3f &origin, const cgmath::Vector3f &endpoint) const
{
    float t = 0.0;
    if (!mesh::RaySegmentIntersectsFace(faceIntersectorAabbTreeNode.facePtr(),
            origin, endpoint, &t)) {
        return false;
    }

    if (mFaceIntersectorListener != NULL
        && !mFaceIntersectorListener->allowFaceIntersectionTest(
            faceIntersectorAabbTreeNode.facePtr(), t)) {
        return false;
    }

    return true;
}

bool
FaceIntersector::intersectsRaySegment(const cgmath::Vector3f &origin,
    const cgmath::Vector3f &endpoint, cgmath::Vector3f *intersectionPoint,
    mesh::FacePtr *facePtr)
{
    FaceIntersectorAabbTreeNode *faceIntersectorAabbTreeNode = NULL;
    if (mFaceIntersectorAabbTree.intersectsRaySegment(origin, endpoint, this,
            intersectionPoint, &faceIntersectorAabbTreeNode)) {
        *facePtr = faceIntersectorAabbTreeNode->facePtr();
        return true;
    }

    return false;
}

bool
FaceIntersector::objectIntersectsRaySegment(
    const FaceIntersectorAabbTreeNode &faceIntersectorAabbTreeNode,
    const cgmath::Vector3f &origin, const cgmath::Vector3f &endpoint, float *t) const
{
    mesh::FacePtr facePtr = faceIntersectorAabbTreeNode.facePtr();

    float intersectionT = 0.0;
    if (!mesh::RaySegmentIntersectsFace(facePtr, origin, endpoint, &intersectionT)) {
        // The ray segment does not intersect the face.
        return false;
    }

    if (intersectionT > *t) {
        // The point of intersection is farther away than the last
        // known point of intersection.
        return false;
    }

    if (mFaceIntersectorListener != NULL
        && !mFaceIntersectorListener->allowFaceIntersectionTest(
            faceIntersectorAabbTreeNode.facePtr(), intersectionT)) {
        return false;
    }

    *t = intersectionT;

    return true;
}

void
FaceIntersector::applyToTriangleVectorIntersection(const TriangleVector &triangleVector,
    TriangleListener *triangleListener) const
{
    mFaceIntersectorAabbTree.applyToTriangleVectorIntersection(triangleVector,
        triangleListener);
}

bool
FaceIntersector::applyToBoundingBoxIntersection(const cgmath::BoundingBox3f &boundingBox,
    BoundingBoxListener *boundingBoxListener) const
{
    return mFaceIntersectorAabbTree.applyToBoundingBoxIntersection(boundingBox,
        boundingBoxListener);
}

std::string
FaceIntersector::aabbSizeStatistics() const
{
    return mFaceIntersectorAabbTree.sizeStatistics();
}

std::string
FaceIntersector::aabbQueryStatistics() const
{
    return mFaceIntersectorAabbTree.queryStatistics();
}

unsigned
FaceIntersector::queries() const
{
    return mFaceIntersectorAabbTree.queries();
}

} // namespace meshisect
