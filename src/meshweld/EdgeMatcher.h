// Copyright 2009 Drew Olbrich

#ifndef MESHWELD__EDGE_MATCHER__INCLUDED
#define MESHWELD__EDGE_MATCHER__INCLUDED

#include <cgmath/Vector3f.h>
#include <mesh/DeletedElementTracker.h>

#include "EdgeAabbTree.h"

namespace meshweld {

// EdgeMatcher
//
// This class works with the AABB tree to find edges
// that are nearly coincident with a reference edge.

class EdgeMatcher : public EdgeAabbTree::BoundingBoxListener
{
public:
    EdgeMatcher();
    ~EdgeMatcher();

    // Absolute tolerance.
    void setAbsoluteTolerance(float absoluteTolerance);

    // Relative tolerance.
    void setRelativeTolerance(float relativeTolerance);

    // DeletedElementTracker that keeps track of edges that have been deleted,
    // so we don't match against them.
    void setDeletedElementTracker(mesh::DeletedElementTracker *deletedElementTracker);

    // The edge being tested. We don't want to bother matching it with itself.
    void setEdgePtr(mesh::EdgePtr edgePtr);

    // Test the specified edge against a EdgeAabbTree.
    void testAgainstEdgeAabbTree(const EdgeAabbTree &edgeAabbTree);

    // The vector of edges that match the edge we're testing.
    typedef std::vector<mesh::EdgePtr> EdgePtrVector;
    const EdgePtrVector &matchingEdgePtrVector() const;

    // For EdgeAabbTree::BoundingBoxListener:
    virtual bool applyObjectToBoundingBox(Edge &edge,
        const cgmath::BoundingBox3f &boundingBox);

private:
    bool pointsAreEquivalent(const cgmath::Vector3f &a, const cgmath::Vector3f &b) const;

    float mAbsoluteTolerance;
    float mRelativeTolerance;

    mesh::DeletedElementTracker *mDeletedElementTracker;

    mesh::EdgePtr mEdgePtr;

    cgmath::Vector3f mEndpoint0;
    cgmath::Vector3f mEndpoint1;

    EdgePtrVector mMatchingEdgePtrVector;
};

} // namespace meshweld

#endif // MESHWELD__EDGE_MATCHER__INCLUDED
