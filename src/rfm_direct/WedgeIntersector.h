// Copyright 2008 Retroactive Fiasco.

#ifndef RFM_DISCMESH__WEDGE_INTERSECTOR__INCLUDED
#define RFM_DISCMESH__WEDGE_INTERSECTOR__INCLUDED

#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <mesh/Types.h>
#include <meshretri/EndpointIdentifier.h>

#include "Endpoint.h"
#include "LineSegment.h"

// WedgeIntersector
//
// This class provides member functions for intersecting wedges with triangles.

class WedgeIntersector
{
public:
    WedgeIntersector();
    ~WedgeIntersector();

    // Define a VE event wedge, given a light source vertex and
    // an occluder edge. Returns false if the wedge is degenerate.
    bool setVeEventWedge(mesh::VertexPtr vertexPtr, mesh::EdgePtr edgePtr);

    // Define an EV event wedge, given a light source edge and
    // an occluder vertex. Returns false if the wedge is degenerate.
    bool setEvEventWedge(mesh::EdgePtr edgePtr, mesh::VertexPtr vertexPtr);

    // Define a distant light EE event wedge.
    bool setDistantLightEeEventWedge(const cgmath::Vector3f &lightVertex0,
        const cgmath::Vector3f &lightVertex1, unsigned lightVertexIndex, mesh::EdgePtr edgePtr);

    // Define a distant light EV event wedge.
    bool setDistantLightEvEventWedge(const cgmath::Vector3f &lightVertex0,
        const cgmath::Vector3f &lightVertex1, 
        unsigned lightVertexIndex0, unsigned lightVertexIndex1, mesh::VertexPtr vertexPtr);

    enum EventType { 
        VE_EVENT, 
        EV_EVENT,
        DISTANT_LIGHT_EE_EVENT,
        DISTANT_LIGHT_EV_EVENT
    };
    EventType eventType() const;

    // The wedge's input vertex.
    bool vertexPtrIsDefined() const;
    mesh::VertexPtr vertexPtr() const;

    // The wedge's input edge.
    bool edgePtrIsDefined() const;
    mesh::EdgePtr edgePtr() const;

    // Light vertex indices from the creation of distant light events.
    unsigned lightVertexIndex0() const;
    unsigned lightVertexIndex1() const;

    // Test the wedge defined above with a mesh face, which must be
    // a triangle. The number of intersections is returned, along with a pointer
    // to an array of line segments.
    int testTriangle(mesh::FacePtr facePtr, LineSegment **lineSegmentArray);

    // Projects a point onto edge PQ of the wedge and returns the parametric
    // coordinate along the length of the edge. Used internally and also by
    // LineSegmentCollection.
    float projectPointOntoEdgePQ(const cgmath::Vector3f &worldSpacePoint) const;

    // Project a point along PQ, specified as parametric value S (a visibility parameter),
    // back toward V onto the specified line segment.
    bool projectEdgePQPointOntoLineSegment(float s,
        const LineSegment &lineSegment, cgmath::Vector3f *point);

    // Transform a world space point to wedge space.
    void transformWorldSpacePointToWedgeSpacePoint(const cgmath::Vector3f &worldSpacePoint,
        cgmath::Vector2f *wedgeSpacePoint);

    // Transform a wedge space point back to world space.
    void transformWedgeSpacePointToWorldSpacePoint(const cgmath::Vector2f &wedgeSpacePoint,
        cgmath::Vector3f *worldSpacePoint);

    // Returns the vector toward the light source from the specified point.
    cgmath::Vector3f getVectorTowardLight(const cgmath::Vector3f &point) const;

    // Returns true if the specified face is adjacent to the wedge vertex or edge.
    bool faceIsAdjacentToWedge(mesh::FacePtr facePtr) const;

    // Returns a unique identifier associated with the wedge being traced.
    // Needed by Region::createProjectedDifferenceEndpointIdentifier.
    unsigned long wedgeIdentifier() const;

private:
    struct ClippableEndpoint {
        ClippableEndpoint() : mPoint(), mId(), mVisibilityParameter(0.0),
                              mIsDegreeZeroDiscontinuity(false) {}
        cgmath::Vector3f mPoint; 
        meshretri::EndpointIdentifier mId;
        float mVisibilityParameter;
        bool mIsDegreeZeroDiscontinuity;
    };

    void initializeEdgePQ();
    bool initializeWedge();
    bool initializeTriangle();
    bool triangleIntersectsWedgePlane();
    bool triangleIsFrontfacing();
    void testTriangleVertices();
    void testTriangleEdges();
    void clipToVeWedge(const ClippableEndpoint &inputCe0, const ClippableEndpoint &inputCe1);
    void clipToEvWedge(const ClippableEndpoint &inputCe0, const ClippableEndpoint &inputCe1);
    bool clipToPlane(const cgmath::Vector3f &origin, const cgmath::Vector3f &normal,
        const meshretri::EndpointIdentifier &newIdIfClipped, 
        float newVisibilityParameterIfClipped, bool markIsDegreeZeroDiscontinuityIfClipped,
        ClippableEndpoint *ep0, ClippableEndpoint *ep1);
    enum PositionRelativeToOccluder { ABOVE_OCCLUDER, BELOW_OCCLUDER };
    void recordClippableEndpointsAsLineSegment(const ClippableEndpoint &ce0,
        const ClippableEndpoint &ce1, PositionRelativeToOccluder positionRelativeToOccluder);
    void snapClippableEndpointToPQ(ClippableEndpoint *p);
    void snapClippableEndpointToV(ClippableEndpoint *p);

    EventType mEventType;

    bool mVertexPtrIsDefined;
    mesh::VertexPtr mVertexPtr;
    bool mEdgePtrIsDefined;
    mesh::EdgePtr mEdgePtr;

    unsigned mLightVertexIndex0;
    unsigned mLightVertexIndex1;

    // Wedge.
    cgmath::Vector3f mV;
    cgmath::Vector3f mW;   // Only used for distant area lights.
    cgmath::Vector3f mP;
    cgmath::Vector3f mQ;
    mesh::VertexPtr mWedgeVertexP;
    mesh::VertexPtr mWedgeVertexQ;
    cgmath::Vector3f mWedgePositiveXAxis;
    cgmath::Vector3f mWedgePositiveYAxis;
    cgmath::Vector3f mWedgePositiveZAxis;

    meshretri::EndpointIdentifier mEndpointIdentifierVP;
    meshretri::EndpointIdentifier mEndpointIdentifierWQ;
    meshretri::EndpointIdentifier mEndpointIdentifierPQ;

    // Triangle.
    mesh::FacePtr mFacePtr;
    mesh::VertexPtr mVertexPtr0;   // A
    mesh::VertexPtr mVertexPtr1;   // B
    mesh::VertexPtr mVertexPtr2;   // C
    mesh::EdgePtr mEdgePtr0;       // AB
    mesh::EdgePtr mEdgePtr1;       // BC
    mesh::EdgePtr mEdgePtr2;       // CA
    cgmath::Vector3f mA;
    cgmath::Vector3f mB;
    cgmath::Vector3f mC;
    float mOrientationA;
    float mOrientationB;
    float mOrientationC;
    cgmath::Vector3f mFaceNormal;

    cgmath::Vector3f mD;
    cgmath::Vector3f mE;
    bool mDIsDefined;
    bool mEIsDefined;
    meshretri::EndpointIdentifier mId0;
    meshretri::EndpointIdentifier mId1;

    int mLineSegmentCount;
    LineSegment mLineSegmentArray[2];

    unsigned long mWedgeIdentifier;
};

#endif // RFM_DISCMESH__WEDGE_INTERSECTOR__INCLUDED
