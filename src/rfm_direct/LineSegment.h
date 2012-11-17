// Copyright 2008 Retroactive Fiasco.

#ifndef RFM_DISCMESH__LINE_SEGMENT__INCLUDED
#define RFM_DISCMESH__LINE_SEGMENT__INCLUDED

#include <vector>

#include <mesh/Types.h>
#include <cgmath/Vector3f.h>
#include <meshretri/EndpointIdentifier.h>

#include "Endpoint.h"

// LineSegment
//
// A 2D line segment in wedge space.

class LineSegment
{
public:
    LineSegment();
    ~LineSegment();

    // First endpoint.
    void setPoint0(const Endpoint &point0);
    const Endpoint &point0() const;

    // Second endpoint.
    void setPoint1(const Endpoint &point1);
    const Endpoint &point1() const;

    // Face that the line segment lies in.
    void setFacePtr(const mesh::FacePtr &facePtr);
    const mesh::FacePtr &facePtr() const;

    // Intersections of the line segment with other line segments.
    // Used by LineSegmentCollection to determine how to subdivide
    // line segments where they intersect, resulting in a set of
    // line segments that do not intersect.
    struct Intersection {
        float mT;
        cgmath::Vector2f mWedgePosition;
        cgmath::Vector3f mWorldPosition;
        meshretri::EndpointIdentifier mEndpointIdentifier;

        bool operator<(const Intersection &rhs) const {
            return mT < rhs.mT;
        }
    };
    void addIntersection(const Intersection &intersection);

    // Sort the vector of intersections by T.
    void sortIntersectionVector();

    // All of the intersections of this line segment with other line segments.
    typedef std::vector<Intersection> IntersectionVector;
    const IntersectionVector &intersectionVector() const;

    // Used by LineSegmentCollection to perform a rough sort a vector of line segments
    // by their minimum Y values, such that the line segment at the beginning of the
    // list will be the farthest away from the light source, and will have the
    // smallest Y value. The goal of LineSegmentCollection's sorting is to
    // order the line segments back to front with respect to the light source,
    // so line segments farthest away from the light source can be processed first.
    bool operator<(const LineSegment &rhs) const;

    // Returns true if the current (LHS) line segment's point nearest the light source
    // (max Y) is nearer than the farthest point of the specified (RHS) line segment 
    bool potentiallyOccludes(const LineSegment &rhs) const;

    // Returns true if the current (LHS) line segment occludes the specified (RHS) line segment.
    bool occludes(const LineSegment &rhs) const;

    // Returns true if the current (LHS) line segment occludes the specified (RHS) line segment,
    // but also handles the case where the two line segments intersect very close
    // to one of the endpoints.
    bool occludesSlightlyIntersecting(const LineSegment &rhs) const;

    // Mark the line segment as having already been reordered.
    // Used in LineSegmentCollection::sortLineSegments to ensure that
    // we won't loop endlessly.
    void setWasReordered(bool wasReordered);
    bool wasReordered() const;

    // True if the line segment is positioned within the wedge
    // between the occluder and the light source.
    void setIsAboveOccluder(bool isAboveOccluder);
    bool isAboveOccluder() const;

private:
    Endpoint mPoint0;
    Endpoint mPoint1;
    mesh::FacePtr mFacePtr;

    IntersectionVector mIntersectionVector;

    bool mWasReordered;
    bool mIsAboveOccluder;
};

#endif // RFM_DISCMESH__LINE_SEGMENT__INCLUDED
