// Copyright 2008 Retroactive Fiasco.

#include "FaceLineSegment.h"

#include <cassert>
#include <algorithm>

namespace meshretri {

FaceLineSegment::FaceLineSegment()
    : mWorldPositionArray(),
      mIsDegreeZeroDiscontinuity(),
      mEndpointIdentifierArray(),
      mEdgePtr(),
      mHasEdgePtr()

{
    mIsDegreeZeroDiscontinuity[0] = false;
    mIsDegreeZeroDiscontinuity[1] = false;

    mHasEdgePtr[0] = false;
    mHasEdgePtr[1] = false;
}

FaceLineSegment::~FaceLineSegment()
{
}

void
FaceLineSegment::setWorldPosition(unsigned index, const cgmath::Vector3f &worldPosition)
{
    assert(index < 2);

    mWorldPositionArray[index] = worldPosition;
}

const cgmath::Vector3f &
FaceLineSegment::worldPosition(unsigned index) const
{
    assert(index < 2);
    
    return mWorldPositionArray[index];
}

void
FaceLineSegment::setIsDegreeZeroDiscontinuity(unsigned index, bool isDegreeZeroDiscontinuity)
{
    assert(index < 2);

    mIsDegreeZeroDiscontinuity[index] = isDegreeZeroDiscontinuity;
}

bool
FaceLineSegment::isDegreeZeroDiscontinuity(unsigned index) const
{
    assert(index < 2);
    
    return mIsDegreeZeroDiscontinuity[index];
}

void
FaceLineSegment::setEndpointIdentifier(unsigned index, 
    const EndpointIdentifier &endpointIdentifier)
{
    assert(index < 2);

    mEndpointIdentifierArray[index] = endpointIdentifier;
}

const EndpointIdentifier &
FaceLineSegment::endpointIdentifier(unsigned index) const
{
    assert(index < 2);

    return mEndpointIdentifierArray[index];
}

void
FaceLineSegment::setEdgePtr(unsigned index, mesh::EdgePtr edgePtr)
{
    assert(index < 2);

    mEdgePtr[index] = edgePtr;
    mHasEdgePtr[index] = true;
}

mesh::EdgePtr
FaceLineSegment::edgePtr(unsigned index) const
{
    assert(index < 2);

    return mEdgePtr[index];
}

bool
FaceLineSegment::hasEdgePtr(unsigned index) const
{
    assert(index < 2);

    return mHasEdgePtr[index];
}

bool
FaceLineSegment::isColinearWithExistingMeshEdge() const
{
    return mHasEdgePtr[0]
        && mHasEdgePtr[1]
        && mEdgePtr[0] == mEdgePtr[1];
}

void
FaceLineSegment::swapEndpoints()
{
    std::swap(mWorldPositionArray[0], mWorldPositionArray[1]);
    std::swap(mIsDegreeZeroDiscontinuity[0], mIsDegreeZeroDiscontinuity[1]);
    std::swap(mEndpointIdentifierArray[0], mEndpointIdentifierArray[1]);
    std::swap(mEdgePtr[0], mEdgePtr[1]);
    std::swap(mHasEdgePtr[0], mHasEdgePtr[1]);
}

} // namespace meshretri
