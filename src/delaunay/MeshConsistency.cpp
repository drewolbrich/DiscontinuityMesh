// Copyright 2008 Drew Olbrich

#include "MeshConsistency.h"

#include <cassert>
#include <vector>
#include <iostream>
#include <algorithm>

// We can't include cmath because isnan, ininf, and isnormal aren't defined
// in it for some reason, at least under Mac OS X 10.4.
#include <math.h>

#include <exact/GeometricPredicates.h>
#include <cgmath/Vector2f.h>

#include "Mesh.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "EdgeOperations.h"

using cgmath::Vector2f;

namespace delaunay {

// Tests that the vertex positions of the mesh are all finite.
// They aren't "not a number" or infinity.
static void TestFiniteVertexPositions(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that the vertex positions are all unique within the mesh.
static void TestDuplicateVertices(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that the edges of the mesh are all unique. That is, that there 
// are no two edges that share the same two vertices.
static void TestDuplicateEdges(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that the faces of the mesh are all unique. That is, that there 
// are no two faces that share the same three vertices.
static void TestDuplicateFaces(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that each vertex has at least two edges that are unique within the vertex
// and that each vertex has at least two faces that are unique within the vertex.
static void TestMeshVertexConsistency(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that each edge has two vertices that are unique within the edge
// and either one or two faces that are unique within the edge.
static void TestMeshEdgeConsistency(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that each face has three vertices that are unique within the face
// and three edges that are unique within the face.
static void TestMeshFaceConsistency(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that the vertices of each face are specified in counterclockwise order
// and that the edges of each face are specified in counterclockwise order.
static void TestCounterclockwiseMeshFaces(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that edges adjacent to each vertex have pointers back to the vertex.
static void TestMeshVertexEdgeBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that faces adjacent to each vertex have pointers back to the vertex.
static void TestMeshVertexFaceBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that vertices adjacent to each edge have pointers back to the edge.
static void TestMeshEdgeVertexBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that faces adjacent to each edge have pointers back to the edge.
static void TestMeshEdgeFaceBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that vertices adjacent to each face have pointers back to the face.
static void TestMeshFaceVertexBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that edges adjacent to each face have pointers back to the face.
static void TestMeshFaceEdgeBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that the vertices and edges of each face have a consistent order.
// The first adjacent edge of a face must have as adjacent vertices
// the first and second adjacent vertices of the face, and so on.
static void TestVertexEdgeOrderOfFaces(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Tests that whenever an edge has two faces, the two faces lie on opposite
// sides of the edge.
static void TestEdgeFaceOrientation(MeshPtr meshPtr, MeshConsistencyStatus &status);

// Used by TestDuplicateEdges to sort a vector of edge pointers in order of
// their smallest vertex pointer.
struct CompareEdgesByVertices {
    bool operator()(EdgePtr edge1, EdgePtr edge2) {
        VertexPtr edge1MinVertex;
        VertexPtr edge1MaxVertex;
        assert(edge1->adjacentVertexCount() == 2);
        if (&*edge1->adjacentVertex(0) < &*edge1->adjacentVertex(1)) {
            edge1MinVertex = edge1->adjacentVertex(0);
            edge1MaxVertex = edge1->adjacentVertex(1);
        } else {
            edge1MinVertex = edge1->adjacentVertex(1);
            edge1MaxVertex = edge1->adjacentVertex(0);
        }

        VertexPtr edge2MinVertex;
        VertexPtr edge2MaxVertex;
        assert(edge2->adjacentVertexCount() == 2);
        if (&*edge2->adjacentVertex(0) < &*edge2->adjacentVertex(1)) {
            edge2MinVertex = edge2->adjacentVertex(0);
            edge2MaxVertex = edge2->adjacentVertex(1);
        } else {
            edge2MinVertex = edge2->adjacentVertex(1);
            edge2MaxVertex = edge2->adjacentVertex(0);
        }

        if (&*edge1MinVertex == &*edge2MinVertex) {
            return &*edge1MaxVertex < &*edge2MaxVertex;
        } else {
            return &*edge1MinVertex < &*edge2MinVertex;
        }
    }
};

// Used by CompareFacesByVertices and TestDuplicateFaces to compare two VertexPtrs.
struct CompareVertexPtrs {
    bool operator()(VertexPtr vertex1, VertexPtr vertex2) {
        return &*vertex1 < &*vertex2;
    }
};

// Used by TestDuplicateFaces to sort a vector of face pointers in order of
// their smallest vertex pointer.
struct CompareFacesByVertices {
    bool operator()(FacePtr face1, FacePtr face2) {
        VertexPtr face1Vertices[3];
        assert(face1->adjacentVertexCount() == 3);
        face1Vertices[0] = face1->adjacentVertex(0);
        face1Vertices[1] = face1->adjacentVertex(1);
        face1Vertices[2] = face1->adjacentVertex(2);

        VertexPtr face2Vertices[3];
        assert(face2->adjacentVertexCount() == 3);
        face2Vertices[0] = face2->adjacentVertex(0);
        face2Vertices[1] = face2->adjacentVertex(1);
        face2Vertices[2] = face2->adjacentVertex(2);

        std::sort(&face1Vertices[0], &face1Vertices[3], CompareVertexPtrs());
        std::sort(&face2Vertices[0], &face2Vertices[3], CompareVertexPtrs());

        if (face1Vertices[0] == face2Vertices[0]) {
            if (face1Vertices[1] == face2Vertices[1]) {
                return &*face1Vertices[2] < &*face2Vertices[2];
            } else {
                return &*face1Vertices[1] < &*face2Vertices[1];
            }
        } else {
            return &*face1Vertices[0] < &*face2Vertices[0];
        }
    }
};

MeshConsistencyStatus
TestMeshConsistency(MeshPtr meshPtr)
{
    MeshConsistencyStatus status;

    TestFiniteVertexPositions(meshPtr, status);
    TestDuplicateVertices(meshPtr, status);
    TestDuplicateEdges(meshPtr, status);
    TestDuplicateFaces(meshPtr, status);
    TestMeshVertexConsistency(meshPtr, status);
    TestMeshEdgeConsistency(meshPtr, status);
    TestMeshFaceConsistency(meshPtr, status);
    TestCounterclockwiseMeshFaces(meshPtr, status);
    TestMeshVertexEdgeBackpointers(meshPtr, status);
    TestMeshVertexFaceBackpointers(meshPtr, status);
    TestMeshEdgeVertexBackpointers(meshPtr, status);
    TestMeshEdgeFaceBackpointers(meshPtr, status);
    TestMeshFaceVertexBackpointers(meshPtr, status);
    TestMeshFaceEdgeBackpointers(meshPtr, status);
    TestVertexEdgeOrderOfFaces(meshPtr, status);
    TestEdgeFaceOrientation(meshPtr, status);

    return status;
}

static void
TestFiniteVertexPositions(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (VertexPtr vertexPtr = meshPtr->vertexBegin();
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
        const Vector2f position = vertexPtr->position();

        // The vertex position must not be "not a number".
        if (isnan(position[0])
            || isnan(position[1])) {
            status.incrementInconsistency(NAN_VERTICES);
        }

        // The vertex position must not be infinite.
        if (isinf(position[0])
            || isinf(position[1])) {
            status.incrementInconsistency(INFINITY_VERTICES);
        }
    }
}

static void 
TestDuplicateVertices(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    typedef std::vector<cgmath::Vector2f> Vector2fVector;
    Vector2fVector vector2fVector;
    vector2fVector.reserve(meshPtr->vertexCount());

    for (VertexPtr vertexPtr = meshPtr->vertexBegin(); 
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
        vector2fVector.push_back(vertexPtr->position());
    }

    std::sort(vector2fVector.begin(), vector2fVector.end());

    for (Vector2fVector::size_type index = 0;
         index + 1 < vector2fVector.size(); ++index) {
        // None of the vertices should have the same position.
        if (vector2fVector[index] == vector2fVector[index + 1]) {
            status.incrementInconsistency(DUPLICATE_VERTICES);
        }
    }
}

static void 
TestDuplicateEdges(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    typedef std::vector<EdgePtr> EdgePtrVector;
    EdgePtrVector edgePtrVector;
    edgePtrVector.reserve(meshPtr->edgeCount());

    for (EdgePtr edgePtr = meshPtr->edgeBegin();
         edgePtr != meshPtr->edgeEnd(); ++edgePtr) {
        if (edgePtr->adjacentVertexCount() == 2) {
            edgePtrVector.push_back(edgePtr);
        }
    }

    std::sort(edgePtrVector.begin(), edgePtrVector.end(), CompareEdgesByVertices());

    for (EdgePtrVector::size_type index = 0;
         index + 1 < edgePtrVector.size(); ++index) {
        assert(edgePtrVector[index]->adjacentVertexCount() == 2);
        assert(edgePtrVector[index + 1]->adjacentVertexCount() == 2);
        if ((edgePtrVector[index]->adjacentVertex(0) 
                == edgePtrVector[index + 1]->adjacentVertex(0)
                && edgePtrVector[index]->adjacentVertex(1) 
                == edgePtrVector[index + 1]->adjacentVertex(1))
            || (edgePtrVector[index]->adjacentVertex(0) 
                == edgePtrVector[index + 1]->adjacentVertex(1)
                && edgePtrVector[index]->adjacentVertex(1) 
                == edgePtrVector[index + 1]->adjacentVertex(0))) {
            status.incrementInconsistency(DUPLICATE_EDGES);
        }
    }
}

static void 
TestDuplicateFaces(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    typedef std::vector<FacePtr> FacePtrVector;
    FacePtrVector facePtrVector;
    facePtrVector.reserve(meshPtr->faceCount());

    for (FacePtr facePtr = meshPtr->faceBegin();
         facePtr != meshPtr->faceEnd(); ++facePtr) {
        if (facePtr->adjacentVertexCount() == 3) {
            facePtrVector.push_back(facePtr);
        }
    }

    std::sort(facePtrVector.begin(), facePtrVector.end(), CompareFacesByVertices());

    for (FacePtrVector::size_type index = 0;
         index + 1 < facePtrVector.size(); ++index) {

        VertexPtr face1Vertices[3];
        assert(facePtrVector[index]->adjacentVertexCount() == 3);
        face1Vertices[0] = facePtrVector[index]->adjacentVertex(0);
        face1Vertices[1] = facePtrVector[index]->adjacentVertex(1);
        face1Vertices[2] = facePtrVector[index]->adjacentVertex(2);

        VertexPtr face2Vertices[3];
        assert(facePtrVector[index + 1]->adjacentVertexCount() == 3);
        face2Vertices[0] = facePtrVector[index + 1]->adjacentVertex(0);
        face2Vertices[1] = facePtrVector[index + 1]->adjacentVertex(1);
        face2Vertices[2] = facePtrVector[index + 1]->adjacentVertex(2);

        std::sort(&face1Vertices[0], &face1Vertices[3], CompareVertexPtrs());
        std::sort(&face2Vertices[0], &face2Vertices[3], CompareVertexPtrs());

        if (face1Vertices[0] == face2Vertices[0]
            && face1Vertices[1] == face2Vertices[1]
            && face1Vertices[2] == face2Vertices[2]) {
            status.incrementInconsistency(DUPLICATE_FACES);
        }
    }
}

static void 
TestMeshVertexConsistency(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (VertexPtr vertexPtr = meshPtr->vertexBegin(); 
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {

        // The vertex must have at least two adjacent edges.
        if (vertexPtr->adjacentEdgeCount() == 0) {
            status.incrementInconsistency(VERTICES_WITH_ZERO_EDGES);
        } else if (vertexPtr->adjacentEdgeCount() == 1) {
            status.incrementInconsistency(VERTICES_WITH_ONE_EDGE);
        } else {
            // Check for duplicate edges.
            typedef std::vector<EdgePtr> EdgePtrVector;
            EdgePtrVector edgePtrVector;
            for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
                if (std::find(edgePtrVector.begin(), edgePtrVector.end(),
                        vertexPtr->adjacentEdge(index)) != edgePtrVector.end()) {
                    status.incrementInconsistency(VERTICES_WITH_DUPLICATE_EDGES);
                    break;
                }
                edgePtrVector.push_back(vertexPtr->adjacentEdge(index));
            }
        }

        // The vertex must have at least one adjacent face.
        if (vertexPtr->adjacentFaceCount() == 0) {
            status.incrementInconsistency(VERTICES_WITHOUT_FACES);
        } else {
            // Check for duplicate faces.
            typedef std::vector<FacePtr> FacePtrVector;
            FacePtrVector facePtrVector;
            for (int index = 0; index < vertexPtr->adjacentFaceCount(); ++index) {
                if (std::find(facePtrVector.begin(), facePtrVector.end(),
                        vertexPtr->adjacentFace(index)) != facePtrVector.end()) {
                    status.incrementInconsistency(VERTICES_WITH_DUPLICATE_FACES);
                    break;
                }
                facePtrVector.push_back(vertexPtr->adjacentFace(index));
            }
        }
    }
}

static void 
TestMeshEdgeConsistency(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (EdgePtr edgePtr = meshPtr->edgeBegin(); edgePtr != meshPtr->edgeEnd(); ++edgePtr) {

        // The edge must have two adjacent vertices.
        if (edgePtr->adjacentVertexCount() != 2) {
            status.incrementInconsistency(EDGES_WITHOUT_TWO_VERTICES);
        } else {
            // The two vertices must not be the same.
            assert(edgePtr->adjacentVertexCount() == 2);
            if (edgePtr->adjacentVertex(0) == edgePtr->adjacentVertex(1)) {
                status.incrementInconsistency(EDGES_WITH_DUPLICATE_VERTICES);
            }
        }

        // The edge must have either one or two adjacent faces.
        if (edgePtr->adjacentFaceCount() == 0) {
            status.incrementInconsistency(EDGES_WITHOUT_FACES);
        }

        // If the edge has two adjacent faces, they must not be the same.
        if (edgePtr->adjacentFaceCount() == 2
            && edgePtr->adjacentFace(0) == edgePtr->adjacentFace(1)) {
            status.incrementInconsistency(EDGES_WITH_DUPLICATE_FACES);
        }
    }
}

static void 
TestMeshFaceConsistency(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (FacePtr facePtr = meshPtr->faceBegin(); facePtr != meshPtr->faceEnd(); ++facePtr) {
        // Every face must have three vertices.
        if (facePtr->adjacentVertexCount() != 3) {
            status.incrementInconsistency(FACES_WITHOUT_THREE_VERTICES);
        } else {
            // The three vertices must be different.
            if (facePtr->adjacentVertex(0) == facePtr->adjacentVertex(1)
                || facePtr->adjacentVertex(1) == facePtr->adjacentVertex(2)
                || facePtr->adjacentVertex(2) == facePtr->adjacentVertex(0)) {
                status.incrementInconsistency(FACES_WITH_DUPLICATE_VERTICES);
            }
        }

        // Every face must have three edges.
        if (facePtr->adjacentEdgeCount() != 3) {
            status.incrementInconsistency(FACES_WITHOUT_THREE_EDGES);
        } else {
            // The three edges must be different.
            if (facePtr->adjacentEdge(0) == facePtr->adjacentEdge(1)
                || facePtr->adjacentEdge(1) == facePtr->adjacentEdge(2)
                || facePtr->adjacentEdge(2) == facePtr->adjacentEdge(0)) {
                status.incrementInconsistency(FACES_WITH_DUPLICATE_EDGES);
            }
        }
    }
}

static void 
TestCounterclockwiseMeshFaces(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (FacePtr facePtr = meshPtr->faceBegin(); facePtr != meshPtr->faceEnd(); ++facePtr) {
        // The three vertices of the face must be specified in
        // counterclockwise order.
        if (facePtr->adjacentVertexCount() != 3) {
            continue;
        }
        float orientation = exact::TestOrientation2d(
            facePtr->adjacentVertex(0)->position(),
            facePtr->adjacentVertex(1)->position(),
            facePtr->adjacentVertex(2)->position());
        if (orientation == 0.0) {
            status.incrementInconsistency(FACES_WITH_ZERO_AREA);
        }
        if (orientation < 0.0) {
            status.incrementInconsistency(FACES_WITH_CLOCKWISE_VERTICES);
        }
    }
}

static void 
TestMeshVertexEdgeBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (VertexPtr vertexPtr = meshPtr->vertexBegin(); 
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
        for (int edgeIndex = 0; edgeIndex < vertexPtr->adjacentEdgeCount(); ++edgeIndex) {
            EdgePtr edgePtr = vertexPtr->adjacentEdge(edgeIndex);
            bool found = false;
            for (int vertexIndex = 0; vertexIndex < edgePtr->adjacentVertexCount(); 
                 ++vertexIndex) {
                if (edgePtr->adjacentVertex(vertexIndex) == vertexPtr) {
                    found = true;
                }
            }
            if (!found) {
                status.incrementInconsistency(VERTEX_EDGES_WITHOUT_BACKPOINTER);
            }
        }
    }
}

static void 
TestMeshVertexFaceBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (VertexPtr vertexPtr = meshPtr->vertexBegin(); 
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
        for (int faceIndex = 0; faceIndex < vertexPtr->adjacentFaceCount(); ++faceIndex) {
            FacePtr facePtr = vertexPtr->adjacentFace(faceIndex);
            bool found = false;
            for (int vertexIndex = 0; vertexIndex < facePtr->adjacentVertexCount(); 
                 ++vertexIndex) {
                if (facePtr->adjacentVertex(vertexIndex) == vertexPtr) {
                    found = true;
                }
            }
            if (!found) {
                status.incrementInconsistency(VERTEX_FACES_WITHOUT_BACKPOINTER);
            }
        }
    }
}

static void 
TestMeshEdgeVertexBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (EdgePtr edgePtr = meshPtr->edgeBegin(); edgePtr != meshPtr->edgeEnd(); ++edgePtr) {
        for (int vertexIndex = 0; vertexIndex < edgePtr->adjacentVertexCount(); ++vertexIndex) {
            VertexPtr vertexPtr = edgePtr->adjacentVertex(vertexIndex);
            bool found = false;
            for (int edgeIndex = 0; edgeIndex < vertexPtr->adjacentEdgeCount(); ++edgeIndex) {
                if (vertexPtr->adjacentEdge(edgeIndex) == edgePtr) {
                    found = true;
                }
            }
            if (!found) {
                status.incrementInconsistency(EDGE_VERTICES_WITHOUT_BACKPOINTER);
            }
        }
    }
}

static void 
TestMeshEdgeFaceBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (EdgePtr edgePtr = meshPtr->edgeBegin(); edgePtr != meshPtr->edgeEnd(); ++edgePtr) {
        for (int faceIndex = 0; faceIndex < edgePtr->adjacentFaceCount(); ++faceIndex) {
            FacePtr facePtr = edgePtr->adjacentFace(faceIndex);
            bool found = false;
            for (int edgeIndex = 0; edgeIndex < facePtr->adjacentEdgeCount(); ++edgeIndex) {
                if (facePtr->adjacentEdge(edgeIndex) == edgePtr) {
                    found = true;
                }
            }
            if (!found) {
                status.incrementInconsistency(EDGE_FACES_WITHOUT_BACKPOINTER);
            }
        }
    }
}

static void 
TestMeshFaceVertexBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (FacePtr facePtr = meshPtr->faceBegin(); facePtr != meshPtr->faceEnd(); ++facePtr) {
        for (int vertexIndex = 0; vertexIndex < facePtr->adjacentVertexCount(); ++vertexIndex) {
            VertexPtr vertexPtr = facePtr->adjacentVertex(vertexIndex);
            bool found = false;
            for (int faceIndex = 0; faceIndex < vertexPtr->adjacentFaceCount(); ++faceIndex) {
                if (vertexPtr->adjacentFace(faceIndex) == facePtr) {
                    found = true;
                }
            }
            if (!found) {
                status.incrementInconsistency(FACE_VERTICES_WITHOUT_BACKPOINTER);
            }
        }
    }
}

static void 
TestMeshFaceEdgeBackpointers(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (FacePtr facePtr = meshPtr->faceBegin(); facePtr != meshPtr->faceEnd(); ++facePtr) {
        for (int edgeIndex = 0; edgeIndex < facePtr->adjacentEdgeCount(); ++edgeIndex) {
            EdgePtr edgePtr = facePtr->adjacentEdge(edgeIndex);
            bool found = false;
            for (int faceIndex = 0; faceIndex < edgePtr->adjacentFaceCount(); ++faceIndex) {
                if (edgePtr->adjacentFace(faceIndex) == facePtr) {
                    found = true;
                }
            }
            if (!found) {
                status.incrementInconsistency(FACE_EDGES_WITHOUT_BACKPOINTER);
            }
        }
    }
}

static void 
TestVertexEdgeOrderOfFaces(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (FacePtr facePtr = meshPtr->faceBegin(); facePtr != meshPtr->faceEnd(); ++facePtr) {
        if (facePtr->adjacentVertexCount() != 3
            || facePtr->adjacentEdgeCount() != 3) {
            continue;
        }
        for (int edgeIndex = 0; edgeIndex < facePtr->adjacentEdgeCount(); ++edgeIndex) {
            EdgePtr edgePtr = facePtr->adjacentEdge(edgeIndex);
            if (edgePtr->adjacentVertexCount() == 2) {
                VertexPtr expectedVertex1 = facePtr->adjacentVertex(edgeIndex);
                VertexPtr expectedVertex2 = facePtr->adjacentVertex((edgeIndex + 1) % 3);
                if (!EdgeHasAdjacentVertices(edgePtr, expectedVertex1, expectedVertex2)) {
                    status.incrementInconsistency(FACES_WITH_MISMATCHED_VERTEX_EDGE_ORDER);
                }
            }
        }
    }
}

static void 
TestEdgeFaceOrientation(MeshPtr meshPtr, MeshConsistencyStatus &status)
{
    for (EdgePtr edgePtr = meshPtr->edgeBegin();
         edgePtr != meshPtr->edgeEnd(); ++edgePtr) {
        if (edgePtr->adjacentFaceCount() == 2) {
            VertexPtr vertexPtrArray[2];
            assert(edgePtr->adjacentVertexCount() == 2);
            for (int vertexIndex = 0; vertexIndex < 2; ++vertexIndex) {
                FacePtr facePtr = edgePtr->adjacentFace(vertexIndex);
                bool found = false;
                for (int edgeIndex = 0; edgeIndex < facePtr->adjacentVertexCount(); ++edgeIndex) {
                    VertexPtr vertexPtr = facePtr->adjacentVertex(edgeIndex);
                    if (!EdgeHasAdjacentVertex(edgePtr, vertexPtr)) {
                        vertexPtrArray[vertexIndex] = vertexPtr;
                        found = true;
                        break;
                    }
                }
                assert(found);
            }
            float orientation0 = exact::TestOrientation2d(
                edgePtr->adjacentVertex(0)->position(),
                edgePtr->adjacentVertex(1)->position(),
                vertexPtrArray[0]->position());
            float orientation1 = exact::TestOrientation2d(
                edgePtr->adjacentVertex(0)->position(),
                edgePtr->adjacentVertex(1)->position(),
                vertexPtrArray[1]->position());
            if ((orientation0 > 0.0
                    && orientation1 > 0.0)
                || (orientation0 < 0.0
                    && orientation1 < 0.0)) {
                status.incrementInconsistency(EDGES_WITH_FACES_ON_SAME_SIDE);
            }
        }
    }
}

void 
AssertMeshConsistency(MeshPtr
// Ifdef'ing this out avoids a compiler warning about an unused parameter.
#ifdef DEBUG
    meshPtr
#endif
    )
{
#ifdef DEBUG
    MeshConsistencyStatus status = TestMeshConsistency(meshPtr);

    // Each of these inconsistency checks is performed individually
    // so that when one of them fails, it'll be clear from the assertion
    // failure message which one is the problem.
    assert(status.inconsistencyCount(NAN_VERTICES) == 0);
    assert(status.inconsistencyCount(INFINITY_VERTICES) == 0);
    assert(status.inconsistencyCount(DUPLICATE_VERTICES) == 0);
    assert(status.inconsistencyCount(DUPLICATE_EDGES) == 0);
    assert(status.inconsistencyCount(DUPLICATE_FACES) == 0);
    assert(status.inconsistencyCount(VERTICES_WITH_ZERO_EDGES) == 0);
    assert(status.inconsistencyCount(VERTICES_WITH_ONE_EDGE) == 0);
    assert(status.inconsistencyCount(VERTICES_WITH_DUPLICATE_EDGES) == 0);
    assert(status.inconsistencyCount(VERTICES_WITHOUT_FACES) == 0);
    assert(status.inconsistencyCount(VERTICES_WITH_DUPLICATE_FACES) == 0);
    assert(status.inconsistencyCount(EDGES_WITHOUT_TWO_VERTICES) == 0);
    assert(status.inconsistencyCount(EDGES_WITH_DUPLICATE_VERTICES ) == 0);
    assert(status.inconsistencyCount(EDGES_WITHOUT_FACES) == 0);
    assert(status.inconsistencyCount(EDGES_WITH_DUPLICATE_FACES) == 0);
    assert(status.inconsistencyCount(FACES_WITHOUT_THREE_VERTICES) == 0);
    assert(status.inconsistencyCount(FACES_WITH_DUPLICATE_VERTICES) == 0);
    assert(status.inconsistencyCount(FACES_WITHOUT_THREE_EDGES) == 0);
    assert(status.inconsistencyCount(FACES_WITH_DUPLICATE_EDGES) == 0);
    assert(status.inconsistencyCount(FACES_WITH_ZERO_AREA) == 0);
    assert(status.inconsistencyCount(FACES_WITH_CLOCKWISE_VERTICES) == 0);
    assert(status.inconsistencyCount(EDGE_FACES_WITHOUT_BACKPOINTER) == 0);
    assert(status.inconsistencyCount(EDGE_VERTICES_WITHOUT_BACKPOINTER) == 0);
    assert(status.inconsistencyCount(FACE_EDGES_WITHOUT_BACKPOINTER) == 0);
    assert(status.inconsistencyCount(FACE_VERTICES_WITHOUT_BACKPOINTER) == 0);
    assert(status.inconsistencyCount(FACES_WITH_MISMATCHED_VERTEX_EDGE_ORDER) == 0);
    assert(status.inconsistencyCount(EDGES_WITH_FACES_ON_SAME_SIDE) == 0);

    // If this fails, it means that a new value was added to the MeshInconsistencyType
    // enum, but was not added to the sequence of assert calls above.
    assert(status.isConsistent());
#endif
}

} // namespace delaunay
