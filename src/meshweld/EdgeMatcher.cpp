// Copyright 2009 Drew Olbrich

#include "EdgeMatcher.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>

#include <mesh/Edge.h>
#include <mesh/Vertex.h>
#include <mesh/EdgeOperations.h>

namespace meshweld {

EdgeMatcher::EdgeMatcher()
    : mAbsoluteTolerance(0.0),
      mRelativeTolerance(0.0),
      mDeletedElementTracker(NULL),
      mEdgePtr(),
      mEndpoint0(),
      mEndpoint1(),
      mMatchingEdgePtrVector()
{
}

EdgeMatcher::~EdgeMatcher()
{
}

void
EdgeMatcher::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

void
EdgeMatcher::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

void
EdgeMatcher::setDeletedElementTracker(mesh::DeletedElementTracker *deletedElementTracker)
{
    mDeletedElementTracker = deletedElementTracker;
}

void
EdgeMatcher::setEdgePtr(mesh::EdgePtr edgePtr)
{
    mEdgePtr = edgePtr;
}

void
EdgeMatcher::testAgainstEdgeAabbTree(const EdgeAabbTree &edgeAabbTree)
{
    cgmath::BoundingBox3f boundingBox = cgmath::GrowBoundingBox3fByAbsoluteAndRelativeTolerance(
        mesh::GetEdgeBoundingBox(mEdgePtr), mAbsoluteTolerance, mRelativeTolerance);

    mesh::GetEdgeVertexPositions(mEdgePtr, &mEndpoint0, &mEndpoint1);

    mMatchingEdgePtrVector.clear();
    mMatchingEdgePtrVector.reserve(4);

    // This calls EdgeMatcher::applyObjectToBoundingBox
    // for each edge whose bounding box intersects 'boundingBox'.
    edgeAabbTree.applyToBoundingBoxIntersection(boundingBox, this);
}

const EdgeMatcher::EdgePtrVector &
EdgeMatcher::matchingEdgePtrVector() const
{
    return mMatchingEdgePtrVector;
}

bool
EdgeMatcher::applyObjectToBoundingBox(Edge &edge,
    const cgmath::BoundingBox3f &)
{
    mesh::EdgePtr edgePtr = edge.edgePtr();

    if (edgePtr == mEdgePtr) {
        // Don't bother comparing the edge we're looking for against itself.
        return false;
    }

    assert(mDeletedElementTracker != NULL);
    if (mDeletedElementTracker->hasEdge(edgePtr)) {
        // Don't compare the edge against one that is already known
        // to have been deleted. Otherwise, we'd be accessing memory
        // that was already freed up.
        return false;
    }

    assert(edgePtr->adjacentVertexCount() == 2);
    mesh::AdjacentVertexConstIterator iterator = edgePtr->adjacentVertexBegin();
    const cgmath::Vector3f endpoint0 = (*iterator)->position();
    ++iterator;
    const cgmath::Vector3f endpoint1 = (*iterator)->position();

    if ((pointsAreEquivalent(endpoint0, mEndpoint0)
            && pointsAreEquivalent(endpoint1, mEndpoint1))
        || (pointsAreEquivalent(endpoint1, mEndpoint0)
            && pointsAreEquivalent(endpoint0, mEndpoint1))) {
        mMatchingEdgePtrVector.push_back(edgePtr);
    }

    // Continue testing against other edges. There may be multiple matches.
    return false;
}

bool
EdgeMatcher::pointsAreEquivalent(const cgmath::Vector3f &a, const cgmath::Vector3f &b) const
{
    for (unsigned axis = 0; axis < 3; ++axis) {
        float delta = fabsf(a[axis] - b[axis]);
        float maxAbs = fabsf(fabsf(a[axis]) - fabsf(b[axis]));
        if (delta > mAbsoluteTolerance
            && delta > maxAbs*mRelativeTolerance) {
            return false;
        }
    }

    return true;
}

} // namespace meshweld
