// Copyright 2009 Drew Olbrich

#ifndef MESHRETRI__RETRIANGULATOR_EDGE__INCLUDED
#define MESHRETRI__RETRIANGULATOR_EDGE__INCLUDED

#include <vector>

#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <mesh/AttributeKey.h>
#include <cgmath/Vector3f.h>

#include "EdgePoint.h"

namespace meshretri {

// RetriangulatorEdge
//
// Information that Retriangulator keeps track of for each edge.

class RetriangulatorEdge
{
public:
    RetriangulatorEdge();
    ~RetriangulatorEdge();

    // Backpointer to the corresponding mesh edge.
    void setEdgePtr(const mesh::EdgePtr &edgePtr);
    const mesh::EdgePtr &edgePtr() const;

    // Epsilon value for 3D comparisons along the edge.
    void setEpsilon(float epsilon);
    float epsilon() const;

    // Add a EdgePoint to the edge.
    void addEdgePoint(const EdgePoint &edgePoint);

    // Merge EdgePoints what were created from the same wedge,
    // even if they are not coincident.
    void weldEdgePointsCreatedFromCommonWedge();

    // Merge EdgePoints that have the same value of 't'.
    // This eliminates funky cases where the edge folds back on itself
    // multiple times at extremely small scales.
    void weldParametricallyEqualEdgePoints();

    // Split the mesh edge at each EdgePoint.
    void splitMeshEdge(mesh::Mesh *mesh,
        mesh::AttributeKey isDegreeZeroDiscontinuityAttributeKey);

    // Assign the positions of the EdgePoints from their FaceLineSegment endpoint
    // correspondence, and sort them in order of their position along the edge.
    void updateEdgePoints();

    // Return the number of EdgePoints along the edge.
    size_t edgePointCount() const;

    // Return an EdgePoint.
    const EdgePoint &edgePoint(size_t index) const;

    // Returns the number of spans that the edge is divided up into
    // by its EdgePoints.
    size_t spanCount() const;

    // Return the endpoints of a span in 2D.
    void getSpanEndpoints2d(size_t spanIndex, cgmath::Vector2f *p0, cgmath::Vector2f *p1, 
        int axis0, int axis1) const;

    // Returns the index of the EdgePoint nearest the VertexPtr.
    size_t getEdgePointIndexNearestVertex(mesh::VertexPtr vertexPtr) const;

    // Returns the EdgePoint index of the span endpoint closest to the
    // specified EdgePoint index.
    size_t getNearestEdgePointFromSpan(size_t spanIndex, size_t edgePointIndex) const;

    // Returns the EdgePoint index of the span endpoint farthest from the
    // specified EdgePoint index.
    size_t getFarthestEdgePointFromSpan(size_t spanIndex, size_t edgePointIndex) const;

private:
    // Backpointer to the Edge.
    mesh::EdgePtr mEdgePtr;

    typedef std::vector<EdgePoint> EdgePointVector;
    EdgePointVector mEdgePointVector;

    float mEpsilon;
};

} // namespace meshretri

#endif // MESHRETRI__RETRIANGULATOR_EDGE__INCLUDED
