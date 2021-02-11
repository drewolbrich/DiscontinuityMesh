// Copyright 2009 Drew Olbrich

#include "RetriangulatorEdge.h"

#include <cassert>
#include <iostream>
#include <algorithm>

#include <con/Streams.h>
#include <mesh/Edge.h>
#include <mesh/EdgeOperations.h>
#include <cgmath/Vector3d.h>

#include "RetriangulatorFace.h"

namespace meshretri {

RetriangulatorEdge::RetriangulatorEdge()
    : mEdgePtr(), 
      mEdgePointVector(), 
      mEpsilon(0.0)
{
}

RetriangulatorEdge::~RetriangulatorEdge()
{
}

void
RetriangulatorEdge::setEdgePtr(const mesh::EdgePtr &edgePtr)
{
    mEdgePtr = edgePtr;
}

const mesh::EdgePtr &
RetriangulatorEdge::edgePtr() const
{
    return mEdgePtr;
}

void
RetriangulatorEdge::setEpsilon(float epsilon)
{
    mEpsilon = epsilon;
}

float
RetriangulatorEdge::epsilon() const
{
    return mEpsilon;
}

void
RetriangulatorEdge::addEdgePoint(const EdgePoint &edgePoint)
{
    mEdgePointVector.push_back(edgePoint);
}

void
RetriangulatorEdge::weldEdgePointsCreatedFromCommonWedge()
{
    // This is O(n^2), but we're assuming the number of EdgePoints
    // is never very large.

    for (size_t index = 0; index < mEdgePointVector.size(); ++index) {
        const EdgePoint &ep1 = mEdgePointVector[index];

        EndpointIdentifier id1 = ep1.retriangulatorFace()->faceLineSegment(
            ep1.faceLineSegmentIndex()).endpointIdentifier(ep1.endpointIndex());

        for (size_t secondaryIndex = index + 1; secondaryIndex < mEdgePointVector.size(); 
             ++secondaryIndex) {
            EdgePoint &ep2 = mEdgePointVector[secondaryIndex];

            if (id1 == ep2.retriangulatorFace()->faceLineSegment(
                    ep2.faceLineSegmentIndex()).endpointIdentifier(ep2.endpointIndex())
                // If both endpoints are from the same line segment,
                // don't weld the endpoints together.
                && !(ep1.retriangulatorFace() == ep2.retriangulatorFace()
                    && ep1.faceLineSegmentIndex() == ep2.faceLineSegmentIndex())) {

                // We're assuming the endpoints are close together.
                assert(ep1.retriangulatorFace()->faceLineSegment(
                           ep1.faceLineSegmentIndex()).worldPosition(ep1.endpointIndex())
                    .equivalent(
                        ep2.retriangulatorFace()->faceLineSegment(
                            ep2.faceLineSegmentIndex()).worldPosition(ep2.endpointIndex()),
                        10.0*mEpsilon));

                ep2.retriangulatorFace()->faceLineSegment(ep2.faceLineSegmentIndex())
                    .setWorldPosition(ep2.endpointIndex(), 
                        ep1.retriangulatorFace()->faceLineSegment(
                            ep1.faceLineSegmentIndex()).worldPosition(ep1.endpointIndex()));
            }
        }
    }    
}

void
RetriangulatorEdge::weldParametricallyEqualEdgePoints()
{
    for (size_t index = 0; index + 1 < mEdgePointVector.size(); ++index) {
        EdgePoint &ep1 = mEdgePointVector[index];
        EdgePoint &ep2 = mEdgePointVector[index + 1];

        if (ep1.t() == ep2.t()) {
            // We're assuming the endpoints are close together.
            assert(ep1.retriangulatorFace()->faceLineSegment(
                       ep1.faceLineSegmentIndex()).worldPosition(ep1.endpointIndex())
                .equivalent(
                    ep2.retriangulatorFace()->faceLineSegment(
                        ep2.faceLineSegmentIndex()).worldPosition(ep2.endpointIndex()),
                    10.0*mEpsilon));

            ep2.retriangulatorFace()->faceLineSegment(ep2.faceLineSegmentIndex())
                .setWorldPosition(ep2.endpointIndex(), 
                    ep1.retriangulatorFace()->faceLineSegment(
                        ep1.faceLineSegmentIndex()).worldPosition(ep1.endpointIndex()));
        }
    }
}

void
RetriangulatorEdge::splitMeshEdge(mesh::Mesh *mesh,
    mesh::AttributeKey isDegreeZeroDiscontinuityAttributeKey)
{
    cgmath::Vector3f p0;
    cgmath::Vector3f p1;
    mesh::GetEdgeVertexPositions(mEdgePtr, &p0, &p1);

    // Split the edge.
    mesh::EdgePtr edgePtr = mEdgePtr;
    cgmath::Vector3f lastPoint(cgmath::Vector3f::ZERO);
    for (size_t index = 0; index < mEdgePointVector.size(); ++index) {
        EdgePoint &edgePoint = mEdgePointVector[index];
        if ((index > 0 
                && edgePoint.position() == lastPoint)
            || edgePoint.position() == p0
            || edgePoint.position() == p1) {
            // Don't split the edge redundantly when there are 
            // multiple points at the same location,
            // or when a point is coincident with one of the original
            // edge endpoints.
            continue;
        }
        lastPoint = edgePoint.position();

        mesh::VertexPtr newVertexPtr;
        mesh::EdgePtr newEdgePtr;
        mesh::SplitEdge(mesh, edgePtr, edgePoint.position(),
            &newVertexPtr, &newEdgePtr);

        if (edgePoint.retriangulatorFace()->faceLineSegment(
                edgePoint.faceLineSegmentIndex())
            .isDegreeZeroDiscontinuity(edgePoint.endpointIndex())) {
            newVertexPtr->setBool(isDegreeZeroDiscontinuityAttributeKey, true);
        }
    }
}

void
RetriangulatorEdge::updateEdgePoints()
{
    if (mEdgePointVector.empty()) {
        return;
    }

    cgmath::Vector3f p0;
    cgmath::Vector3f p1;
    mesh::GetEdgeVertexPositions(mEdgePtr, &p0, &p1);

    cgmath::Vector3d p0d(p0);
    cgmath::Vector3d p1d(p1);
    double edgeLength = (p1d - p0d).length();

    cgmath::Vector3d dx(p1d - p0d);
    dx.normalize();

    // Compute the position and value of 't' for each edge point.
    for (size_t index = 0; index < mEdgePointVector.size(); ++index) {
        EdgePoint &edgePoint = mEdgePointVector[index];

        edgePoint.setPosition(
            edgePoint.retriangulatorFace()->faceLineSegment(edgePoint.faceLineSegmentIndex())
            .worldPosition(edgePoint.endpointIndex()));

        edgePoint.setT((cgmath::Vector3d(edgePoint.position()) - p0d).dot(dx)/edgeLength);
    }

    // Sort the edge points by 't'.
    std::sort(mEdgePointVector.begin(), mEdgePointVector.end());
}

size_t
RetriangulatorEdge::edgePointCount() const
{
    return mEdgePointVector.size();
}

const EdgePoint &
RetriangulatorEdge::edgePoint(size_t index) const
{
    assert(index < mEdgePointVector.size());

    return mEdgePointVector[index];
}

size_t
RetriangulatorEdge::spanCount() const
{
    return mEdgePointVector.size() + 1;
}

void
RetriangulatorEdge::getSpanEndpoints2d(size_t spanIndex, 
    cgmath::Vector2f *p0, cgmath::Vector2f *p1, int axis0, int axis1) const
{
    if (mEdgePointVector.size() == 0) {
        // The edge has no EdgePoints, so there is only one span.
        assert(spanIndex == 0);
        mesh::AdjacentVertexIterator iterator = mEdgePtr->adjacentVertexBegin();
        *p0 = cgmath::Vector2f((*iterator)->position(), axis0, axis1);
        ++iterator;
        *p1 = cgmath::Vector2f((*iterator)->position(), axis0, axis1);
        return;
    }

    if (spanIndex == 0) {
        assert(mEdgePtr->adjacentVertexCount() == 2);
        mesh::AdjacentVertexIterator iterator = mEdgePtr->adjacentVertexBegin();
        *p0 = cgmath::Vector2f((*iterator)->position(), axis0, axis1);
    } else {
        assert(spanIndex - 1 < mEdgePointVector.size());
        *p0 = cgmath::Vector2f(mEdgePointVector[spanIndex - 1].position(), axis0, axis1);
    }

    if (spanIndex == mEdgePointVector.size()) {
        assert(mEdgePtr->adjacentVertexCount() == 2);
        mesh::AdjacentVertexIterator iterator = mEdgePtr->adjacentVertexBegin();
        ++iterator;
        *p1 = cgmath::Vector2f((*iterator)->position(), axis0, axis1);
    } else {
        assert(spanIndex < mEdgePointVector.size());
        *p1 = cgmath::Vector2f(mEdgePointVector[spanIndex].position(), axis0, axis1);
    }
}

size_t
RetriangulatorEdge::getEdgePointIndexNearestVertex(mesh::VertexPtr vertexPtr) const
{
    assert(mEdgePtr->adjacentVertexCount() == 2);
    mesh::AdjacentVertexIterator iterator = mEdgePtr->adjacentVertexBegin();
    if (*iterator == vertexPtr) {
        return 0;
    }

    ++iterator;
    assert(*iterator == vertexPtr);
    assert(mEdgePointVector.size() != 0);
    return mEdgePointVector.size() - 1;
}

size_t
RetriangulatorEdge::getNearestEdgePointFromSpan(size_t spanIndex, size_t edgePointIndex) const
{
    assert(edgePointIndex < mEdgePointVector.size());

    // edgePointIndex:  v   0   1   v
    // spanIndex:         0   1   2

    int result;
    if (edgePointIndex < spanIndex) {
        result = int(spanIndex) - 1;
    } else {
        result = int(spanIndex);
    }

    if (result < 0) {
        result = 0;
    }

    if (result > int(mEdgePointVector.size()) - 1) {
        result = int(mEdgePointVector.size()) - 1;
    }

    return size_t(result);
}

size_t
RetriangulatorEdge::getFarthestEdgePointFromSpan(size_t spanIndex, size_t edgePointIndex) const
{
    assert(edgePointIndex < mEdgePointVector.size());

    // edgePointIndex:  v   0   1   v
    // spanIndex:         0   1   2

    int result;
    if (edgePointIndex < spanIndex) {
        result = int(spanIndex);
    } else {
        result = int(spanIndex) - 1;
    }

    if (result < 0) {
        result = 0;
    }

    if (result > int(mEdgePointVector.size()) - 1) {
        result = int(mEdgePointVector.size()) - 1;
    }

    return size_t(result);
}

} // namespace meshretri
