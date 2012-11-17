// Copyright 2009 Retroactive Fiasco.

#ifndef MESHISECT__EDGE_INTERSECTOR__INCLUDED
#define MESHISECT__EDGE_INTERSECTOR__INCLUDED

#include <cgmath/BoundingBox3f.h>
#include <mesh/Types.h>

#include "EdgeIntersectorAabbTree.h"
#include "EdgeIntersectorAabbTreeNode.h"

namespace mesh {
class Mesh;
}

namespace meshisect {

// EdgeIntersector
//
// Class that uses an axis aligned bounding box hierarchy to accelerate 
// intersection tests of edges and tetrahedrons.

class EdgeIntersector
{
public:
    EdgeIntersector();
    ~EdgeIntersector();

    // The meshisect that will be tested for intersections.
    void setMesh(mesh::Mesh *meshisect);
    mesh::Mesh *meshisect() const;

    // Creates the AABB hierachy used for the intersection test.
    void initialize();

    typedef cgmath::AabbTree<
        EdgeIntersectorAabbTreeNode>::TetrahedronListener TetrahedronListener;

    // Apply the TetrahedronListener to all edges that intersect a tetrahedron.
    // Returns true if any listener function call returns true.
    bool applyToTetrahedronIntersection(const cgmath::Vector3f &v0, 
        const cgmath::Vector3f &v1, const cgmath::Vector3f &v2, const cgmath::Vector3f &v3,
        TetrahedronListener *tetrahedronListener) const;

    typedef cgmath::AabbTree<
        EdgeIntersectorAabbTreeNode>::BoundingBoxListener BoundingBoxListener;

    // Apply the BoundingBoxListener to all edges that intersect a bounding box.
    // Returns true if any listener function call returns true.
    bool applyToBoundingBoxIntersection(const cgmath::BoundingBox3f &boundingBox,
        BoundingBoxListener *boundingBoxListener) const;

private:
    mesh::Mesh *mMesh;

    EdgeIntersectorAabbTree mEdgeIntersectorAabbTree;

    EdgeIntersectorAabbTree::ObjectVector mEdgeIntersectorAabbTreeNodeVector;
};

} // namespace meshisect

#endif // MESHISECT__EDGE_INTERSECTOR__INCLUDED
