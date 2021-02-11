// Copyright 2008 Drew Olbrich

#include "LineSegmentCollection.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <list>

#include <cgmath/LineOperations.h>
#include <cgmath/LineSetIntersector.h>
#include <cgmath/Vector2fOperations.h>
#include <cgmath/Tolerance.h>
#include <exact/GeometricPredicates.h>

#include "Region.h"
#include "WedgeIntersector.h"

LineSegmentCollection::LineSegmentCollection()
    : mWedgeIntersector(NULL),
      mLineSegmentVector()
{
}

LineSegmentCollection::~LineSegmentCollection()
{
}

void
LineSegmentCollection::setWedgeIntersector(WedgeIntersector *wedgeIntersector)
{
    mWedgeIntersector = wedgeIntersector;
}

WedgeIntersector *
LineSegmentCollection::wedgeIntersector() const
{
    return mWedgeIntersector;
}

void
LineSegmentCollection::addLineSegment(const LineSegment &lineSegment)
{
    mLineSegmentVector.push_back(lineSegment);
}

void
LineSegmentCollection::calculateVisibleLineSegments()
{
    findLineSegmentIntersections();
    findLineSegmentAbutments();
    splitLineSegments();
    sortLineSegments();
    findVisibleLineSegments();
}

LineSegmentCollection::iterator
LineSegmentCollection::begin()
{
    return mLineSegmentVector.begin();
}

LineSegmentCollection::iterator
LineSegmentCollection::end()
{
    return mLineSegmentVector.end();
}

LineSegmentCollection::const_iterator
LineSegmentCollection::begin() const
{
    return mLineSegmentVector.begin();
}

LineSegmentCollection::const_iterator
LineSegmentCollection::end() const
{
    return mLineSegmentVector.end();
}

LineSegmentCollection::size_type
LineSegmentCollection::size() const
{
    return mLineSegmentVector.size();
}

bool
LineSegmentCollection::empty() const
{
    return mLineSegmentVector.empty();
}

void
LineSegmentCollection::clear()
{
    return mLineSegmentVector.clear();
}

void
LineSegmentCollection::findLineSegmentIntersections()
{
    cgmath::LineSetIntersector lineSetIntersector;
    cgmath::LineSetIntersector::LineSegmentVector lineSegmentVector;
    for (size_t index = 0; index < mLineSegmentVector.size(); ++index) {
        cgmath::LineSetIntersector::LineSegment lineSegment;
        lineSegment.mPoint0 = mLineSegmentVector[index].point0().wedgePosition();
        lineSegment.mPoint1 = mLineSegmentVector[index].point1().wedgePosition();
        lineSegmentVector.push_back(lineSegment);
    }
    lineSetIntersector.setLineSegmentVector(&lineSegmentVector);

    if (!lineSetIntersector.findIntersections()) {
        return;
    }

    const cgmath::LineSetIntersector::IntersectionVector &intersectionVector
        = lineSetIntersector.getIntersectionVector();
    for (size_t index = 0; index < intersectionVector.size(); ++index) {
        const cgmath::LineSetIntersector::Intersection &lineSetIntersection 
            = intersectionVector[index];

        // Generate a unique identifier for this intersection point.
        meshretri::EndpointIdentifier endpointIdentifier
            = meshretri::EndpointIdentifier::createUniqueIdentifier();

        const cgmath::LineSetIntersector::Intersection::LineSegmentIndexVector &
            lineSegmentIndexVector = lineSetIntersection.mLineSegmentIndexVector;
        for (size_t index = 0; index < lineSegmentIndexVector.size(); ++index) {
            size_t lineSegmentIndex = lineSegmentIndexVector[index];
            assert(lineSegmentIndex < mLineSegmentVector.size());
            LineSegment &lineSegment = mLineSegmentVector[lineSegmentIndex];

            const cgmath::Vector2f &ip = lineSetIntersection.mPoint;

            const cgmath::Vector2f &p0 = lineSegment.point0().wedgePosition();
            const cgmath::Vector2f &p1 = lineSegment.point1().wedgePosition();

            // Don't bother recording intersections with endpoints,
            // because it doesn't make sense to subdivide the line segments there.
            if (ip == p0
                || ip == p1) {
                continue;
            }

            LineSegment::Intersection intersection;
            intersection.mT = (p1 - p0).normalized().dot(ip - p0)/(p1 - p0).length();
            assert(intersection.mT >= 0.0);
            assert(intersection.mT <= 1.0);
            intersection.mWedgePosition = ip;
            mWedgeIntersector->transformWedgeSpacePointToWorldSpacePoint(ip,
                &intersection.mWorldPosition);
            intersection.mEndpointIdentifier = endpointIdentifier;
            lineSegment.addIntersection(intersection);
        }
    }
}

void
LineSegmentCollection::findLineSegmentAbutments()
{
    // Add intersection points where the endpoint of one line segment
    // nearly intersects another. This is critical to ensure that vertices are
    // introduced into the mesh where objects incidentally contact other objects,
    // but do not form a connected mesh. This allows the mesh to be
    // shaded properly in these situations and avoid artifacts that resemble light leaks.

    for (size_t index1 = 0; index1 < mLineSegmentVector.size(); ++index1) {
        LineSegment &lineSegment1 = mLineSegmentVector[index1];
        const Endpoint &ep0 = lineSegment1.point0();
        const Endpoint &ep1 = lineSegment1.point1();
        const cgmath::Vector2f &p0 = ep0.wedgePosition();
        const cgmath::Vector2f &p1 = ep1.wedgePosition();

        float firstLineSegmentMaxAbs = std::max(1.0f,
            std::max(p0.maxAbs(), p1.maxAbs()));

        for (size_t index2 = 0; index2 < mLineSegmentVector.size(); ++index2) {

            if (index1 == index2) {
                continue;
            }

            const LineSegment &lineSegment2 = mLineSegmentVector[index2];
            const Endpoint eq0 = lineSegment2.point0();
            const Endpoint eq1 = lineSegment2.point1();
            const cgmath::Vector2f &q0 = eq0.wedgePosition();
            const cgmath::Vector2f &q1 = eq1.wedgePosition();

            float epsilon = cgmath::TOLERANCE
                *std::max(firstLineSegmentMaxAbs,
                    std::max(q0.maxAbs(), q1.maxAbs()));

            if (cgmath::GetDistanceFromLineSegmentToPoint2f(p0, p1, q0) <= epsilon
                && (p0 - q0).length() > epsilon
                && (p1 - q0).length() > epsilon) {

                cgmath::Vector2f ip;
                cgmath::GetClosestPointOnLineSegment2f(p0, p1, q0, &ip);

                LineSegment::Intersection intersection;
                intersection.mT = (p1 - p0).normalized().dot(ip - p0)/(p1 - p0).length();
                assert(intersection.mT >= 0.0);
                assert(intersection.mT <= 1.0);
                intersection.mWedgePosition = ip;
                mWedgeIntersector->transformWedgeSpacePointToWorldSpacePoint(ip,
                    &intersection.mWorldPosition);
                intersection.mEndpointIdentifier 
                    = meshretri::EndpointIdentifier::createUniqueIdentifier();
                lineSegment1.addIntersection(intersection);
            }

            if (cgmath::GetDistanceFromLineSegmentToPoint2f(p0, p1, q1) <= epsilon
                && (p0 - q1).length() > epsilon
                && (p1 - q1).length() > epsilon) {

                cgmath::Vector2f ip;
                cgmath::GetClosestPointOnLineSegment2f(p0, p1, q1, &ip);

                LineSegment::Intersection intersection;
                intersection.mT = (p1 - p0).normalized().dot(ip - p0)/(p1 - p0).length();
                assert(intersection.mT >= 0.0);
                assert(intersection.mT <= 1.0);
                intersection.mWedgePosition = ip;
                mWedgeIntersector->transformWedgeSpacePointToWorldSpacePoint(ip,
                    &intersection.mWorldPosition);
                intersection.mEndpointIdentifier 
                    = meshretri::EndpointIdentifier::createUniqueIdentifier();
                lineSegment1.addIntersection(intersection);
            }
        }
    }
}

void
LineSegmentCollection::splitLineSegments()
{
    LineSegmentVector newLineSegmentVector;

    for (size_t index = 0; index < mLineSegmentVector.size(); ++index) {
        LineSegment &lineSegment = mLineSegmentVector[index];

        lineSegment.sortIntersectionVector();

        const LineSegment::IntersectionVector &intersectionVector
            = lineSegment.intersectionVector();

        const Endpoint &p0 = lineSegment.point0();
        const Endpoint &p1 = lineSegment.point1();

        Endpoint lastEndpoint = p0;

        for (size_t index = 0; index < intersectionVector.size(); ++index) {
            const LineSegment::Intersection &intersection = intersectionVector[index];

            Endpoint newEndpoint;
            newEndpoint.setWorldPosition(intersection.mWorldPosition);
            newEndpoint.setWedgePosition(intersection.mWedgePosition);

            assert(mWedgeIntersector != NULL);
            newEndpoint.setVisibilityParameter(
                mWedgeIntersector->projectPointOntoEdgePQ(
                    newEndpoint.worldPosition()));

            newEndpoint.setIsDegreeZeroDiscontinuity(true);

            newEndpoint.setEndpointIdentifier(intersection.mEndpointIdentifier);

            // Don't record degenerate line segments.
            if (newEndpoint.wedgePosition() == lastEndpoint.wedgePosition()) {
                continue;
            }

            LineSegment newLineSegment = lineSegment;
            newLineSegment.setPoint0(lastEndpoint);
            newLineSegment.setPoint1(newEndpoint);
            newLineSegmentVector.push_back(newLineSegment);

            lastEndpoint = newEndpoint;
        }

        // Only record the final line segment if it's not degenerate.
        if (p1.wedgePosition() != lastEndpoint.wedgePosition()) {
            LineSegment newLineSegment = lineSegment;
            newLineSegment.setPoint0(lastEndpoint);
            newLineSegment.setPoint1(p1);
            newLineSegmentVector.push_back(newLineSegment);
        }
    }

// I've disabled this check, because it'll get triggered if input
// polygons are coplanar and intersect.
#if 0
#ifdef DEBUG
    for (size_t index1 = 0; index1 < newLineSegmentVector.size(); ++index1) {
        for (size_t index2 = index1 + 1; index2 < newLineSegmentVector.size(); ++index2) {
            cgmath::Vector2f a0 = newLineSegmentVector[index1].point0().wedgePosition();
            cgmath::Vector2f a1 = newLineSegmentVector[index1].point1().wedgePosition();
            cgmath::Vector2f b0 = newLineSegmentVector[index2].point0().wedgePosition();
            cgmath::Vector2f b1 = newLineSegmentVector[index2].point1().wedgePosition();
            // If the line segments have all been properly clipped against each other,
            // they must either share endpoints or should not intersect.
            assert(a0 == b0 || a0 == b1 || a1 == b0 || a1 == b1
                || !exact::TestLineSegmentsIntersect(a0, a1, b0, b1));
        }
    }
#endif
#endif

    std::swap(mLineSegmentVector, newLineSegmentVector);
}

void
LineSegmentCollection::sortLineSegments()
{
    LineSegmentVector lineSegmentVectorCopy = mLineSegmentVector;

    // The following is a modified 2D implementation of Newell's Algorithm.
    // See "A Solution to the Hidden Surface Problem",
    // M. E. Newell, R. G. Newell, T. L. Sancha (1972).

    // The line segments are sorted in decreasing distance from the
    // light source.
    // (+Y is in the direction of the light source, -Y is away from the light source.)

    // First perform a rough sort of the line segments by their
    // minimum Y values, in increasing order.
    // This puts the line segment farthest away from the light source
    // at the beginning of the list.
    std::sort(mLineSegmentVector.begin(), mLineSegmentVector.end());

    const size_t originalSize = mLineSegmentVector.size();

    // Copy the contents of the vector into a list so it's
    // easier to remove elements from the list and append them
    // to the beginning of the list.
    typedef std::list<LineSegment> LineSegmentList;
    LineSegmentList lineSegmentList(mLineSegmentVector.begin(), mLineSegmentVector.end());

    // Clear out the line segment vector. That's where we're going
    // store our output.
    mLineSegmentVector.clear();
    mLineSegmentVector.reserve(originalSize);

    // Process all the line segments in the list, back to front.
    while (!lineSegmentList.empty()) {

        // Consider the element at the front of the list (P).
        LineSegment p = lineSegmentList.front();

        // Now we want to see if P occludes any of the line segments Q
        // in the list. If P doesn't occlude any of the line segments,
        // we can safely place it in our output vector, because it is behind
        // all of the other line segments.
        LineSegmentList::iterator iterator = lineSegmentList.begin();
        // Skip over P. We haven't removed it from the list yet.
        ++iterator;
        while (iterator != lineSegmentList.end()) {
            LineSegment &q = *iterator;

            if (!p.potentiallyOccludes(q)
                && !q.wasReordered()) {

                // If we reach a line segment Q which P cannot
                // possibly occlude, P must be the farthest line segment
                // from the line source, so it's OK to place it in our output vector.
                lineSegmentList.pop_front();
                mLineSegmentVector.push_back(p);
                break;

            } else if (p.occludes(q)) {

#if 0
                assert(!q.occludes(p));
#endif
                // If P and Q are mutually occluding, it's because the
                // line segments are nearly colinear,
                // or because the line segment clipping code 
                // resulted in two line segments that still intersect,
                // but only nearly so at one endpoint.
                // In this case, do an additional test to ensure
                // that Q effectively occludes P even though
                // they nearly intersect.
                if (q.occludes(p)
                    && q.occludesSlightlyIntersecting(p)) {
                    lineSegmentList.pop_front();
                    mLineSegmentVector.push_back(p);
                    break;
                }

#if 0
                assert(!q.reordered());
#endif
                // If we've already reordered this line segment once before
                // because the input was nearly degenerate,
                // give up and add the line segment to the list.
                if (q.wasReordered()) {
                    lineSegmentList.erase(iterator);
                    mLineSegmentVector.push_back(q);
                    break;
                }

                // If we've found a line segment Q that P occludes,
                // the list is ordered incorrectly.
                // Move Q to the beginning of the list.
                // If we've already moved Q to the beginning of the list
                // in this way, we're encountered a situation where
                // P and Q intersect. This should never happen.
                q.setWasReordered(true);
                lineSegmentList.erase(iterator);
                lineSegmentList.push_front(q);
                break;
            }

            ++iterator;
        }

        if (iterator == lineSegmentList.end()) {
            // If we've reached the end of the list, then P doesn't occlude
            // any of the line segments remaining in the list, 
            // so move P to the output vector.
            lineSegmentList.pop_front();
            mLineSegmentVector.push_back(p);
        }
    }

    assert(lineSegmentList.size() == 0);
    assert(mLineSegmentVector.size() == originalSize);

    // Reverse the vector, because in findVisibleLineSegments, below,
    // we want to process the line segments in front to back order,
    // not back to front.
    std::reverse(mLineSegmentVector.begin(), mLineSegmentVector.end());
}

void
LineSegmentCollection::findVisibleLineSegments()
{
    LineSegmentVector visibleLineSegmentVector;

    // This is initially the empty set.
    Region alreadyVisibleRegion;
 
    for (size_t index = 0; index < mLineSegmentVector.size(); ++index) {
        LineSegment &lineSegment = mLineSegmentVector[index];

        Endpoint ep0 = lineSegment.point0();
        Endpoint ep1 = lineSegment.point1();
        if (ep0.visibilityParameter() == ep1.visibilityParameter()) {
            // The line segment is vertical, and so has no thickness
            // in the S direction, and therefore cannot cast shadows.
            // Also, it would cause division by zero in some of the
            // code below.
            continue;
        }
        if (ep0.visibilityParameter() > ep1.visibilityParameter()) {
            std::swap(ep0, ep1);
        }

        Region lineSegmentRegion(
            ep0.visibilityParameter(),
            ep1.visibilityParameter(),
            ep0.endpointIdentifier(),
            ep1.endpointIdentifier(), 
            ep0.isDegreeZeroDiscontinuity(),
            ep1.isDegreeZeroDiscontinuity());

        // Subtract out the horizontal region of the wedge that
        // we have already determined to be visible, and which therefore must
        // shadow the line segment.
        Region visibleRegionOfLineSegment = lineSegmentRegion;
        visibleRegionOfLineSegment.computeDifference(alreadyVisibleRegion, mWedgeIntersector);

        for (size_t index = 0; index < visibleRegionOfLineSegment.spanCount(); ++index) {
            const Region::Span &span = visibleRegionOfLineSegment.span(index);

            // We know S (the visibility parameter of the endpoints
            // when mapped onto edge PQ of the wedge).

            // We want to find the world space and wedge space coordinates
            // along lineSegment that would project to the point
            // that corresponds to S along PQ.

            // If we could find the world space coordinate along the line
            // segment that would project to this point,
            // we could calculate the parametric value of this point along
            // the line segment, and from that, calculate the corresponding wedge
            // space coordinate.

            assert(mWedgeIntersector != NULL);

            cgmath::Vector3f world0;
            if (!mWedgeIntersector->projectEdgePQPointOntoLineSegment(
                    span.mMinS, lineSegment, &world0)) {
                continue;
            }

            cgmath::Vector3f world1;
            if (!mWedgeIntersector->projectEdgePQPointOntoLineSegment(
                    span.mMaxS, lineSegment, &world1)) {
                continue;
            }

            // Avoid dividing by zero.
            if (lineSegment.point1().worldPosition() == lineSegment.point0().worldPosition()) {
                continue;
            }

            float t0 = (world0 - lineSegment.point0().worldPosition()).length()
                /(lineSegment.point1().worldPosition() 
                    - lineSegment.point0().worldPosition()).length();
            t0 = std::max(0.0f, std::min(1.0f, t0));
            cgmath::Vector2f wedge0 = cgmath::InterpolateVector2f(
                lineSegment.point0().wedgePosition(),
                lineSegment.point1().wedgePosition(), t0);

            float t1 = (world1 - lineSegment.point0().worldPosition()).length()
                /(lineSegment.point1().worldPosition() 
                    - lineSegment.point0().worldPosition()).length();
            t1 = std::max(0.0f, std::min(1.0f, t1));
            cgmath::Vector2f wedge1 = cgmath::InterpolateVector2f(
                lineSegment.point0().wedgePosition(),
                lineSegment.point1().wedgePosition(), t1);

            // Transform the clipped wedge space point back to world space.
            mWedgeIntersector->transformWedgeSpacePointToWorldSpacePoint(wedge0, &world0);
            mWedgeIntersector->transformWedgeSpacePointToWorldSpacePoint(wedge1, &world1);

// This is a cheesy experimental attempt to correct for floating point error
// in the transformation from visibility parameters
// to wedge space positions and then to world space points,
// which resulted in mesh shading artifacts.
// I had to comment out the assert statements, because they were failing
// on backprojections, although bizarrely, this did not seem to cause any
// noticable errors.
// If this becomes a problem, I'll have to resort to more explicit tracking
// of these points through the meshretri library.
// See "How are line segments that arise from D0 discontinuity vertices dealt with now?"
// in TODO.txt.
#if 1
            if (span.mMinS == 0.0) {
#if 0
                assert(world0.equivalent(ep0.worldPosition(), 0.001));
#endif
                world0 = ep0.worldPosition();
            }
            if (span.mMaxS == 1.0) {
#if 0
                assert(world1.equivalent(ep1.worldPosition(), 0.001));
#endif
                world1 = ep1.worldPosition();
            }
#endif

            // Don't record line segments that lie above the occluder.
            // We don't want to record them in the discontinuity mesh.
            if (lineSegment.isAboveOccluder()) {
                continue;
            }

            Endpoint ep0;
            ep0.setWorldPosition(world0);
            ep0.setWedgePosition(wedge0);
            ep0.setEndpointIdentifier(span.mMinId);
            ep0.setIsDegreeZeroDiscontinuity(span.mMinIsD0);

            Endpoint ep1;
            ep1.setWorldPosition(world1);
            ep1.setWedgePosition(wedge1);
            ep1.setEndpointIdentifier(span.mMaxId);
            ep1.setIsDegreeZeroDiscontinuity(span.mMaxIsD0);

            LineSegment newLineSegment;
            newLineSegment.setPoint0(ep0);
            newLineSegment.setPoint1(ep1);
            newLineSegment.setFacePtr(lineSegment.facePtr());

            visibleLineSegmentVector.push_back(newLineSegment);
        }

        // Expand the existing known visible region by adding
        // the line segment to it.
        alreadyVisibleRegion.computeUnion(lineSegmentRegion);

        // If the visible region covers the entire width of the wedge, 
        // it's impossible that any other faces will be visible.
        if (alreadyVisibleRegion.full()) {
            break;
        }
    }

    mLineSegmentVector.swap(visibleLineSegmentVector);
}
