// Copyright 2009 Retroactive Fiasco.

#include "RetriangulatorFace.h"

#include <cassert>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <limits>

#include <cgmath/Constants.h>
#include <cgmath/LineOperations.h>
#include <cgmath/Vector2d.h>
#include <cgmath/Vector3d.h>
#include <cgmath/Vector3fOperations.h>
#include <con/Streams.h>
#include <exact/GeometricPredicates.h>
#include <except/FailedOperationException.h>
#include <mesh/FaceOperations.h>
#include <mesh/EdgeOperations.h>
#include <svg/SvgWriter.h>

#include "Retriangulator.h"
#include "RetriangulatorEdge.h"
#include "EdgePoint.h"
#include "FaceSplitter.h"
#include "TriangleVector.h"

namespace meshretri {

struct Endpoint {
    size_t mFaceLineSegmentIndex;
    unsigned mEndpointIndex;
};

typedef std::vector<Endpoint> EndpointVector;

typedef std::map<EndpointIdentifier, EndpointVector> EndpointIdentifierToEndpointVectorMap;

RetriangulatorFace::RetriangulatorFace()
    : mFacePtr(), 
      mFaceLineSegmentVector(), 
      mAxis0(0), 
      mAxis1(1), 
      mAxis2(2), 
      mEpsilon(0.0),
      mOriginalVertexPtrVector()
{
}

RetriangulatorFace::~RetriangulatorFace()
{
}

void
RetriangulatorFace::setFacePtr(const mesh::FacePtr &facePtr)
{
    mFacePtr = facePtr;

    mOriginalVertexPtrVector.clear();
    for (mesh::AdjacentVertexIterator iterator = mFacePtr->adjacentVertexBegin();
         iterator != mFacePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        mOriginalVertexPtrVector.push_back(vertexPtr);
    }
}

const mesh::FacePtr &
RetriangulatorFace::facePtr() const
{
    return mFacePtr;
}

void
RetriangulatorFace::addFaceLineSegment(const FaceLineSegment &faceLineSegment)
{
    mFaceLineSegmentVector.push_back(faceLineSegment);
}

void
RetriangulatorFace::setAxis0(size_t axis0)
{
    assert(axis0 < 3);

    mAxis0 = axis0;
}

size_t
RetriangulatorFace::axis0() const
{
    return mAxis0;
}

void
RetriangulatorFace::setAxis1(size_t axis1)
{
    assert(axis1 < 3);

    mAxis1 = axis1;
}

size_t
RetriangulatorFace::axis1() const
{
    return mAxis1;
}

void
RetriangulatorFace::setAxis2(size_t axis2)
{
    assert(axis2 < 3);

    mAxis2 = axis2;
}

size_t
RetriangulatorFace::axis2() const
{
    return mAxis2;
}

void
RetriangulatorFace::setEpsilon(float epsilon)
{
    mEpsilon = epsilon;
}

float
RetriangulatorFace::epsilon() const
{
    return mEpsilon;
}

void
RetriangulatorFace::weldFaceLineSegmentEndpoints()
{
    EndpointIdentifierToEndpointVectorMap endpointIdentifierToEndpointVectorMap;

    // Create a map of unique EndpointIdentifiers to all FaceLineSegment endpoints
    // that share that EndpointIdentifier.
    for (size_t faceLineSegmentIndex = 0;
         faceLineSegmentIndex < mFaceLineSegmentVector.size();
         ++faceLineSegmentIndex) {
        FaceLineSegment &faceLineSegment = mFaceLineSegmentVector[faceLineSegmentIndex];

        for (unsigned endpointIndex = 0; endpointIndex < 2; ++endpointIndex) {
            Endpoint endpoint;
            endpoint.mFaceLineSegmentIndex = faceLineSegmentIndex;
            endpoint.mEndpointIndex = endpointIndex;
            endpointIdentifierToEndpointVectorMap[
                faceLineSegment.endpointIdentifier(endpointIndex)].push_back(endpoint);
        }
    }

    // Weld together all FaceLineSegment endpoints that share common EndpointIdentifiers.
    for (EndpointIdentifierToEndpointVectorMap::const_iterator iterator
             = endpointIdentifierToEndpointVectorMap.begin();
         iterator != endpointIdentifierToEndpointVectorMap.end(); ++iterator) {
        const EndpointVector &endpointVector = (*iterator).second;

        if (endpointVector.size() > 1) {
            cgmath::Vector3f position;
            for (EndpointVector::const_iterator iterator = endpointVector.begin();
                 iterator != endpointVector.end(); ++iterator) {
                const Endpoint &endpoint = *iterator;
                FaceLineSegment &faceLineSegment = mFaceLineSegmentVector[
                    endpoint.mFaceLineSegmentIndex];
                if (iterator == endpointVector.begin()) {
                    position = faceLineSegment.worldPosition(endpoint.mEndpointIndex);
                } else {
                    assert(position.equivalent(
                               faceLineSegment.worldPosition(endpoint.mEndpointIndex),
                               10.0*mEpsilon));
                    faceLineSegment.setWorldPosition(endpoint.mEndpointIndex, position);
                }
            }
        }
    }
}

void
RetriangulatorFace::createEdgePointsFromEndpointIdentifierEdgePtrs(
    Retriangulator *retriangulator)
{
    for (size_t faceLineSegmentIndex = 0;
         faceLineSegmentIndex < mFaceLineSegmentVector.size();
         ++faceLineSegmentIndex) {
        FaceLineSegment &faceLineSegment = mFaceLineSegmentVector[faceLineSegmentIndex];

        for (unsigned endpointIndex = 0; endpointIndex < 2; ++endpointIndex) {

            mesh::EdgePtr edgePtr;
            if (faceLineSegment.endpointIdentifier(endpointIndex).getEdgePtr(&edgePtr)) {

                // This test is somewhat of a hack. If the point doesn't
                // really seem to lie near the edge we think it's associated with,
                // we don't bother assigning it to that edge.
                // There are cases where points really are located extremely far away
                // from the edges associated with their EndpointIdentifiers.
                // We've never tracked down exactly why that is.

                assert(edgePtr->adjacentVertexCount() == 2);
                mesh::AdjacentVertexIterator iterator = edgePtr->adjacentVertexBegin();
                const cgmath::Vector2f edgePoint0((*iterator)->position(), mAxis0, mAxis1);
                ++iterator;
                const cgmath::Vector2f edgePoint1((*iterator)->position(), mAxis0, mAxis1);

                const cgmath::Vector2f lineSegmentPoint(
                    faceLineSegment.worldPosition(endpointIndex), mAxis0, mAxis1);

                bool pointIsNearEdge = cgmath::GetDistanceFromLineSegmentToPoint2f(
                    edgePoint0, edgePoint1, lineSegmentPoint) < 10.0*mEpsilon;

                if (pointIsNearEdge) {
                    retriangulator->addEdgePointToRetriangulatorEdge(edgePtr,
                        this, faceLineSegmentIndex, endpointIndex);
                    faceLineSegment.setEdgePtr(endpointIndex, edgePtr);
                }
            }
        }
    }
}

bool
RetriangulatorFace::createEdgePointsFromPerimeterIntersections(Retriangulator *retriangulator)
{
    bool createdEdgePoints = false;

    cgmath::Vector2f faceCenter = cgmath::Vector2f(
        mesh::GetFaceAverageVertexPosition(mFacePtr), mAxis0, mAxis1);

    // Loop over all the edges on the perimeter of the face.
    for (mesh::AdjacentEdgeIterator iterator = mFacePtr->adjacentEdgeBegin();
         iterator != mFacePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);

        // Loop over all the face's line segments
        // and test if any of them intersect the spans along the edge
        // defined by its EdgePoints.
        for (size_t faceLineSegmentIndex = 0;
             faceLineSegmentIndex < mFaceLineSegmentVector.size();
             ++faceLineSegmentIndex) {
            FaceLineSegment &faceLineSegment = mFaceLineSegmentVector[faceLineSegmentIndex];

            // If the line segment is already known to be colinear
            // with the original mesh edge, there's no need to test it
            // for intersection with the edge again.
            if (faceLineSegment.isColinearWithExistingMeshEdge()) {
                continue;
            }

            // Consider the two endpoints of the LineSegment.
            for (unsigned endpointIndex = 0; endpointIndex < 2; ++endpointIndex) {

                // If this endpoint has already been added to an edge in a previous step
                // (where the endpoint was known to exactly intersect the edge via its
                // EndpointIdentifier), don't try to add it to the edge again, or to
                // another edge.
                if (faceLineSegment.hasEdgePtr(endpointIndex)) {
                    continue;
                }

                // The current FaceLineSegment endpoint, in the 2D space of the face.
                cgmath::Vector2f endpoint(faceLineSegment.worldPosition(endpointIndex),
                    mAxis0, mAxis1);

                // Loop over all the spans of the edge and see if the
                // endpoint exactly intersects or extends beyond them.
                for (size_t spanIndex = 0; spanIndex < retriangulatorEdge->spanCount(); 
                     ++spanIndex) {

                    // Get the endpoints of the edge span, in the 2D space of the face.
                    cgmath::Vector2f spanP0;
                    cgmath::Vector2f spanP1;
                    retriangulatorEdge->getSpanEndpoints2d(spanIndex, &spanP0, &spanP1, 
                        mAxis0, mAxis1);

                    // Test if the FaceLineSegment endpoint is outside of, or exactly
                    // intersects, the edge perimeter. To do this, we test if a line
                    // segment from the center of the face to the endpoint exactly
                    // intersects any of the edge's spans.
                    if (exact::TestLineSegmentsIntersect2f(endpoint, faceCenter,
                            spanP0, spanP1)) {

                        // Add a new EdgePoint to the edge, based on this
                        // FaceLineSegment endpoint.
                        EdgePoint edgePoint;
                        edgePoint.setRetriangulatorFace(this);
                        edgePoint.setFaceLineSegmentIndex(faceLineSegmentIndex);
                        edgePoint.setEndpointIndex(endpointIndex);
                        retriangulatorEdge->addEdgePoint(edgePoint);

                        createdEdgePoints = true;
                        faceLineSegment.setEdgePtr(endpointIndex, edgePtr);

                        // Update the EdgePoint positions and resort them,
                        // to reflect the newly added EdgePoint.
                        // This is somewhat expensive and redundant, but we're assuming
                        // that we won't be adding large number of points to
                        // most edges, and that it'll be rare for a FaceLineSegment
                        // to be colinear with an edge.
                        retriangulatorEdge->updateEdgePoints();

                        // We've just sorted the EdgePoints along the edge,
                        // invaliding all the spans, so we must break
                        // out of the span loop.
                        break;
                    }
                }
            }
        }
    }

    return createdEdgePoints;
}

bool
RetriangulatorFace::correctSelfIntersectingFacePerimeterLineSegments(
    Retriangulator *retriangulator)
{
    // We're assuming that it'll rarely be the case that a face's
    // perimeter will self-intersect and need correction,
    // and that corrections will result in extremely small displacements
    // of line segment endpoints.

    size_t perimeterSpanCount = getPerimeterSpanCount(retriangulator);
    assert(perimeterSpanCount >= 3);

    if (perimeterSpanCount == 3) {
        return false;
    }

    bool foundIntersectingSpans = false;

    int loopCount = 0;

again:
    ++loopCount;

    if (loopCount > 100) {
        // If this happens, we're most likely stuck in an infinite loop
        // because of an unexpected geometric configuration.
        throw except::FailedOperationException(SOURCE_LINE) << "Exceeded loop limit "
            << "in face retriangulator while correcting self intersecting "
            << "face perimeter line segments.";
    }

    for (size_t spanIndex1 = 0; spanIndex1 + 1 < perimeterSpanCount; ++spanIndex1) {
        cgmath::Vector2f p0;
        cgmath::Vector2f p1;
        getPerimeterSpanEndpoints2d(retriangulator, spanIndex1, &p0, &p1);

        for (size_t spanIndex2 = spanIndex1 + 1; spanIndex2 < perimeterSpanCount; ++spanIndex2) {
            cgmath::Vector2f q0;
            cgmath::Vector2f q1;
            getPerimeterSpanEndpoints2d(retriangulator, spanIndex2, &q0, &q1);

            if (perimeterSpansIntersect(p0, p1, q0, q1)) {

                bool spansAreColinear = exact::TestLineSegmentsAreColinear2f(p0, p1, q0, q1);

                RetriangulatorEdge *retriangulatorEdge1 = getPerimeterSpanRetriangulatorEdge(
                    retriangulator, spanIndex1);
                RetriangulatorEdge *retriangulatorEdge2 = getPerimeterSpanRetriangulatorEdge(
                    retriangulator, spanIndex2);

                mesh::VertexPtr sharedVertexPtr = getVertexPtrSharedByTwoEdges(
                    retriangulatorEdge1->edgePtr(), retriangulatorEdge2->edgePtr());

                // If spans on two edges intersect and they have no EdgePoints,
                // this probably means that the face is degenerate and we're
                // not handling that case correctly.
                assert(retriangulatorEdge1->edgePointCount() > 0
                    || retriangulatorEdge2->edgePointCount() > 0);

                // The code below assumes that the intersecting spans
                // are on different mesh edges.
                if (retriangulatorEdge1 == retriangulatorEdge2) {

                    spanIndex1 = mapPerimeterSpanIndexToRetriangulatorEdgeSpanIndex(
                        retriangulator, spanIndex1);
                    spanIndex2 = mapPerimeterSpanIndexToRetriangulatorEdgeSpanIndex(
                        retriangulator, spanIndex2);
                    if (spanIndex2 < spanIndex1) {
                        std::swap(spanIndex1, spanIndex2);
                    }

                    size_t edgePointIndex1 = 0;
                    size_t edgePointIndex2 = 0;
                    edgePointIndex1 = spanIndex1;
                    edgePointIndex2 = spanIndex2 + 1;
                    cgmath::Vector3f newPosition;
                    if (edgePointIndex2 > retriangulatorEdge1->edgePointCount() - 1) {
                        edgePointIndex2 = retriangulatorEdge1->edgePointCount() - 1;
                        mesh::EdgePtr edgePtr = retriangulatorEdge1->edgePtr();
                        assert(edgePtr->adjacentVertexCount() == 2);
                        mesh::AdjacentVertexIterator iterator = edgePtr->adjacentVertexBegin();
                        ++iterator;
                        newPosition = (*iterator)->position();
                    } else {
                        newPosition = retriangulatorEdge1->edgePoint(edgePointIndex2).position();
                    }

                    assert(edgePointIndex1 < retriangulatorEdge1->edgePointCount());
                    assert(edgePointIndex2 < retriangulatorEdge1->edgePointCount());

                    // It's possible that the span of edge points we've decided
                    // to collapse is too aggressive, and this is a possible
                    // source of artifacts.

                    weldEdgePointRangeToPoint(retriangulatorEdge1, 
                        edgePointIndex1, edgePointIndex2,
                        newPosition);
                        
                    retriangulatorEdge1->updateEdgePoints();

                } else {

                    if (retriangulatorEdge1->edgePointCount() > 0) {
                        size_t vertexEndpointIndex1 
                            = retriangulatorEdge1->getEdgePointIndexNearestVertex(
                                sharedVertexPtr);
                        spanIndex1 = mapPerimeterSpanIndexToRetriangulatorEdgeSpanIndex(
                            retriangulator, spanIndex1);
                        size_t spanEdgepointIndex1;
                        if (!spansAreColinear) {
                            spanEdgepointIndex1 
                                = retriangulatorEdge1->getNearestEdgePointFromSpan(
                                spanIndex1, vertexEndpointIndex1);
                        } else {
                            spanEdgepointIndex1 
                                = retriangulatorEdge1->getFarthestEdgePointFromSpan(
                                spanIndex1, vertexEndpointIndex1);
                        }
                        weldEdgePointRangeToPoint(retriangulatorEdge1, vertexEndpointIndex1,
                            spanEdgepointIndex1, sharedVertexPtr->position());
                        retriangulatorEdge1->updateEdgePoints();
                    }

                    if (retriangulatorEdge2->edgePointCount() > 0) {
                        size_t vertexEndpointIndex2 
                            = retriangulatorEdge2->getEdgePointIndexNearestVertex(sharedVertexPtr);
                        spanIndex2 = mapPerimeterSpanIndexToRetriangulatorEdgeSpanIndex(
                            retriangulator, spanIndex2);
                        size_t spanEdgepointIndex2;
                        if (!spansAreColinear) {
                            spanEdgepointIndex2 
                                = retriangulatorEdge2->getNearestEdgePointFromSpan(
                                spanIndex2, vertexEndpointIndex2);
                        } else {
                            spanEdgepointIndex2 
                                = retriangulatorEdge2->getFarthestEdgePointFromSpan(
                                spanIndex2, vertexEndpointIndex2);
                        }
                        weldEdgePointRangeToPoint(retriangulatorEdge2, vertexEndpointIndex2,
                            spanEdgepointIndex2, sharedVertexPtr->position());
                        retriangulatorEdge2->updateEdgePoints();
                    }
                }

                foundIntersectingSpans = true;

                // Changing the perimeter means that existing spans are invalidated,
                // and we have to start over again.
                goto again;
            }
        }
    }

    return foundIntersectingSpans;
}

bool
RetriangulatorFace::correctSelfIntersectingFacePerimeterEdgePoints(Retriangulator *retriangulator)
{
    bool foundIntersectingEdgePoints = false;

again:
    // Loop over each pair of edges, without visiting the same pair of edges twice.
    assert(mFacePtr->adjacentEdgeCount() == 3);
    mesh::AdjacentEdgeIterator lastEdgeIterator = mFacePtr->adjacentEdgeBegin();
    ++lastEdgeIterator;
    ++lastEdgeIterator;
    for (mesh::AdjacentEdgeIterator iterator1 = mFacePtr->adjacentEdgeBegin();
         iterator1 != lastEdgeIterator; ++iterator1) {
        mesh::EdgePtr edgePtr1 = *iterator1;
        RetriangulatorEdge *retriangulatorEdge1
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr1);

        mesh::AdjacentEdgeIterator iterator2 = iterator1;
        ++iterator2;
        for (; iterator2 != mFacePtr->adjacentEdgeEnd(); ++iterator2) {
            mesh::EdgePtr edgePtr2 = *iterator2;
            RetriangulatorEdge *retriangulatorEdge2
                = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr2);

            // Compare all EdgePoints in one edge against those in the other.

            for (size_t edgePointIndex1 = 0; 
                 edgePointIndex1 < retriangulatorEdge1->edgePointCount(); 
                 ++edgePointIndex1) {
                for (size_t edgePointIndex2 = 0; 
                     edgePointIndex2 < retriangulatorEdge2->edgePointCount(); 
                     ++edgePointIndex2) {

                    const cgmath::Vector2f ep0(
                        retriangulatorEdge1->edgePoint(edgePointIndex1).position(), 
                        mAxis0, mAxis1);
                    const cgmath::Vector2f ep1(
                        retriangulatorEdge2->edgePoint(edgePointIndex2).position(), 
                        mAxis0, mAxis1);

                    // If coincident EdgePoints are found, and these EdgePoints are not
                    // coincident with the face vertices, collapse the EdgePoints to the
                    // position of the mesh vertex shared by the two edges.

                    if (ep0 != ep1) {
                        // The EdgePoints are not coincident.
                        continue;
                    }

                    mesh::VertexPtr sharedVertexPtr = getVertexPtrSharedByTwoEdges(
                        retriangulatorEdge1->edgePtr(), retriangulatorEdge2->edgePtr());

                    const cgmath::Vector2f sharedVertexPosition(
                        sharedVertexPtr->position(), mAxis0, mAxis1);
                    if (ep0 == sharedVertexPosition
                        || ep1 == sharedVertexPosition) {
                        // One of the EdgePoints is coincident with a mesh vertex,
                        // in which case the EdgePoint can not be collapsed.
                        // If we tried, we'd wind up stuck in an infinite loop.
                        continue;
                    }

                    // Correct the first edge.
                    size_t vertexEndpointIndex1 
                        = retriangulatorEdge1->getEdgePointIndexNearestVertex(sharedVertexPtr);
                    weldEdgePointRangeToPoint(retriangulatorEdge1, vertexEndpointIndex1,
                        edgePointIndex1, sharedVertexPtr->position());
                    retriangulatorEdge1->updateEdgePoints();

                    // Correct the second edge.
                    size_t vertexEndpointIndex2 
                        = retriangulatorEdge2->getEdgePointIndexNearestVertex(sharedVertexPtr);
                    weldEdgePointRangeToPoint(retriangulatorEdge2, vertexEndpointIndex2,
                        edgePointIndex2, sharedVertexPtr->position());
                    retriangulatorEdge2->updateEdgePoints();

                    foundIntersectingEdgePoints = true;

                    // Changing the perimeter means that existing spans are invalidated,
                    // and we have to start over again.
                    goto again;
                }
            }
        }
    }

    return foundIntersectingEdgePoints;
}

bool
RetriangulatorFace::correctEdgePointsFromLineSegmentIntersections(Retriangulator *retriangulator)
{
    size_t perimeterSpanCount = getPerimeterSpanCount(retriangulator);
    assert(perimeterSpanCount >= 3);

    if (perimeterSpanCount == 3) {
        return false;
    }

    bool foundIntersectingSpans = false;

again:
    // Loop over all the edges on the perimeter of the face.
    for (mesh::AdjacentEdgeIterator iterator = mFacePtr->adjacentEdgeBegin();
         iterator != mFacePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);

        mesh::VertexPtr ev0;
        mesh::VertexPtr ev1;
        mesh::GetEdgeAdjacentVertices(edgePtr, &ev0, &ev1);

        cgmath::Vector3f ep0w;
        cgmath::Vector3f ep1w;
        mesh::GetEdgeVertexPositions(edgePtr, &ep0w, &ep1w);

        const cgmath::Vector2f ep0(ep0w, mAxis0, mAxis1);
        const cgmath::Vector2f ep1(ep1w, mAxis0, mAxis1);

        // Loop over all EdgePoints on this edge.
        for (size_t edgePointIndex = 0; edgePointIndex < retriangulatorEdge->edgePointCount(); 
             ++edgePointIndex) {
            const EdgePoint &edgePoint = retriangulatorEdge->edgePoint(edgePointIndex);

            if (edgePoint.retriangulatorFace() != this) {
                // The EdgePoint's FaceLineSegment isn't on this face.
                continue;
            }

            // Find the line segment associated with this EdgePoint.
            const FaceLineSegment &faceLineSegment 
                = this->faceLineSegment(edgePoint.faceLineSegmentIndex());
            cgmath::Vector2f p0(faceLineSegment.worldPosition(0), mAxis0, mAxis1);
            cgmath::Vector2f p1(faceLineSegment.worldPosition(1), mAxis0, mAxis1);

            // Test for the crazy case when a line segment straddles two edges
            // and intersects a mesh vertex somewhere along its endpoint.
            if (p0 != ep0
                && p0 != ep1
                && p1 != ep0
                && p1 != ep1) {
                if (exact::TestLineSegmentIntersectsPoint2f(p0, p1, ep0)) {
                    size_t spanEdgepointIndex = retriangulatorEdge->getNearestEdgePointFromSpan(
                        0, edgePointIndex);
                    weldEdgePointRangeToPoint(retriangulatorEdge, edgePointIndex,
                        spanEdgepointIndex, ep0w);
                    retriangulatorEdge->updateEdgePoints();
                    foundIntersectingSpans = true;
                    goto again;
                }
                if (exact::TestLineSegmentIntersectsPoint2f(p0, p1, ep1)) {
                    size_t spanEdgepointIndex = retriangulatorEdge->getNearestEdgePointFromSpan(
                        retriangulatorEdge->spanCount() - 1, edgePointIndex);
                    weldEdgePointRangeToPoint(retriangulatorEdge, edgePointIndex,
                        spanEdgepointIndex, ep1w);
                    retriangulatorEdge->updateEdgePoints();
                    foundIntersectingSpans = true;
                    goto again;
                }
            }

            // Test if the line segment intersects any of the edge's spans.
            for (size_t spanIndex = 0; spanIndex < retriangulatorEdge->spanCount();
                 ++spanIndex) {

                cgmath::Vector2f q0;
                cgmath::Vector2f q1;
                retriangulatorEdge->getSpanEndpoints2d(spanIndex, &q0, &q1, mAxis0, mAxis1);

                if (q0 == p0
                    || q0 == p1
                    || q1 == p0
                    || q1 == p1) {
                    continue;
                }

                if (exact::TestLineSegmentsIntersect2f(q0, q1, p0, p1)) {
                    size_t spanEdgepointIndex = retriangulatorEdge->getNearestEdgePointFromSpan(
                        spanIndex, edgePointIndex);
                    weldEdgePointRangeToPoint(retriangulatorEdge, edgePointIndex,
                        spanEdgepointIndex, edgePoint.position());
                    retriangulatorEdge->updateEdgePoints();
                    foundIntersectingSpans = true;
                    goto again;
                }
            }
        }
    }

    return foundIntersectingSpans;
}

bool
RetriangulatorFace::correctRemainingLineSegmentPerimeterIntersections(
    Retriangulator *retriangulator)
{
    bool foundIntersectingLineSegments = false;

    size_t perimeterSpanCount = getPerimeterSpanCount(retriangulator);

    for (size_t faceLineSegmentIndex = 0;
         faceLineSegmentIndex < mFaceLineSegmentVector.size();
         ++faceLineSegmentIndex) {
        FaceLineSegment &faceLineSegment = mFaceLineSegmentVector[faceLineSegmentIndex];

        cgmath::Vector2f p0(faceLineSegment.worldPosition(0), mAxis0, mAxis1);
        cgmath::Vector2f p1(faceLineSegment.worldPosition(1), mAxis0, mAxis1);

        for (size_t spanIndex = 0; spanIndex < perimeterSpanCount; ++spanIndex) {

            cgmath::Vector2f q0;
            cgmath::Vector2f q1;
            getPerimeterSpanEndpoints2d(retriangulator, spanIndex, &q0, &q1);

            if (q0 == q1) {
                continue;
            }

            // It's not considered an error if the line segment
            // has an endpoint in common with the perimeter span.
            if (q0 == p0
                || q0 == p1
                || q1 == p0
                || q1 == p1) {
                continue;
            }

            if (!exact::TestLineSegmentsIntersect2f(p0, p1, q0, q1)) {
                continue;
            }

            // At this point, we know that the line segment intersects
            // a perimeter span.

            foundIntersectingLineSegments = true;

            // This is a big hack to fix the line segment. We're assuming
            // this happens very rarely, and only during the calculation
            // of backprojections during shading, and that the line
            // segment is nearly colinear with the perimeter.
            faceLineSegment.setWorldPosition(1, faceLineSegment.worldPosition(0));
        }
    }

    return foundIntersectingLineSegments;
}

void
RetriangulatorFace::collapseEdgeCoincidentLineSegments()
{
    for (size_t faceLineSegmentIndex = 0;
         faceLineSegmentIndex < mFaceLineSegmentVector.size();
         ++faceLineSegmentIndex) {
        FaceLineSegment &faceLineSegment = mFaceLineSegmentVector[faceLineSegmentIndex];

        unsigned coincidentEndpointCount = 0;
            
        for (unsigned endpointIndex = 0; endpointIndex < 2; ++endpointIndex) {

            cgmath::Vector2f endpointPosition(faceLineSegment.worldPosition(endpointIndex),
                mAxis0, mAxis1);

            for (mesh::AdjacentVertexIterator iterator = mFacePtr->adjacentVertexBegin();
                 iterator != mFacePtr->adjacentVertexEnd(); ++iterator) {
                mesh::VertexPtr vertexPtr = *iterator;

                cgmath::Vector2f vertexPosition(vertexPtr->position(), mAxis0, mAxis1);

                if (vertexPosition == endpointPosition) {
                    ++coincidentEndpointCount;
                }
            }
        }

        if (coincidentEndpointCount == 2) {
            faceLineSegment.setWorldPosition(1, faceLineSegment.worldPosition(0));
        }
    }
}

bool
RetriangulatorFace::splitAllMutuallyIntersectingFaceLineSegments()
{
    // This algorithm is O(n^2) and could be improved if it turns out
    // to be a bottleneck.

    bool faceLineSegmentsWereSplit = false;

    // First test for all colinear intersections.
    for (size_t index1 = 0; index1 + 1 < mFaceLineSegmentVector.size(); ++index1) {
        for (size_t index2 = index1 + 1; index2 < mFaceLineSegmentVector.size(); ++index2) {

            cgmath::Vector3f p10 = mFaceLineSegmentVector[index1].worldPosition(0);
            cgmath::Vector3f p11 = mFaceLineSegmentVector[index1].worldPosition(1);
            cgmath::Vector3f p20 = mFaceLineSegmentVector[index2].worldPosition(0);
            cgmath::Vector3f p21 = mFaceLineSegmentVector[index2].worldPosition(1);

            cgmath::Vector2f a0 = cgmath::Vector2f(p10, mAxis0, mAxis1);
            cgmath::Vector2f a1 = cgmath::Vector2f(p11, mAxis0, mAxis1);
            cgmath::Vector2f b0 = cgmath::Vector2f(p20, mAxis0, mAxis1);
            cgmath::Vector2f b1 = cgmath::Vector2f(p21, mAxis0, mAxis1);

            if (exact::TestLineSegmentsAreColinear2f(a0, a1, b0, b1)
                && exact::TestLineSegmentsIntersect2f(a0, a1, b0, b1)) {

                if (splitColinearFaceLineSegments(index1, index2)) {
                    faceLineSegmentsWereSplit = true;
                }
            }
        }
    }

    // Then test for all non-colinear intersections.
    for (size_t index1 = 0; index1 + 1 < mFaceLineSegmentVector.size(); ++index1) {
        for (size_t index2 = index1 + 1; index2 < mFaceLineSegmentVector.size(); ++index2) {

            cgmath::Vector3f p10 = mFaceLineSegmentVector[index1].worldPosition(0);
            cgmath::Vector3f p11 = mFaceLineSegmentVector[index1].worldPosition(1);
            cgmath::Vector3f p20 = mFaceLineSegmentVector[index2].worldPosition(0);
            cgmath::Vector3f p21 = mFaceLineSegmentVector[index2].worldPosition(1);

            cgmath::Vector2f a0 = cgmath::Vector2f(p10, mAxis0, mAxis1);
            cgmath::Vector2f a1 = cgmath::Vector2f(p11, mAxis0, mAxis1);
            cgmath::Vector2f b0 = cgmath::Vector2f(p20, mAxis0, mAxis1);
            cgmath::Vector2f b1 = cgmath::Vector2f(p21, mAxis0, mAxis1);

            cgmath::Vector2f intersectionPoint;

            if (a0 != b0
                && a0 != b1
                && a1 != b0
                && a1 != b1
                && exact::TestLineSegmentsIntersect2f(a0, a1, b0, b1)
                && cgmath::GetIntersectionOfTwoLines(a0, a1, b0, b1, &intersectionPoint)) {

                cgmath::Vector3f splitPosition;
                splitPosition[mAxis0] = intersectionPoint[0];
                splitPosition[mAxis1] = intersectionPoint[1];
                if ((a1 - a0).length() > (b1 - b0).length()) {
                    double t = (cgmath::Vector2d(intersectionPoint) 
                        - cgmath::Vector2d(a0)).length()
                        /(cgmath::Vector2d(a1) - cgmath::Vector2d(a0)).length();
                    splitPosition[mAxis2] = double(p10[mAxis2])*(1.0 - t) 
                        + double(p11[mAxis2])*t;
                } else {
                    double t = (cgmath::Vector2d(intersectionPoint) 
                        - cgmath::Vector2d(b0)).length()
                        /(cgmath::Vector2d(b1) - cgmath::Vector2d(b0)).length();
                    splitPosition[mAxis2] = double(p20[mAxis2])*(1.0 - t) 
                        + double(p21[mAxis2])*t;
                }

                bool splitPointIsDegreeZero
                    = (mFaceLineSegmentVector[index1].isDegreeZeroDiscontinuity(0)
                        && mFaceLineSegmentVector[index1].isDegreeZeroDiscontinuity(1))
                    || (mFaceLineSegmentVector[index2].isDegreeZeroDiscontinuity(0)
                        && mFaceLineSegmentVector[index2].isDegreeZeroDiscontinuity(1));

                if (intersectionPoint != a0
                    && intersectionPoint != a1) {
                    splitFaceLineSegment(index1, splitPosition, splitPointIsDegreeZero);
                    faceLineSegmentsWereSplit = true;
                }

                if (intersectionPoint != b0
                    && intersectionPoint != b1) {
                    splitFaceLineSegment(index2, splitPosition, splitPointIsDegreeZero);
                    faceLineSegmentsWereSplit = true;
                }
            }
        }
    }

    return faceLineSegmentsWereSplit;
}

void
RetriangulatorFace::splitMeshFace(mesh::Mesh *mesh, Retriangulator *retriangulator)
{
    FaceSplitter faceSplitter;
    faceSplitter.setMesh(mesh);
    faceSplitter.setRetriangulator(retriangulator);
    faceSplitter.setRetriangulatorFace(this);
    faceSplitter.splitFace();
}

void
RetriangulatorFace::splitBackprojectionFace(const RetriangulatorEdge *retriangulatorEdgeArray,
    TriangleVector *triangleVector, Retriangulator *retriangulator)
{
    FaceSplitter faceSplitter;
    faceSplitter.setRetriangulator(retriangulator);
    faceSplitter.setRetriangulatorFace(this);
    faceSplitter.setRetriangulatorEdgeArray(retriangulatorEdgeArray);
    faceSplitter.splitBackprojectionFace(triangleVector);
}

size_t
RetriangulatorFace::faceLineSegmentCount() const
{
    return mFaceLineSegmentVector.size();
}

FaceLineSegment &
RetriangulatorFace::faceLineSegment(size_t index)
{
    assert(index < mFaceLineSegmentVector.size());

    return mFaceLineSegmentVector[index];
}

const std::vector<mesh::ConstVertexPtr> &
RetriangulatorFace::originalVertexPtrVector() const
{
    return mOriginalVertexPtrVector;
}

void
RetriangulatorFace::writeToSvgFile(Retriangulator *retriangulator,
    const std::string &filename)
{
    con::debug << "(RetriangulatorFace::writeToSvgFile)" << std::endl;

    svg::SvgWriter svgWriter;

    mesh::FacePtr facePtr = this->facePtr();

    con::debug << std::setprecision(12);

    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        con::debug << "    vertex " << vertexPtr->position() << std::endl;
    }

    for (mesh::AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
         iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;

        assert(edgePtr->adjacentVertexCount() == 2);
        mesh::AdjacentVertexIterator vertexIterator = edgePtr->adjacentVertexBegin();
        cgmath::Vector2f p0((*vertexIterator)->position(), mAxis0, mAxis1);
        ++vertexIterator;
        cgmath::Vector2f p1((*vertexIterator)->position(), mAxis0, mAxis1);

        std::vector<cgmath::Vector2f> pointVector;
        pointVector.push_back(p0);

        con::debug << "    edge" << std::endl;

        con::debug << "        " << p0 << std::endl;

        if (retriangulator->meshEdgeHasRetriangulatorEdge(edgePtr)) {
            RetriangulatorEdge *retriangulatorEdge 
                = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);
            for (size_t index = 0; index < retriangulatorEdge->edgePointCount(); ++index) {
                const EdgePoint &edgePoint = retriangulatorEdge->edgePoint(index);
                pointVector.push_back(cgmath::Vector2f(edgePoint.position(), mAxis0, mAxis1));

                con::debug << "        " << pointVector.back() << std::endl;
            }
        }

        pointVector.push_back(p1);

        con::debug << "        " << p1 << std::endl;

        for (size_t index = 0; index + 1 < pointVector.size(); ++index) {
            svgWriter.addLineSegment(pointVector[index], pointVector[index + 1], 1.0);
        }
    }

    for (size_t index = 0; index < faceLineSegmentCount(); ++index) {
        const FaceLineSegment &faceLineSegment = this->faceLineSegment(index);
        cgmath::Vector2f p0(faceLineSegment.worldPosition(0), mAxis0, mAxis1);
        cgmath::Vector2f p1(faceLineSegment.worldPosition(1), mAxis0, mAxis1);
        svgWriter.addLineSegment(p0, p1, 0.05);

        con::debug << "    line segment" << std::endl;
        con::debug << "        " << p0 << std::endl;
        con::debug << "        " << p1 << std::endl;
        if (faceLineSegment.isColinearWithExistingMeshEdge()) {
            con::debug << "        (is colinear with existing mesh edge)" << std::endl;
        }
        if (p0 == p1) {
            con::debug << "        (is degenerate)" << std::endl;
        }
    }

    svgWriter.writeToSvgFile(filename);

    con::info << "Wrote face to \"" << filename << "\"." << std::endl;
}

void
RetriangulatorFace::splitFaceLineSegment(size_t oldFaceLineSegmentIndex, 
    const cgmath::Vector3f &splitPosition, bool splitPointIsDegreeZero)
{
    FaceLineSegment &oldFaceLineSegment = mFaceLineSegmentVector[oldFaceLineSegmentIndex];

    if (oldFaceLineSegment.isDegreeZeroDiscontinuity(0)
        && oldFaceLineSegment.isDegreeZeroDiscontinuity(1)) {
        splitPointIsDegreeZero = true;
    }

    FaceLineSegment newFaceLineSegment;

    newFaceLineSegment.setWorldPosition(0, splitPosition);
    newFaceLineSegment.setIsDegreeZeroDiscontinuity(0, splitPointIsDegreeZero);

    newFaceLineSegment.setWorldPosition(1,
        oldFaceLineSegment.worldPosition(1));
    newFaceLineSegment.setIsDegreeZeroDiscontinuity(1, 
        oldFaceLineSegment.isDegreeZeroDiscontinuity(1));

    // This must be set before newFaceLineSegment is appended to the vector,
    // at which point the oldFaceLineSegment reference may or may not be invalidated.
    oldFaceLineSegment.setWorldPosition(1, splitPosition);
    oldFaceLineSegment.setIsDegreeZeroDiscontinuity(1, splitPointIsDegreeZero);

    mFaceLineSegmentVector.push_back(newFaceLineSegment);
}

bool
RetriangulatorFace::splitColinearFaceLineSegments(size_t oldFaceLineSegmentIndex1,
    size_t oldFaceLineSegmentIndex2)
{
    FaceLineSegment &a = mFaceLineSegmentVector[oldFaceLineSegmentIndex1];
    FaceLineSegment &b = mFaceLineSegmentVector[oldFaceLineSegmentIndex2];

    // Note that one or both of the line segments may be degenerate.

    // Find the longest axis of the span of line segment endpoints along the line.
    float maxLength = 0.0;
    unsigned maxAxis = mAxis0;
    for (unsigned axisIndex = 0; axisIndex < 2; ++axisIndex) {
        int axis;
        if (axisIndex == 0) {
            axis = mAxis0;
        } else {
            axis = mAxis1;
        }
        float min = std::min(a.worldPosition(0)[axis], std::min(a.worldPosition(1)[axis],
                std::min(b.worldPosition(0)[axis], b.worldPosition(1)[axis])));
        float max = std::max(a.worldPosition(0)[axis], std::max(a.worldPosition(1)[axis],
                std::max(b.worldPosition(0)[axis], b.worldPosition(1)[axis])));
        float length = max - min;
        if (length > maxLength) {
            maxLength = length;
            maxAxis = axis;
        }
    }
    unsigned axis = maxAxis;

    if (a.worldPosition(0)[axis] > a.worldPosition(1)[axis]) {
        a.swapEndpoints();
    }
    if (b.worldPosition(0)[axis] > b.worldPosition(1)[axis]) {
        b.swapEndpoints();
    }

    float a0 = a.worldPosition(0)[axis];
    float a1 = a.worldPosition(1)[axis];
    float b0 = b.worldPosition(0)[axis];
    float b1 = b.worldPosition(1)[axis];

    // +---+
    // +===+
    // +===+
    if (a0 == b0 && a1 == b1) {
        if (a0 == a1) {
            return false;
        }
        a.setWorldPosition(1, a.worldPosition(0));
        b.setIsDegreeZeroDiscontinuity(1,
            b.isDegreeZeroDiscontinuity(1) || a.isDegreeZeroDiscontinuity(1));
        return true;
    }

    // +---+
    //     +===+
    // +---+===+
    if (a1 == b0) {
        return false;
    }

    //     +---+
    // +===+
    // +===+---+
    if (a0 == b1) {
        return false;
    }

    // +-------+
    // +===+
    // +===+---+
    if (a0 == b0 && b1 < a1) {
        a.setWorldPosition(0, b.worldPosition(1));
        b.setIsDegreeZeroDiscontinuity(0,
            b.isDegreeZeroDiscontinuity(0) || a.isDegreeZeroDiscontinuity(0));
        b.setIsDegreeZeroDiscontinuity(1,
            b.isDegreeZeroDiscontinuity(1) || a.isDegreeZeroDiscontinuity(0));
        return true;
    }

    // +-------+
    //     +===+
    // +---+===+
    if (a0 < b0 && a1 == b1) {
        a.setWorldPosition(1, b.worldPosition(0));
        b.setIsDegreeZeroDiscontinuity(0,
            b.isDegreeZeroDiscontinuity(0) || a.isDegreeZeroDiscontinuity(1));
        b.setIsDegreeZeroDiscontinuity(1,
            b.isDegreeZeroDiscontinuity(1) || a.isDegreeZeroDiscontinuity(1));
        return true;
    }

    // +---+
    // +=======+
    // +---+===+
    if (a0 == b0 && a1 < b1) {
        b.setWorldPosition(0, a.worldPosition(1));
        a.setIsDegreeZeroDiscontinuity(0,a.isDegreeZeroDiscontinuity(0)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        a.setIsDegreeZeroDiscontinuity(1, a.isDegreeZeroDiscontinuity(1)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        return true;
    }

    //     +---+
    // +=======+
    // +===+---+
    if (a0 > b0 && a1 == b1) {
        b.setWorldPosition(1, a.worldPosition(0));
        a.setIsDegreeZeroDiscontinuity(0, a.isDegreeZeroDiscontinuity(0)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        a.setIsDegreeZeroDiscontinuity(1, a.isDegreeZeroDiscontinuity(1)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        return true;
    }

    //     +-------+
    // +=======+
    // +===+***+---+
    if (a0 > b0 && a0 < b1 && a1 > b1) {
        FaceLineSegment c;
        c.setWorldPosition(0, a.worldPosition(0));
        c.setWorldPosition(1, b.worldPosition(1));
        c.setIsDegreeZeroDiscontinuity(0, a.isDegreeZeroDiscontinuity(0)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        c.setIsDegreeZeroDiscontinuity(1, b.isDegreeZeroDiscontinuity(1)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        if (a.hasEdgePtr(0)) {
            c.setEdgePtr(0, a.edgePtr(0));
        }
        if (b.hasEdgePtr(1)) {
            c.setEdgePtr(1, b.edgePtr(1));
        }
        addFaceLineSegment(c);
        // Adding face line segment 'c' may have invalidated references 'a' and 'b' if the
        // line segment vector has been resized, so we must get them again.
        FaceLineSegment &a = mFaceLineSegmentVector[oldFaceLineSegmentIndex1];
        FaceLineSegment &b = mFaceLineSegmentVector[oldFaceLineSegmentIndex2];
        cgmath::Vector3f tempX = a.worldPosition(0);
        bool tempD0 = a.isDegreeZeroDiscontinuity(0);
        a.setWorldPosition(0, b.worldPosition(1));
        a.setIsDegreeZeroDiscontinuity(0, b.isDegreeZeroDiscontinuity(1)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        b.setWorldPosition(1, tempX);
        b.setIsDegreeZeroDiscontinuity(1, tempD0
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        return true;
    }

    // +-------+
    //     +=======+
    // +---+***+===+
    if (a0 < b0 && a1 > b0 && a1 < b1) {
        FaceLineSegment c;
        c.setWorldPosition(0, b.worldPosition(0));
        c.setWorldPosition(1, a.worldPosition(1));
        c.setIsDegreeZeroDiscontinuity(0, b.isDegreeZeroDiscontinuity(0)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        c.setIsDegreeZeroDiscontinuity(1, a.isDegreeZeroDiscontinuity(1)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        if (b.hasEdgePtr(0)) {
            c.setEdgePtr(0, b.edgePtr(0));
        }
        if (a.hasEdgePtr(1)) {
            c.setEdgePtr(1, a.edgePtr(1));
        }
        addFaceLineSegment(c);
        // Adding face line segment 'c' may have invalidated references 'a' and 'b' if the
        // line segment vector has been resized, so we must get them again.
        FaceLineSegment &a = mFaceLineSegmentVector[oldFaceLineSegmentIndex1];
        FaceLineSegment &b = mFaceLineSegmentVector[oldFaceLineSegmentIndex2];
        cgmath::Vector3f tempX = a.worldPosition(1);
        bool tempD0 = a.isDegreeZeroDiscontinuity(1);
        a.setWorldPosition(1, b.worldPosition(0));
        a.setIsDegreeZeroDiscontinuity(1, b.isDegreeZeroDiscontinuity(0)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        b.setWorldPosition(0, tempX);
        b.setIsDegreeZeroDiscontinuity(0, tempD0
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        return true;
    }

    // +-----------+
    //     +===+
    // +---+===+***+
    if (a0 < b0 && b1 < a1) {
        FaceLineSegment c;
        c.setWorldPosition(0, b.worldPosition(1));
        c.setWorldPosition(1, a.worldPosition(1));
        c.setIsDegreeZeroDiscontinuity(0, b.isDegreeZeroDiscontinuity(1)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        c.setIsDegreeZeroDiscontinuity(1, a.isDegreeZeroDiscontinuity(1));
        if (b.hasEdgePtr(1)) {
            c.setEdgePtr(0, b.edgePtr(1));
        }
        if (a.hasEdgePtr(1)) {
            c.setEdgePtr(1, a.edgePtr(1));
        }
        addFaceLineSegment(c);
        // Adding face line segment 'c' may have invalidated references 'a' and 'b' if the
        // line segment vector has been resized, so we must get them again.
        FaceLineSegment &a = mFaceLineSegmentVector[oldFaceLineSegmentIndex1];
        FaceLineSegment &b = mFaceLineSegmentVector[oldFaceLineSegmentIndex2];
        a.setWorldPosition(1, b.worldPosition(0));
        b.setIsDegreeZeroDiscontinuity(0, b.isDegreeZeroDiscontinuity(0)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        b.setIsDegreeZeroDiscontinuity(1, b.isDegreeZeroDiscontinuity(1)
            || (a.isDegreeZeroDiscontinuity(0) && a.isDegreeZeroDiscontinuity(1)));
        return true;
    }

    //     +---+
    // +===========+
    // +===+---+***+
    if (a0 > b0 && a1 < b1) {
        FaceLineSegment c;
        c.setWorldPosition(0, a.worldPosition(1));
        c.setWorldPosition(1, b.worldPosition(1));
        c.setIsDegreeZeroDiscontinuity(0, a.isDegreeZeroDiscontinuity(1)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        c.setIsDegreeZeroDiscontinuity(1, b.isDegreeZeroDiscontinuity(1));
        addFaceLineSegment(c);
        // Adding face line segment 'c' may have invalidated references 'a' and 'b' if the
        // line segment vector has been resized, so we must get them again.
        FaceLineSegment &a = mFaceLineSegmentVector[oldFaceLineSegmentIndex1];
        FaceLineSegment &b = mFaceLineSegmentVector[oldFaceLineSegmentIndex2];
        b.setWorldPosition(1, a.worldPosition(0));
        b.setIsDegreeZeroDiscontinuity(1, b.isDegreeZeroDiscontinuity(1)
            || a.isDegreeZeroDiscontinuity(0));
        a.setIsDegreeZeroDiscontinuity(0, a.isDegreeZeroDiscontinuity(0) 
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        a.setIsDegreeZeroDiscontinuity(1, a.isDegreeZeroDiscontinuity(1)
            || (b.isDegreeZeroDiscontinuity(0) && b.isDegreeZeroDiscontinuity(1)));
        return true;
    }

    // We should never reach this point.
    assert(0);

    return false;
}

size_t
RetriangulatorFace::getPerimeterSpanCount(Retriangulator *retriangulator) const
{
    size_t spanCount = 0;

    // Loop over all the edges on the perimeter of the face.
    for (mesh::AdjacentEdgeIterator iterator = mFacePtr->adjacentEdgeBegin();
         iterator != mFacePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);

        spanCount += retriangulatorEdge->spanCount();
    }

    return spanCount;
}

void
RetriangulatorFace::getPerimeterSpanEndpoints2d(Retriangulator *retriangulator, 
    size_t spanIndex, cgmath::Vector2f *p0, cgmath::Vector2f *p1) const
{
    // Loop over all the edges on the perimeter of the face.
    for (mesh::AdjacentEdgeIterator iterator = mFacePtr->adjacentEdgeBegin();
         iterator != mFacePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);

        size_t spanCountOnEdge = retriangulatorEdge->spanCount();

        if (spanIndex < spanCountOnEdge) {
            retriangulatorEdge->getSpanEndpoints2d(spanIndex, p0, p1, mAxis0, mAxis1);
            return;
        }

        assert(spanIndex >= spanIndex);
        spanIndex -= spanCountOnEdge;
    }

    // We should never reach this point.
    assert(0);
}

RetriangulatorEdge *
RetriangulatorFace::getPerimeterSpanRetriangulatorEdge(Retriangulator *retriangulator,
    size_t spanIndex) const
{
    // Loop over all the edges on the perimeter of the face.
    for (mesh::AdjacentEdgeIterator iterator = mFacePtr->adjacentEdgeBegin();
         iterator != mFacePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);

        size_t spanCountOnEdge = retriangulatorEdge->spanCount();

        if (spanIndex < spanCountOnEdge) {
            return retriangulatorEdge;
        }

        assert(spanIndex >= spanIndex);
        spanIndex -= spanCountOnEdge;
    }

    // We should never reach this point.
    assert(0);

    return NULL;
}

size_t
RetriangulatorFace::mapPerimeterSpanIndexToRetriangulatorEdgeSpanIndex(
    Retriangulator *retriangulator, size_t perimeterSpanIndex) const
{
    size_t spanIndex = perimeterSpanIndex;

    // Loop over all the edges on the perimeter of the face.
    for (mesh::AdjacentEdgeIterator iterator = mFacePtr->adjacentEdgeBegin();
         iterator != mFacePtr->adjacentEdgeEnd(); ++iterator) {
        mesh::EdgePtr edgePtr = *iterator;
        RetriangulatorEdge *retriangulatorEdge 
            = retriangulator->getRetriangulatorEdgeFromMeshEdge(edgePtr);

        size_t spanCountOnEdge = retriangulatorEdge->spanCount();

        if (spanIndex < spanCountOnEdge) {
            return spanIndex;
        }

        assert(spanIndex >= spanIndex);
        spanIndex -= spanCountOnEdge;
    }

    // We should never reach this point.
    assert(0);

    return 0;
}

bool
RetriangulatorFace::perimeterSpansIntersect(const cgmath::Vector2f &p0, 
    const cgmath::Vector2f &p1, const cgmath::Vector2f &q0, const cgmath::Vector2f &q1)
{
    if (!exact::TestLineSegmentsIntersect2f(p0, p1, q0, q1)) {
        // The line segments do not intersect.
        return false;
    }

    if (p0 != q0
        && p0 != q1
        && p1 != q0
        && p1 != q1) {
        // The line segments have no endpoints in common,
        // so one line segment must intersect the other somewhere along its length.
        return true;
    }

    if (p0 == p1
        || q0 == q1) {
        // One of the line segments is degenerate.
        return false;
    }

    if (
        // +-----+
        // +==+
        (p0 == q0 && exact::TestLineSegmentIntersectsPoint2f(p0, p1, q1))
        || (p1 == q0 && exact::TestLineSegmentIntersectsPoint2f(p0, p1, q1))
        // +-----+
        //    +==+
        || (p0 == q1 && exact::TestLineSegmentIntersectsPoint2f(p0, p1, q0))
        || (p1 == q1 && exact::TestLineSegmentIntersectsPoint2f(p0, p1, q0))
        // +--+
        // +=====+
        || (q0 == p0 && exact::TestLineSegmentIntersectsPoint2f(q0, q1, p1))
        || (q1 == p0 && exact::TestLineSegmentIntersectsPoint2f(q0, q1, p1))
        //    +--+
        // +=====+
        || (q0 == p1 && exact::TestLineSegmentIntersectsPoint2f(q0, q1, p0))
        || (q1 == p1 && exact::TestLineSegmentIntersectsPoint2f(q0, q1, p0))
        ) {
        // The line segments have one endpoint in common,
        // And the other endpoint of one line segment intersects
        // the other line segment along its length.
        return true;
    }

    return false;
}

mesh::VertexPtr
RetriangulatorFace::getVertexPtrSharedByTwoEdges(mesh::EdgePtr edgePtr1,
    mesh::EdgePtr edgePtr2) const
{
    assert(edgePtr1->adjacentVertexCount() == 2);
    mesh::AdjacentVertexIterator iterator = edgePtr1->adjacentVertexBegin();
    mesh::VertexPtr vertexPtr1a = *iterator;
    ++iterator;
    mesh::VertexPtr vertexPtr1b = *iterator;

    assert(edgePtr2->adjacentVertexCount() == 2);
    iterator = edgePtr2->adjacentVertexBegin();
    if (*iterator == vertexPtr1a
        || *iterator == vertexPtr1b) {
        return *iterator;
    }
    ++iterator;
    assert(*iterator == vertexPtr1a
        || *iterator == vertexPtr1b);
    return *iterator;
}

void
RetriangulatorFace::weldEdgePointRangeToPoint(RetriangulatorEdge *retriangulatorEdge,
    size_t edgePointIndex1, size_t edgePointIndex2, const cgmath::Vector3f &position)
{
    if (edgePointIndex1 > edgePointIndex2) {
        std::swap(edgePointIndex1, edgePointIndex2);
    }

    for (size_t edgePointIndex = edgePointIndex1; edgePointIndex <= edgePointIndex2;
         ++edgePointIndex) {
        assert(edgePointIndex < retriangulatorEdge->edgePointCount());
        const EdgePoint &edgePoint = retriangulatorEdge->edgePoint(edgePointIndex);

        assert(edgePoint.faceLineSegmentIndex() < edgePoint.retriangulatorFace()
            ->faceLineSegmentCount());
        FaceLineSegment &faceLineSegment = edgePoint.retriangulatorFace()
            ->faceLineSegment(edgePoint.faceLineSegmentIndex());

        assert(edgePoint.endpointIndex() < 2);
        faceLineSegment.setWorldPosition(edgePoint.endpointIndex(), position);
    }
}

} // namespace meshretri
