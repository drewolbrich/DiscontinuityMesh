// Copyright 2008 Drew Olbrich

#ifndef MESH__FACE_OPERATIONS__INCLUDED
#define MESH__FACE_OPERATIONS__INCLUDED

#include <cstdlib>
#include <limits>

#include <cgmath/Vector3f.h>
#include <cgmath/BoundingBox3f.h>
#include <cgmath/Tolerance.h>

#include "Mesh.h"
#include "Types.h"

namespace mesh {

class DeletedElementTracker;

// Destroy a face and remove all references to it possessed by adjacent primitives.
void DeleteFace(Mesh *mesh, FacePtr facePtr,
    DeletedElementTracker *deletedElementTracker = NULL);

// Returns true if the specified face is degenerate.
// The face is degenerate if any of the following are true:
//     - It has less than three vertices.
//     - Any pair of vertices are within epsilon distance of each other.
//     - Any vertex is within epsilon distance of a non-adjacent edge.
bool FaceIsDegenerate(ConstFacePtr facePtr, float epsilon);

// Returns true if the specified face is degenerate when projected onto the plane
// defined by two axes.
// The face is considered degenerate if any of the following are true:
//     - It has less than three vertices.
//     - Any pair of vertices are within epsilon distance of each other.
//     - Any vertex is within epsilon distance of a non-adjacent edge.
bool ProjectedFaceIsDegenerate(ConstFacePtr facePtr, int axis1, int axis2, float epsilon);

// Returns true only if all the vertices of the face are coplanar.
// False is also returned if the face has less than three vertices.
bool FaceIsNonplanar(ConstFacePtr facePtr, float epsilon);

// Returns true if any of the edges of the face intersect
// with each other.
bool FaceIsSelfIntersecting(ConstFacePtr facePtr);

// Returns the geometric normal of a face. If the face has less
// than three vertices, Vector3f::zero() is returned. If the
// vertices of the face are not coplanar, the results are undefined.
cgmath::Vector3f GetFaceGeometricNormal(ConstFacePtr facePtr);

// Return the area of a face. This is only well defined if the face is planar.
float GetFaceArea(ConstFacePtr facePtr);

// Return the area of a face when it is projected onto the plane
// defined by the specified two axes.
float GetProjectedFaceArea(ConstFacePtr facePtr, int axis1, int axis2);

// Return the vertex normal of a face vertex if it's defined. Otherwise,
// return the face normal if it's defined. Otherwise, return the
// geometric normal for the face. The argument normal3fKey should be
// the attribute key returned by normal3fAttributeKey.
cgmath::Vector3f GetFaceVertexNormal(ConstFacePtr facePtr, 
    ConstVertexPtr vertexPtr, const AttributeKey &normal3fKey);

// Return the centroid of a face. This assumes that the face is planar.
cgmath::Vector3f GetFaceCentroid(ConstFacePtr facePtr);

// Returns the average position of the face's vertices.
cgmath::Vector3f GetFaceAverageVertexPosition(ConstFacePtr facePtr);

// Returns true if the ray segment is intersected by the specified face.
// Returns false if the face intersects the ray segment beyond its far endpoint,
// as determined by the origin plus the (non unit length) direction vector.
// The value 't' is set to the parameter of the intersection point
// along the ray. If the face is degenerate or nonplanar, the behavior of this
// function is undefined.
// This function only properly handles convex faces.
// The behavior for concave faces is undefined.
bool RaySegmentIntersectsFace(ConstFacePtr facePtr,
    const cgmath::Vector3f &origin, const cgmath::Vector3f &endpoint,
    float *t = NULL);

// Derive an appropriate value of epsilon to use on calculations on a face.
float GetEpsilonFromFace(ConstFacePtr facePtr, 
    float absoluteTolerance = cgmath::TOLERANCE, float relativeTolerance = cgmath::TOLERANCE);

// Returns the bounding box of a face.
cgmath::BoundingBox3f GetFaceBoundingBox(ConstFacePtr facePtr);

// Returns the vertices adjacent to a triangular face.
void GetTriangularFaceAdjacentVertices(ConstFacePtr facePtr,
    VertexPtr *v0, VertexPtr *v1, VertexPtr *v2);

// Returns the edges adjacent to a triangular face.
void GetTriangularFaceAdjacentEdges(ConstFacePtr facePtr,
    EdgePtr *e0, EdgePtr *e1, EdgePtr *e2);

// Returns the positions of the three vertices of a triangular face.
void GetTriangularFaceVertexPositions(ConstFacePtr facePtr,
    cgmath::Vector3f *p0, cgmath::Vector3f *p1, cgmath::Vector3f *p2);

// Given a triangular face and one edge, return the other two edges.
void GetTriangularFaceOtherEdges(ConstFacePtr facePtr,
    ConstEdgePtr edgePtr, EdgePtr *otherEdgePtr1, EdgePtr *otherEdgePtr2);

// Given a triangular face and two edges, return the other edge.
void GetTriangularFaceOtherEdge(ConstFacePtr facePtr,
    ConstEdgePtr edgePtr1, ConstEdgePtr edgePtr2, EdgePtr *otherEdgePtr);

// Given a triangulator face, return the edge opposing the specified vertex.
void GetTriangularFaceEdgeOpposingVertex(ConstFacePtr facePtr,
    ConstVertexPtr vertexPtr, EdgePtr *opposingEdgePtr);

// Create a triangular face and its edges (if they do not already exist)
// from three vertices.
FacePtr CreateTriangularFaceAndEdgesFromVertices(Mesh *mesh,
    VertexPtr vertexPtr0, VertexPtr vertexPtr1, VertexPtr vertexPtr2);

// Returns true if all of the vertices of the face are nearly mutually coincident.
bool AllFaceVerticesAreCoincident(ConstFacePtr facePtr, float epsilon);

// Collapses all of the vertices of the face to a single vertex.
// This function only works for meshes consisting entirely of triangles.
// If the optional deletedElementTracker argument is specified, it will track 
// elements that are deleted as a result of the face collapse operation.
// All vertices of the face are collapsed into the face's first vertex,
// which is returned.
VertexPtr CollapseFace(Mesh *mesh, FacePtr facePtr, 
    DeletedElementTracker *deletedElementTracker = NULL);

// Given a quadrilateral face, replace it with two triangular faces
// that have the specified vertex in common. 
// Mesh primitives that are created or deleted are kept track of
// by the optionally specified DeletedElementTracker.
void TriangulateQuadrilateralFaceFromVertex(Mesh *mesh, FacePtr facePtr, VertexPtr vertexPtr, 
    DeletedElementTracker *deletedElementTracker = NULL);

// Triangulate a quadrilateral face by adding an edge between the closest
// together of the two opposing pairs of vertices.
void TriangulateQuadrilateralFace(Mesh *mesh, FacePtr facePtr,
    DeletedElementTracker *deletedElementTracker = NULL);

} // namespace mesh

#endif // MESH__FACE_OPERATIONS__INCLUDED
