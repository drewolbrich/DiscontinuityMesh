// Copyright 2008 Drew Olbrich

#include "Region.h"

#include <cassert>
#include <limits>
#include <algorithm>
#include <iostream>

#include <mesh/Edge.h>
#include <mesh/Vertex.h>

#include "WedgeIntersector.h"

Region::Region()
    : mSpanVector()
{
}

Region::Region(float minS, float maxS, const meshretri::EndpointIdentifier &minId, 
    const meshretri::EndpointIdentifier &maxId, bool minIsD0, bool maxIsD0)
    : mSpanVector()
{
    mSpanVector.push_back(Span(minS, maxS, minId, maxId, minIsD0, maxIsD0));
}

Region::~Region()
{
}

bool
Region::empty() const
{
    return mSpanVector.empty();
}

bool
Region::full() const
{
    return mSpanVector.size() == 1
        && mSpanVector[0].mMinS <= 0.0
        && mSpanVector[0].mMaxS >= 1.0;
}

bool
Region::isSingleSpan() const
{
    return mSpanVector.size() == 1;
}

float
Region::minS() const
{
    if (mSpanVector.empty()) {
        return std::numeric_limits<float>::max();
    }

    return mSpanVector[0].mMinS;
}

float
Region::maxS() const
{
    if (mSpanVector.empty()) {
        return std::numeric_limits<float>::min();
    }

    return mSpanVector[mSpanVector.size() - 1].mMaxS;
}

void
Region::computeUnion(const Region &rhs)
{
    // This is the only case that is handled for now.
    assert(rhs.isSingleSpan());

    SpanVector newSpanVector;

    // Add all spans to the left of the rhs span.
    size_t index = 0;
    while (index < mSpanVector.size()
        && mSpanVector[index].mMaxS < rhs.mSpanVector[0].mMinS) {
        newSpanVector.push_back(mSpanVector[index]);
        ++index;
    }

    if (index == mSpanVector.size()) {

        newSpanVector.push_back(rhs.mSpanVector[0]);

    } else {

        if (mSpanVector[index].mMinS > rhs.mSpanVector[0].mMaxS) {

            newSpanVector.push_back(rhs.mSpanVector[0]);

        } else {

            // Create a new single span that contains the union of the rhs span
            // with all the spans that intersect it.
            Span newSpan;
            newSpan.mMinS = std::min(mSpanVector[index].mMinS, rhs.mSpanVector[0].mMinS);
            if (newSpan.mMinS == mSpanVector[index].mMinS) {
                newSpan.mMinId = mSpanVector[index].mMinId;
                newSpan.mMinIsD0 = mSpanVector[index].mMinIsD0;
            } else {
                newSpan.mMinId = rhs.mSpanVector[0].mMinId;
                newSpan.mMinIsD0 = rhs.mSpanVector[0].mMinIsD0;
            }
            newSpan.mMaxS = rhs.mSpanVector[0].mMaxS;
            while (index < mSpanVector.size()
                && mSpanVector[index].mMinS <= rhs.mSpanVector[0].mMaxS) {
                newSpan.mMaxS = std::max(mSpanVector[index].mMaxS, rhs.mSpanVector[0].mMaxS);
                if (newSpan.mMaxS == mSpanVector[index].mMaxS) {
                    newSpan.mMaxId = mSpanVector[index].mMaxId;
                    newSpan.mMaxIsD0 = mSpanVector[index].mMaxIsD0;
                } else {
                    newSpan.mMaxId = rhs.mSpanVector[0].mMaxId;
                    newSpan.mMaxIsD0 = rhs.mSpanVector[0].mMaxIsD0;
                }
                ++index;
            }
            newSpanVector.push_back(newSpan);
        }

        // Add all the spans to the right of the rhs span.
        while (index < mSpanVector.size()) {
            newSpanVector.push_back(mSpanVector[index]);
            ++index;
        }
    }

    mSpanVector.swap(newSpanVector);
}

void
Region::computeDifference(const Region &rhs, WedgeIntersector *wedgeIntersector)
{
    // This is the only case that is handled for now.
    assert(isSingleSpan());

    SpanVector newSpanVector;

    // Skip over all rhs spans to the left of the lhs span, since 
    // subtracting them out of the lhs span would have no effect.
    size_t index = 0;
    while (index < rhs.mSpanVector.size()
        && rhs.mSpanVector[index].mMaxS <= mSpanVector[0].mMinS) {
        ++index;
    }

    if (index == rhs.mSpanVector.size()) {

        // If there are no more rhs spans remaining to subtract out, 
        // because they were all to the left of the lhs span,
        // the result is the lhs span.
        newSpanVector.push_back(mSpanVector[0]);

    } else {

        if (rhs.mSpanVector[index].mMinS >= mSpanVector[0].mMaxS) {

            // If the remaining rhs spans we're subtracting out are all
            // to the right of the lhs span, subtracting them would
            // have no effect, so the result is just the lhs span.
            newSpanVector.push_back(mSpanVector[0]);

        } else {

            // Handle the case where the leftmost rhs span is to the
            // right of the left side of the lhs span.
            // +-----------
            //     +===...
            if (rhs.mSpanVector[index].mMinS > mSpanVector[0].mMinS) {
                Span newSpan;
                newSpan.mMinS = mSpanVector[0].mMinS;
                newSpan.mMaxS = rhs.mSpanVector[index].mMinS;
                newSpan.mMinId = mSpanVector[0].mMinId;
                newSpan.mMinIsD0 = mSpanVector[0].mMinIsD0;
                // Because this vertex is being projected onto a different edge,
                // we need to come up with a new identifier for it,
                // or else the Retriangulator code will assume that
                // it's part of the original edge and screw up the mesh.
                newSpan.mMaxId = createProjectedDifferenceEndpointIdentifier(
                    rhs.mSpanVector[index].mMinId, wedgeIntersector);
                newSpan.mMaxIsD0 = false;
                newSpanVector.push_back(newSpan);
            }

            Span newSpan;
            newSpan.mMinS = rhs.mSpanVector[index].mMaxS;
            newSpan.mMinId = createProjectedDifferenceEndpointIdentifier(
                rhs.mSpanVector[index].mMaxId, wedgeIntersector);
            newSpan.mMinIsD0 = false;
            ++index;

            // Handle a series of rhs spans that completely overlap the lhs span.
            //   +-----+
            //  ...+ +====...
            while (index < rhs.mSpanVector.size()
                && rhs.mSpanVector[index].mMinS <= mSpanVector[0].mMaxS) {
                newSpan.mMaxS = rhs.mSpanVector[index].mMinS;
                newSpan.mMaxId = createProjectedDifferenceEndpointIdentifier(
                    rhs.mSpanVector[index].mMinId, wedgeIntersector);
                newSpan.mMaxIsD0 = false;
                newSpanVector.push_back(newSpan);
                newSpan.mMinS = rhs.mSpanVector[index].mMaxS;
                newSpan.mMinId = createProjectedDifferenceEndpointIdentifier(
                    rhs.mSpanVector[index].mMaxId, wedgeIntersector);
                newSpan.mMinIsD0 = false;
                ++index;
            }

            // Handle any remaining portion of the lhs span.
            if (newSpan.mMinS < mSpanVector[0].mMaxS) {
                newSpan.mMaxS = mSpanVector[0].mMaxS;
                newSpan.mMaxId = mSpanVector[0].mMaxId;
                newSpan.mMaxIsD0 = mSpanVector[0].mMaxIsD0;
                newSpanVector.push_back(newSpan);
            }
        }
    }

    mSpanVector.swap(newSpanVector);
}

void
Region::appendSpan(const Span &span)
{
    assert(empty() || maxS() < span.mMinS);

    mSpanVector.push_back(span);
}

size_t
Region::spanCount() const
{
    return mSpanVector.size();
}

Region::Span
Region::span(size_t index) const
{
    assert(index < mSpanVector.size());

    return mSpanVector[index];
}

meshretri::EndpointIdentifier 
Region::createProjectedDifferenceEndpointIdentifier(
    const meshretri::EndpointIdentifier &oldEndpointIdentifier, 
    WedgeIntersector *wedgeIntersector)
{
    meshretri::EndpointIdentifier id;

    mesh::EdgePtr oldEdgePtr;

    switch (wedgeIntersector->eventType()) {

    case WedgeIntersector::VE_EVENT:
        if (oldEndpointIdentifier.getEdgePtr(&oldEdgePtr)
            && wedgeIntersector->edgePtrIsDefined()) {
            id = meshretri::EndpointIdentifier::fromEdgePtrPairAndVertex(
                oldEdgePtr, wedgeIntersector->edgePtr(), wedgeIntersector->vertexPtr());
        } else {
            id = meshretri::EndpointIdentifier::createUniqueIdentifier();
        }
        break;

    case WedgeIntersector::DISTANT_LIGHT_EE_EVENT:
        if (oldEndpointIdentifier.getEdgePtr(&oldEdgePtr)
            && wedgeIntersector->edgePtrIsDefined()) {
            id = meshretri::EndpointIdentifier::fromEdgePtrPairAndIndex(
                oldEdgePtr, wedgeIntersector->edgePtr(), wedgeIntersector->lightVertexIndex0());
        } else {
            id = meshretri::EndpointIdentifier::createUniqueIdentifier();
        }
        break;

    default:
        id = meshretri::EndpointIdentifier::createUniqueIdentifier();
        break;
    }

    return id;
}
