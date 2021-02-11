// Copyright 2009 Drew Olbrich

#ifndef MESHRETRI__EDGE_POINT__INCLUDED
#define MESHRETRI__EDGE_POINT__INCLUDED

#include <cstdlib>

#include <cgmath/Vector3f.h>

namespace meshretri {

class RetriangulatorFace;

// EdgePoint
//
// A point recorded along a RetriangulatorEdge.

class EdgePoint
{
public:
    EdgePoint();
    ~EdgePoint();

    // The RetriangulatorFace whose FaceLineSegment intersected with this edge,
    // causing the creation of this EdgePoint.
    void setRetriangulatorFace(RetriangulatorFace *retriangulatorFace);
    RetriangulatorFace *retriangulatorFace() const;

    // The index of the FaceLineSegment within the RetriangulatorFace.
    // This is only valid up until the point when we start splitting
    // the FaceLineSegments against each other.
    void setFaceLineSegmentIndex(size_t faceLineSegmentIndex);
    size_t faceLineSegmentIndex() const;

    // The endpoint of the FaceLineSegment that intersected the edge.
    // This is only valid up until the point when we start splitting
    // the FaceLineSegments against each other.
    void setEndpointIndex(size_t endpointIndex);
    size_t endpointIndex() const;

    // The position of the point along the edge.
    void setPosition(const cgmath::Vector3f &position);
    const cgmath::Vector3f &position() const;

    // The parametric value of 't' of the point along the edge.
    void setT(double t);
    double t() const;

    // This allows EdgePoints to be sorted by mT using std::sort.
    friend bool operator<(const EdgePoint &lhs, const EdgePoint &rhs);

private:
    RetriangulatorFace *mRetriangulatorFace;
    size_t mFaceLineSegmentIndex;
    size_t mEndpointIndex;
    cgmath::Vector3f mPosition;
    double mT;
};

} // namespace meshretri

#endif // MESHRETRI__EDGE_POINT__INCLUDED
