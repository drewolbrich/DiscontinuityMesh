// Copyright 2009 Drew Olbrich

#ifndef MESHRETRI__RETRIANGULATOR__INCLUDED
#define MESHRETRI__RETRIANGULATOR__INCLUDED

#include <mesh/Mesh.h>
#include <mesh/Types.h>

#include "FaceLineSegment.h"
#include "FaceLineSegmentVector.h"
#include "RetriangulatorFace.h"
#include "RetriangulatorEdge.h"
#include "TriangleVector.h"

class RetriangulatorTest;
class RetriangulatorFaceTest;

namespace meshretri {

class PointSnapper;
class EdgePoint;

// Retriangulator
//
// This class maps vectors of FaceLineSegments to Faces, and then 
// uses constrained Delaunay triangulation to retriangulate the faces.

class Retriangulator
{
public:
    Retriangulator();
    ~Retriangulator();

    // The mesh that's going to be retriangulated.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Absolute tolerance to use when testing for coincident endpoints
    // of FaceLineSegments or intersections of FaceLineSegments with mesh edges.
    void setAbsoluteTolerance(float absoluteTolerance);
    float absoluteTolerance() const;

    // Relative tolerance. When large values are compared, the relative tolerance
    // supercedes the absolute tolerance.
    void setRelativeTolerance(float relativeTolerance);
    float relativeTolerance() const;

    // Optional boolean attribute assigned (true) to new edges inserted into
    // the interior of a face. This attribute is not assigned
    // to edges resulting from splitting existing perimeter edges of faces.
    void setNewEdgeBooleanAttributeKey(const mesh::AttributeKey &newEdgeBooleanAttributeKey);
    const mesh::AttributeKey &newEdgeBooleanAttributeKey() const;

    // Add a FaceLineSegment to the specified face.
    void addFaceLineSegmentToFace(const FaceLineSegment &faceLineSegment, 
        mesh::FacePtr facePtr);

    // Retriangulate the mesh.
    // This function can throws an except::FailedOperationException if it encounters
    // problematic geometry, like a degenerate face.
    void retriangulate();

    // Retriangulate a single backprojection face. The mesh is not modified to reflect the
    // triangulation. Rather, the coordinates of the triangles are returned separately.
    // This function can throws an except::FailedOperationException if it encounters
    // problematic geometry, like a degenerate face.
    void retriangulateBackprojectionFace(mesh::FacePtr facePtr, TriangleVector *triangleVector);

private:
    friend class ::RetriangulatorTest;
    friend class ::RetriangulatorFaceTest;
    friend class RetriangulatorFace;

    // Filter all the line segments in preparation for splitting the mesh faces.
    // This function ensures that the line segments are in a form that is
    // suitable for submitting to the constrained Delaunay triangulator.
    // For example, intersecting line segments are split against each other.
    void filterFaceLineSegments();

    // Reset the class for another use.
    void reset();

    // Add a EdgePoint to the RetriangulatorEdge corresponding
    // to the specified EdgePtr.
    void addEdgePointToRetriangulatorEdge(mesh::EdgePtr edgePtr,
        RetriangulatorFace *retriangulatorFace, size_t faceLineSegmentIndex, 
        size_t endpointIndex);

    // Return the RetriangualtorEdge corresponding to a mesh EdgePtr.
    RetriangulatorEdge *getRetriangulatorEdgeFromEdgePtr(mesh::EdgePtr edgePtr);

    // Initialize vectors of RetriangulatorFaces and RetriangulatorEdges.
    void initialize();

    // Initialize the vector of RetriangulatorFaces.
    void initializeRetriangulatorFaces();

    // Initialize the vector of RetriangulatorEdges.
    void initializeRetriangulatorEdges();

    void initializeRetriangulatorEdgeFromEdgePtr(mesh::EdgePtr edgePtr);

    // Returns the RetriangulatorFace associated with a mesh face.
    RetriangulatorFace *getRetriangulatorFaceFromMeshFace(mesh::FacePtr facePtr);

    // Returns true if the mesh face has a corresponding RetriangulatorFace.
    bool meshFaceHasRetriangulatorFace(mesh::FacePtr facePtr) const;

    // Returns the RetriangulatorEdge associated with a mesh edge.
    RetriangulatorEdge *getRetriangulatorEdgeFromMeshEdge(mesh::EdgePtr edgePtr);

    // Returns true if the mesh edge has a corresponding RetriangulatorEdge.
    bool meshEdgeHasRetriangulatorEdge(mesh::EdgePtr edgePtr) const;

    // Weld together FaceLineSegment endpoints that have identical
    // EndpointIdentifiers.
    void weldFaceLineSegmentEndpoints();

    // Create EdgePoints for all RetriangulatorFaces based on EdgePtrs
    // referenced by EndpointIdentifiers.
    void createEdgePointsFromEndpointIdentifierEdgePtrs();

    // Find faces adjacent to edges with EdgePoints, and keep track of them
    // in a separate vector, so that RetriangulatorFace pointers
    // of EdgePoints won't be invalidated.
    void createAdditionalRetriangulatorFaces();

    // Merge all EdgePoints on the same edge that share common wedge identifiers.
    // Floating point error may result in multiple FaceLineSegment endpoints
    // along the same mesh edge with slightly different positions.
    void weldEdgePointsCreatedFromCommonWedge();

    // Merge EdgePoints that have the same value of 't'.
    // This eliminates funky cases where the edge folds back on itself
    // multiple times at extremely small scales.
    void weldParametricallyEqualEdgePoints();

    // Set the positions of the EdgePoints along each edge, and sort them.
    void updateEdgePoints();

    // Create EdgePoints from line segments that intersect the perimeters of the faces. 
    // Returns true if any EdgePoints were created.
    void createEdgePointsFromPerimeterIntersections();

    // For each face, test if any of the perimeter line segments 
    // intersect each other, and correct this by merging EdgePoints
    // with the nearest mesh vertex as necessary.
    bool correctSelfIntersectingFacePerimeterLineSegments();

    // Correct intersecting EdgePoints from different edges.
    bool correctSelfIntersectingFacePerimeterEdgePoints();

    // For each face, test if any line segments attached to the perimeter
    // intersect other parts of the perimeter along that edge.
    // When this happens, correct the problem by merging neighboring EdgePoints.
    bool correctEdgePointsFromLineSegmentIntersections();

    // It's still possible that line segments intersect the perimeter.
    // Detect all instances of this and correct them via endpoint snapping.
    bool correctRemainingLineSegmentPerimeterIntersections();

    // Collapse all line segments that stretch from one vertex edge to another.
    // They're redundant any only cause problems because they
    // won't otherwise be collapsed in later steps.
    void collapseEdgeCoincidentLineSegments();

    // Split all mutually intersecting FaceLineSegments.
    void splitAllMutuallyIntersectingFaceLineSegments();

    // Split the mesh edges using the EdgePoints stored
    // with each RetriangulatorEdge.
    void splitMeshEdges();

    // Split the mesh faces using the FaceLineSegments stored
    // with each RetriangulatorFace.
    void splitMeshFaces();

    // Add a new RetriangulatorFace, pointing at a mesh face.
    // The mesh face is assigned a backpointer index attribute 
    // to the RetriangulatorFace index.
    size_t addRetriangulatorFace(mesh::FacePtr facePtr);

    // Add a RetriangulatorFace to a second vector of RetriangulatorFaces,
    // used so that additional faces can be added without invaliding
    // the RetriangulatorFace pointers possessed by EdgePoints.
    size_t addAdditionalRetriangulatorFace(mesh::FacePtr facePtr);

    // Add a single face to a TriangleVector.
    void copyFaceToTriangleVector(mesh::FacePtr facePtr, TriangleVector *triangleVector);

    // Reset the attributes assigned to a face and its adjacent edges.
    void resetBackprojectionFaceAttributes(mesh::FacePtr facePtr);

    RetriangulatorFace *getLargestRetriangulatorFace();

    bool isDebugFace(mesh::FacePtr facePtr);
    bool dumpFace(const std::string &filename);

    mesh::Mesh *mMesh;
    mesh::AttributeKey mRetriangulatorFaceIndexKey;
    mesh::AttributeKey mRetriangulatorEdgeIndexKey;

    float mAbsoluteTolerance;
    float mRelativeTolerance;

    mesh::AttributeKey mNewEdgeBooleanAttributeKey;

    typedef std::vector<RetriangulatorFace> RetriangulatorFaceVector;
    RetriangulatorFaceVector mRetriangulatorFaceVector;

    // A second set of faces with no FaceLineSegments, but neighboring
    // edges that must be split. These faces must also be
    // retriangulated. We store them in a separate vector
    // so we can grow the vector without worrying about invalidating
    // the RetriangulatorFace pointers of EdgePoints.
    RetriangulatorFaceVector mAdditionalRetriangulatorFaceVector;

    typedef std::vector<RetriangulatorEdge> RetriangulatorEdgeVector;
    RetriangulatorEdgeVector mRetriangulatorEdgeVector;

    bool mRetriangulatingBackprojectionFace;
};

} // namespace meshretri

#endif // MESHRETRI__RETRIANGULATOR__INCLUDED
