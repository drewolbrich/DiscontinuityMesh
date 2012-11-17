// Copyright 2009 Retroactive Fiasco.

#ifndef MESHISECT__FACE_INTERSECTOR__INCLUDED
#define MESHISECT__FACE_INTERSECTOR__INCLUDED

#include <cgmath/BoundingBox3f.h>
#include <mesh/Types.h>

#include "FaceIntersectorAabbTree.h"
#include "FaceIntersectorAabbTreeNode.h"
#include "FaceIntersectorListener.h"

namespace mesh {
class Mesh;
}

namespace meshisect {

// FaceIntersector
//
// Handles efficient AABB-based intersection of rays, triangles, and bounding boxes
// with meshisect faces.

class FaceIntersector 
    : public cgmath::AabbTree<FaceIntersectorAabbTreeNode>::RaySegmentOcclusionListener,
        public cgmath::AabbTree<FaceIntersectorAabbTreeNode>::RaySegmentIntersectionListener
{
public:
    FaceIntersector();
    ~FaceIntersector();

    // The mesh that will be tested for intersections.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Set an optional listener to ignore certain faces.
    void setIntersectorFaceListener(FaceIntersectorListener *faceIntersectorListener);

    // Creates the AABB hierachy used for the intersection test.
    void initialize();

    // Returns true if the specified ray intersects one or more of the mesh faces.
    bool occludesRaySegment(const cgmath::Vector3f &origin, 
        const cgmath::Vector3f &endpoint) const;

    // For cgmath:AabbTree::RaySegmentOcclusionListener:
    virtual bool objectOccludesRaySegment(
        const FaceIntersectorAabbTreeNode &faceIntersectorAabbTreeNode,
        const cgmath::Vector3f &origin, const cgmath::Vector3f &endpoint) const;

    // Returns true if the specified ray segment intersects one or more faces.
    // If true is returned, the point of intersection closest to the ray origin
    // is returned via intersectionPoint, and the intersected
    // face is also returned.
    bool intersectsRaySegment(const cgmath::Vector3f &origin, 
        const cgmath::Vector3f &endpoint, cgmath::Vector3f *intersectionPoint,
        mesh::FacePtr *facePtr);

    // For cgmath::AabbTree::RaySegmentIntersectionListener:
    virtual bool objectIntersectsRaySegment(
        const FaceIntersectorAabbTreeNode &faceIntersectorAabbTreeNode,
        const cgmath::Vector3f &origin, const cgmath::Vector3f &endpoint, float *t) const;

    typedef cgmath::AabbTree<FaceIntersectorAabbTreeNode>::Triangle Triangle;
    typedef cgmath::AabbTree<FaceIntersectorAabbTreeNode>::TriangleVector TriangleVector;
    typedef cgmath::AabbTree<FaceIntersectorAabbTreeNode>::TriangleListener TriangleListener;

    // Apply the TriangleListener to all faces that intersect an array of triangles.
    void applyToTriangleVectorIntersection(const TriangleVector &triangleVector,
        TriangleListener *triangleListener) const;

    typedef cgmath::AabbTree<
        FaceIntersectorAabbTreeNode>::BoundingBoxListener BoundingBoxListener;

    // Apply the BoundingBoxListener to all faces that intersect a bounding box.
    // Returns true if any listener function call returns true.
    bool applyToBoundingBoxIntersection(const cgmath::BoundingBox3f &boundingBox,
        BoundingBoxListener *boundingBoxListener) const;

    // Returns statistics about the AABB tree.
    std::string aabbSizeStatistics() const;

    // Returns statistics about queries of the AABB tree.
    std::string aabbQueryStatistics() const;

    // Returns number of AABB tree queries.
    unsigned queries() const;

private:
    mesh::Mesh *mMesh;

    FaceIntersectorAabbTree mFaceIntersectorAabbTree;

    FaceIntersectorAabbTree::ObjectVector mFaceIntersectorAabbTreeNodeVector;

    FaceIntersectorListener *mFaceIntersectorListener;
};

} // namespace meshisect

#endif // MESHISECT__FACE_INTERSECTOR__INCLUDED
