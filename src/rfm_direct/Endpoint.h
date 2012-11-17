// Copyright 2008 Retroactive Fiasco.

#ifndef RFM_DISCMESH__ENDPOINT__INCLUDED
#define RFM_DISCMESH__ENDPOINT__INCLUDED

#include <limits>

#include <cgmath/Vector3f.h>
#include <cgmath/Vector2f.h>

#include <meshretri/EndpointIdentifier.h>

// Endpoint
//
// The endpoint of a critical line segment formed by intersecting
// a wedge with a triangle.

class Endpoint
{
public:
    Endpoint();
    ~Endpoint();

    // The 3D worldspace position of the endpoint.
    void setWorldPosition(const cgmath::Vector3f &worldPosition);
    const cgmath::Vector3f &worldPosition() const;

    // The 2D wedgespace position of the endpoint.
    void setWedgePosition(const cgmath::Vector2f &wedgePosition);
    const cgmath::Vector2f &wedgePosition() const;

    // The 1D parametric value of the point when projected from point V onto
    // edge PQ of the wedge.
    static const float UNDEFINED_VISIBILITY_PARAMETER;
    void setVisibilityParameter(float visibilityParameter);
    float visibilityParameter() const;

    // If true, the line segment endpoint represents a point in the mesh
    // where there is a D0 discontinuity.
    void setIsDegreeZeroDiscontinuity(bool isDegreeZeroDiscontinuity);
    bool isDegreeZeroDiscontinuity() const;

    // A value that can be used to uniquely identify the endpoint,
    // even when comparisions of the 3D worldspace position would fail
    // due to floating point roundoff error.
    void setEndpointIdentifier(const meshretri::EndpointIdentifier &endpointIdentifier);
    const meshretri::EndpointIdentifier &endpointIdentifier() const;

private:
    cgmath::Vector3f mWorldPosition;
    cgmath::Vector2f mWedgePosition;

    float mVisibilityParameter;

    bool mIsDegreeZeroDiscontinuity;

    meshretri::EndpointIdentifier mEndpointIdentifier;
};

#endif // RFM_DISCMESH__ENDPOINT__INCLUDED
