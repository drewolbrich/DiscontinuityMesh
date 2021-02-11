// Copyright 2009 Drew Olbrich

#ifndef MESHRETRI__RETRIANGULATOR_FACE__INCLUDED
#define MESHRETRI__RETRIANGULATOR_FACE__INCLUDED

#include <cstdlib>

#include <mesh/Types.h>
#include <mesh/Mesh.h>

#include "FaceLineSegmentVector.h"
#include "TriangleVector.h"

class RetriangulatorTest;
class RetriangulatorFaceTest;

namespace meshretri {

class Retriangulator;
class RetriangulatorEdge;
class EdgePoint;

// RetriangulatorFace
//
// Information that Retriangulator keeps track of for each Face.

class RetriangulatorFace 
{
public:
    RetriangulatorFace();
    ~RetriangulatorFace();

    // Backpointer to corresponding mesh face.
    void setFacePtr(const mesh::FacePtr &facePtr);
    const mesh::FacePtr &facePtr() const;

    // Add a FaceLineSegment.
    void addFaceLineSegment(const FaceLineSegment &faceLineSegment);

    // The two axes most perpendicular to the face normal.
    void setAxis0(size_t axis0);
    size_t axis0() const;
    void setAxis1(size_t axis1);
    size_t axis1() const;

    // The axis most parallel to the face normal.
    void setAxis2(size_t axis2);
    size_t axis2() const;

    // Epsilon value for 2D comparisons in the axis0/axis1 plane.
    void setEpsilon(float epsilon);
    float epsilon() const;

    // Weld together FaceLineSegment endpoints that have identical
    // EndpointIdentifiers.
    void weldFaceLineSegmentEndpoints();

    // Create EdgePoints from all FaceLineSegments whose
    // endpoints lie near or intersect edges of the face.
    void createEdgePointsFromEndpointIdentifierEdgePtrs(Retriangulator *retriangulator);

    // Create EdgePoints from line segments that intersect the perimeter
    // of the face. Returns true if one or more EdgePoints were created.
    bool createEdgePointsFromPerimeterIntersections(Retriangulator *retriangulator);

    // Test if any of the perimeter line segments 
    // intersect each other, and correct this by merging EdgePoints.
    // Returns true if one or more FaceLineSegment endpoints were moved.
    bool correctSelfIntersectingFacePerimeterLineSegments(Retriangulator *retriangulator);

    // Correct intersecting EdgePoints from different edges.
    bool correctSelfIntersectingFacePerimeterEdgePoints(Retriangulator *retriangulator);

    // Test if any line segments attached to the perimeter intersect other parts of the
    // perimeter along the line segment's EdgePoint's edge. When this happens, correct the
    // problem by merging neighboring EdgePoints.
    // Returns true if one or more FaceLineSegment endpoints were moved.
    bool correctEdgePointsFromLineSegmentIntersections(Retriangulator *retriangulator);

    // It's still possible that line segments intersect the perimeter.
    // Detect all instances of this and correct them via endpoint snapping.
    bool correctRemainingLineSegmentPerimeterIntersections(Retriangulator *retriangulator);

    // Collapse all line segments whose endpoints both intersect the same edge of the face
    // perimeter. They're redundant any only cause problems because they won't otherwise
    // be collapsed in later steps.
    void collapseEdgeCoincidentLineSegments();

    // Split all mutually intersecting FaceLineSegments.
    // Returns true if any line segments were split.
    bool splitAllMutuallyIntersectingFaceLineSegments();

    // Split the face along the FaceLineSegments.
    void splitMeshFace(mesh::Mesh *mesh, Retriangulator *retriangulator);

    // Split a face for use in a backprojection. The mesh face is not 
    // actually split. Rather, the resulting CDT is returned as a vector of triangles.
    void splitBackprojectionFace(const RetriangulatorEdge *retriangulatorEdgeArray,
        TriangleVector *triangleVector, Retriangulator *retriangulator);

    // Return the number of added FaceLineSegments.
    size_t faceLineSegmentCount() const;

    // Return a reference to a FaceLineSegment.
    FaceLineSegment &faceLineSegment(size_t index);

    // Return the original vertices of the mesh face,
    // before its edges were split.
    const std::vector<mesh::ConstVertexPtr> &originalVertexPtrVector() const;

    // Write the face to an SVG file for debugging.
    void writeToSvgFile(Retriangulator *retriangulator, 
        const std::string &filename = "/var/tmp/retriangulator_dump.svg");

private:
    friend class ::RetriangulatorTest;
    friend class ::RetriangulatorFaceTest;

    // Split a FaceLineSegment at the specified position. The index of the
    // newly created FaceLineSegment is returned.
    void splitFaceLineSegment(size_t oldFaceLineSegmentIndex, 
        const cgmath::Vector3f &splitPosition, bool splitPointIsDegreeZero);

    // Split a FaceLineSegment with another FaceLineSegment that is
    // colinear with it. A third FaceLineSegment may or may not be created.
    // If the line segments intersect at a single shared endpoint,
    // false is returned.
    bool splitColinearFaceLineSegments(size_t oldFaceLineSegmentIndex1,
        size_t oldFaceLineSegmentIndex2);

    // Returns the number of spans between vertices and EdgePoints
    // on the Face's three edges.
    size_t getPerimeterSpanCount(Retriangulator *retriangulator) const;

    // Return the endpoints of a span around the face perimeter in 2D.
    void getPerimeterSpanEndpoints2d(Retriangulator *retriangulator, size_t spanIndex, 
        cgmath::Vector2f *p0, cgmath::Vector2f *p1) const;

    // Return the RetriangulatorEdge associated with a span on the face perimeter.
    RetriangulatorEdge *getPerimeterSpanRetriangulatorEdge(Retriangulator *retriangulator, 
        size_t spanIndex) const;

    // Convert a perimeter span index to a RetriangulatorSpan index.
    size_t mapPerimeterSpanIndexToRetriangulatorEdgeSpanIndex(Retriangulator *retriangulator,
        size_t perimeterSpanIndex) const;

    // Returns true if two spans intersect at more than just a single point.
    // This would make the CDT fail, and it is necessary to correct the perimeter.
    static bool perimeterSpansIntersect(const cgmath::Vector2f &p0, const cgmath::Vector2f &p1, 
        const cgmath::Vector2f &q0, const cgmath::Vector2f &q1);

    // Return the VertexPtr shared by two EdgePtrs.
    mesh::VertexPtr getVertexPtrSharedByTwoEdges(mesh::EdgePtr edgePtr1,
        mesh::EdgePtr edgePtr2) const;

    // Welds all EdgePoints in the specified range to the position of the VertexPtr.
    void weldEdgePointRangeToPoint(RetriangulatorEdge *retriangulatorEdge,
        size_t edgePointIndex1, size_t edgePointIndex2, const cgmath::Vector3f &position);

    // Backpointer to the Face.
    mesh::FacePtr mFacePtr;

    FaceLineSegmentVector mFaceLineSegmentVector;

    // The first two axes below define the plane that is most perpendicular
    // to the face's normal. The third axis is the one that is
    // most parallel to the face's normal.
    int mAxis0;
    int mAxis1;
    int mAxis2;

    float mEpsilon;

    // Vector of the original VertexPtr's of the face, before
    // the edges were split.
    std::vector<mesh::ConstVertexPtr> mOriginalVertexPtrVector;
};

} // namespace meshretri

#endif // MESHRETRI__RETRIANGULATOR_FACE__INCLUDED
