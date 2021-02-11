// Copyright 2008 Drew Olbrich

#include "Endpoint.h"

const float Endpoint::UNDEFINED_VISIBILITY_PARAMETER = std::numeric_limits<float>::max();

Endpoint::Endpoint()
    : mWorldPosition(),
      mWedgePosition(),
      mVisibilityParameter(UNDEFINED_VISIBILITY_PARAMETER),
      mIsDegreeZeroDiscontinuity(false),
      mEndpointIdentifier()
{
}

Endpoint::~Endpoint()
{
}

void
Endpoint::setWorldPosition(const cgmath::Vector3f &worldPosition)
{
    mWorldPosition = worldPosition;
}

const cgmath::Vector3f &
Endpoint::worldPosition() const
{
    return mWorldPosition;
}

void
Endpoint::setWedgePosition(const cgmath::Vector2f &wedgePosition)
{
    mWedgePosition = wedgePosition;
}

const cgmath::Vector2f &
Endpoint::wedgePosition() const
{
    return mWedgePosition;
}

void
Endpoint::setVisibilityParameter(float visibilityParameter)
{
    mVisibilityParameter = visibilityParameter;
}

float
Endpoint::visibilityParameter() const
{
    return mVisibilityParameter;
}

void
Endpoint::setIsDegreeZeroDiscontinuity(bool isDegreeZeroDiscontinuity)
{
    mIsDegreeZeroDiscontinuity = isDegreeZeroDiscontinuity;
}

bool
Endpoint::isDegreeZeroDiscontinuity() const
{
    return mIsDegreeZeroDiscontinuity;
}

void
Endpoint::setEndpointIdentifier(const meshretri::EndpointIdentifier &endpointIdentifier)
{
    mEndpointIdentifier = endpointIdentifier;
}

const meshretri::EndpointIdentifier &
Endpoint::endpointIdentifier() const
{
    return mEndpointIdentifier;
}
