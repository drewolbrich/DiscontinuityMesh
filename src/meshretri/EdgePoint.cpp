// Copyright 2009 Retroactive Fiasco.

#include "EdgePoint.h"

namespace meshretri {

EdgePoint::EdgePoint()
    : mRetriangulatorFace(NULL),
      mFaceLineSegmentIndex(0),
      mEndpointIndex(0),
      mPosition(cgmath::Vector3f::ZERO),
      mT(0.0)
{
}

EdgePoint::~EdgePoint()
{
}

void
EdgePoint::setRetriangulatorFace(RetriangulatorFace *retriangulatorFace)
{
    mRetriangulatorFace = retriangulatorFace;
}

RetriangulatorFace *
EdgePoint::retriangulatorFace() const
{
    return mRetriangulatorFace;
}

void
EdgePoint::setFaceLineSegmentIndex(size_t faceLineSegmentIndex)
{
    mFaceLineSegmentIndex = faceLineSegmentIndex;
}

size_t
EdgePoint::faceLineSegmentIndex() const
{
    return mFaceLineSegmentIndex;
}

void
EdgePoint::setEndpointIndex(size_t endpointIndex)
{
    mEndpointIndex = endpointIndex;
}

size_t
EdgePoint::endpointIndex() const
{
    return mEndpointIndex;
}

void
EdgePoint::setPosition(const cgmath::Vector3f &position)
{
    mPosition = position;
}

const cgmath::Vector3f &
EdgePoint::position() const
{
    return mPosition;
}

void
EdgePoint::setT(double t)
{
    mT = t;
}

double
EdgePoint::t() const
{
    return mT;
}

bool
operator<(const EdgePoint &lhs, const EdgePoint &rhs)
{
    return lhs.mT < rhs.mT;
}

} // namespace meshretri
