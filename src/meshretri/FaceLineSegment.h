// Copyright 2008 Drew Olbrich

#ifndef MESHRETRI__FACE_LINE_SEGMENT__INCLUDED
#define MESHRETRI__FACE_LINE_SEGMENT__INCLUDED

#include <cgmath/Vector3f.h>
#include <mesh/Types.h>

#include "EndpointIdentifier.h"

namespace meshretri {

// FaceLineSegment
//
// A critical line segment stored on a face. 
// 
// The Retriangulator class uses FaceLineSegments to determine where to subdivide
// the existing faces of a mesh.

class FaceLineSegment
{
public:
    FaceLineSegment();
    ~FaceLineSegment();

    // The endpoints of the line segment on a face, in worldspace.
    void setWorldPosition(unsigned index, const cgmath::Vector3f &worldPosition);
    const cgmath::Vector3f &worldPosition(unsigned index) const;

    // If true, the line segment endpoint represents a point in the mesh
    // where there is a D0 discontinuity.
    void setIsDegreeZeroDiscontinuity(unsigned index, bool isDegreeZeroDiscontinuity);
    bool isDegreeZeroDiscontinuity(unsigned index) const;

    // A value that identifies the origin of the endpoint.
    // Endpoints with equal EndpointIdentifiers are known to be products
    // of the same geometric events, and therefore should
    // occupy the same positions in space.
    void setEndpointIdentifier(unsigned index, const EndpointIdentifier &endpointIdentifier);
    const EndpointIdentifier &endpointIdentifier(unsigned index) const;

    // The mesh edge that the FaceLineSegment endpoint is coincident with.
    void setEdgePtr(unsigned index, mesh::EdgePtr edgePtr);
    mesh::EdgePtr edgePtr(unsigned index) const;

    // Returns true if the endoint has an assigned EdgePtr.
    bool hasEdgePtr(unsigned index) const;

    // Returns true if both endpoints have the same assigned EdgePtr.
    bool isColinearWithExistingMeshEdge() const;

    // Swap the endpoints.
    void swapEndpoints();

private:
    cgmath::Vector3f mWorldPositionArray[2];
    bool mIsDegreeZeroDiscontinuity[2];
    EndpointIdentifier mEndpointIdentifierArray[2];
    mesh::EdgePtr mEdgePtr[2];
    bool mHasEdgePtr[2];
};

} // namespace meshretri

#endif // MESHRETRI__FACE_LINE_SEGMENT__INCLUDED
