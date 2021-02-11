// Copyright 2008 Drew Olbrich

#include "FaceOperations.h"

#include <cassert>

#include <exact/GeometricPredicates.h>

#include "Mesh.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "VertexOperations.h"

namespace delaunay {

void
RotateFace(FacePtr facePtr)
{
    assert(facePtr->adjacentVertexCount() == 3);
    assert(facePtr->adjacentEdgeCount() == 3);

    VertexPtr v1 = facePtr->adjacentVertex(0);
    VertexPtr v2 = facePtr->adjacentVertex(1);
    VertexPtr v3 = facePtr->adjacentVertex(2);

    EdgePtr e1 = facePtr->adjacentEdge(0);
    EdgePtr e2 = facePtr->adjacentEdge(1);
    EdgePtr e3 = facePtr->adjacentEdge(2);

    facePtr->removeAdjacentVertex(v1);
    facePtr->removeAdjacentVertex(v2);
    facePtr->removeAdjacentVertex(v3);

    facePtr->removeAdjacentEdge(e1);
    facePtr->removeAdjacentEdge(e2);
    facePtr->removeAdjacentEdge(e3);

    facePtr->addAdjacentVertex(v3);
    facePtr->addAdjacentVertex(v1);
    facePtr->addAdjacentVertex(v2);

    facePtr->addAdjacentEdge(e3);
    facePtr->addAdjacentEdge(e1);
    facePtr->addAdjacentEdge(e2);
}

VertexPtr
SplitFace(FacePtr facePtr, const cgmath::Vector2f &newVertexPosition, MeshPtr meshPtr)
{
    // See the diagram in retro/src/delaunay/notes/Mesh-Operations.pdf.

    FacePtr f1 = facePtr;

    // The face must have three vertices.
    assert(f1->adjacentVertexCount() == 3);
    VertexPtr v1 = f1->adjacentVertex(0);
    VertexPtr v2 = f1->adjacentVertex(1);
    VertexPtr v3 = f1->adjacentVertex(2);

    // The face must have three edges.
    assert(f1->adjacentVertexCount() == 3);
    EdgePtr e1 = f1->adjacentEdge(0);
    EdgePtr e2 = f1->adjacentEdge(1);
    EdgePtr e3 = f1->adjacentEdge(2);

    // Assert that the specified new vertex is not coincident with any
    // of the face's existing vertices.
    assert(v1->position() != newVertexPosition);
    assert(v2->position() != newVertexPosition);
    assert(v3->position() != newVertexPosition);

    // The new vertex position must be inside the face.
    assert(exact::TestOrientation2d(v1->position(), v2->position(), newVertexPosition) > 0.0);
    assert(exact::TestOrientation2d(v2->position(), v3->position(), newVertexPosition) > 0.0);
    assert(exact::TestOrientation2d(v3->position(), v1->position(), newVertexPosition) > 0.0);

    VertexPtr v4 = meshPtr->createVertex();
    v4->setPosition(newVertexPosition);

    EdgePtr e4 = meshPtr->createEdge();
    EdgePtr e5 = meshPtr->createEdge();
    EdgePtr e6 = meshPtr->createEdge();

    FacePtr f2 = meshPtr->createFace();
    FacePtr f3 = meshPtr->createFace();

    f1->removeAdjacentVertex(v1);
    f1->removeAdjacentVertex(v2);
    f1->removeAdjacentVertex(v3);
    f1->removeAdjacentEdge(e1);
    f1->removeAdjacentEdge(e2);
    f1->removeAdjacentEdge(e3);

    e1->removeAdjacentFace(f1);
    e2->removeAdjacentFace(f1);
    e3->removeAdjacentFace(f1);

    v1->removeAdjacentEdge(e1);
    v1->removeAdjacentEdge(e3);

    v2->removeAdjacentEdge(e1);
    v2->removeAdjacentEdge(e2);

    v3->removeAdjacentEdge(e2);
    v3->removeAdjacentEdge(e3);

    v1->removeAdjacentFace(f1);
    v2->removeAdjacentFace(f1);
    v3->removeAdjacentFace(f1);

    f1->addAdjacentVertex(v1);
    f1->addAdjacentVertex(v2);
    f1->addAdjacentVertex(v4);

    f2->addAdjacentVertex(v2);
    f2->addAdjacentVertex(v3);
    f2->addAdjacentVertex(v4);

    f3->addAdjacentVertex(v3);
    f3->addAdjacentVertex(v1);
    f3->addAdjacentVertex(v4);

    f1->addAdjacentEdge(e1);
    f1->addAdjacentEdge(e5);
    f1->addAdjacentEdge(e4);

    f2->addAdjacentEdge(e2);
    f2->addAdjacentEdge(e6);
    f2->addAdjacentEdge(e5);

    f3->addAdjacentEdge(e3);
    f3->addAdjacentEdge(e4);
    f3->addAdjacentEdge(e6);

    e4->addAdjacentVertex(v1);
    e4->addAdjacentVertex(v4);

    e5->addAdjacentVertex(v2);
    e5->addAdjacentVertex(v4);

    e6->addAdjacentVertex(v3);
    e6->addAdjacentVertex(v4);

    e1->addAdjacentFace(f1);
    e2->addAdjacentFace(f2);
    e3->addAdjacentFace(f3);

    e4->addAdjacentFace(f3);
    e4->addAdjacentFace(f1);

    e5->addAdjacentFace(f1);
    e5->addAdjacentFace(f2);

    e6->addAdjacentFace(f2);
    e6->addAdjacentFace(f3);

    v1->addAdjacentEdge(e1);
    v1->addAdjacentEdge(e3);
    v1->addAdjacentEdge(e4);

    v2->addAdjacentEdge(e1);
    v2->addAdjacentEdge(e2);
    v2->addAdjacentEdge(e5);

    v3->addAdjacentEdge(e2);
    v3->addAdjacentEdge(e3);
    v3->addAdjacentEdge(e6);

    v4->addAdjacentEdge(e4);
    v4->addAdjacentEdge(e5);
    v4->addAdjacentEdge(e6);

    v1->addAdjacentFace(f1);
    v1->addAdjacentFace(f3);

    v2->addAdjacentFace(f1);
    v2->addAdjacentFace(f2);

    v3->addAdjacentFace(f2);
    v3->addAdjacentFace(f3);

    v4->addAdjacentFace(f1);
    v4->addAdjacentFace(f2);
    v4->addAdjacentFace(f3);

    return v4;
}    

void
DeleteFaceAndAllOrphanedAdjacentMeshElements(FacePtr facePtr, MeshPtr meshPtr)
{
    // Unwire the face from all adjacent edges. 
    for (int index = 0; index < facePtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = facePtr->adjacentEdge(index);
        edgePtr->removeAdjacentFace(facePtr);
    }

    // Unwire the face from all adjacent vertices. 
    for (int index = 0; index < facePtr->adjacentVertexCount(); ++index) {
        VertexPtr vertexPtr = facePtr->adjacentVertex(index);
        vertexPtr->removeAdjacentFace(facePtr);
    }

    // Delete all adjacent edges that have no adjacent faces.
    for (int index = 0; index < facePtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = facePtr->adjacentEdge(index);
        if (edgePtr->adjacentFaceCount() == 0) {
            for (int index = 0; index < edgePtr->adjacentVertexCount(); ++index) {
                VertexPtr vertexPtr = edgePtr->adjacentVertex(index);
                vertexPtr->removeAdjacentEdge(edgePtr);
            }
            for (int index = 0; index < edgePtr->adjacentFaceCount(); ++index) {
                FacePtr facePtr = edgePtr->adjacentFace(index);
                facePtr->removeAdjacentEdge(edgePtr);
            }
            meshPtr->destroyEdge(edgePtr);
        }
    }

    // Delete all adjacent vertices that have no adjacent edges. 
    for (int index = 0; index < facePtr->adjacentVertexCount(); ++index) {
        VertexPtr vertexPtr = facePtr->adjacentVertex(index);
        if (vertexPtr->adjacentEdgeCount() == 0) {
            for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
                EdgePtr edgePtr = vertexPtr->adjacentEdge(index);
                edgePtr->removeAdjacentVertex(vertexPtr);
            }
            for (int index = 0; index < vertexPtr->adjacentFaceCount(); ++index) {
                FacePtr facePtr = vertexPtr->adjacentFace(index);
                facePtr->removeAdjacentVertex(vertexPtr);
            }
            meshPtr->destroyVertex(vertexPtr);
        }
    }

    // Delete the face. 
    meshPtr->destroyFace(facePtr);
}

void
DeleteFaceAndAllOrphanedNonconstrainedAdjacentEdges(FacePtr facePtr, MeshPtr meshPtr)
{
    // Unwire the face from all adjacent edges. 
    for (int index = 0; index < facePtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = facePtr->adjacentEdge(index);
        edgePtr->removeAdjacentFace(facePtr);
    }

    // Unwire the face from all adjacent vertices. 
    for (int index = 0; index < facePtr->adjacentVertexCount(); ++index) {
        VertexPtr vertexPtr = facePtr->adjacentVertex(index);
        vertexPtr->removeAdjacentFace(facePtr);
    }

    // Delete all adjacent edges that have no adjacent faces.
    for (int index = 0; index < facePtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = facePtr->adjacentEdge(index);
        if (edgePtr->adjacentFaceCount() == 0
            && !edgePtr->inputEdgeIndexIsDefined()) {
            for (int index = 0; index < edgePtr->adjacentVertexCount(); ++index) {
                VertexPtr vertexPtr = edgePtr->adjacentVertex(index);
                vertexPtr->removeAdjacentEdge(edgePtr);
            }
            for (int index = 0; index < edgePtr->adjacentFaceCount(); ++index) {
                FacePtr facePtr = edgePtr->adjacentFace(index);
                facePtr->removeAdjacentEdge(edgePtr);
            }
            meshPtr->destroyEdge(edgePtr);
        }
    }

    // Delete the face. 
    meshPtr->destroyFace(facePtr);
}

FacePtr
CreateFaceAndAdjacentEdges(VertexPtr v1, VertexPtr v2, VertexPtr v3, MeshPtr meshPtr)
{
    // Assert that the vertices are listed in counterclockwise order
    // and are not colinear.
    assert(exact::TestOrientation2d(v1->position(), v2->position(), v3->position()) > 0.0);

    // Assert that the face doesn't already exist. Check that the first vertex
    // does not have an adjacent face whose adjacent vertices are
    // each of the three specified vertices.
#ifdef DEBUG
    for (int index = 0; index < v1->adjacentFaceCount(); ++index) {
        FacePtr facePtr = v1->adjacentFace(index);
        assert(facePtr->adjacentVertexCount() == 3);
        assert(!(facePtr->adjacentVertex(0) == v1
                && facePtr->adjacentVertex(1) == v2
                && facePtr->adjacentVertex(2) == v3));
        assert(!(facePtr->adjacentVertex(0) == v3
                && facePtr->adjacentVertex(1) == v1
                && facePtr->adjacentVertex(2) == v2));
        assert(!(facePtr->adjacentVertex(0) == v2
                && facePtr->adjacentVertex(1) == v3
                && facePtr->adjacentVertex(2) == v1));
    }
#endif

    // Create the new face.
    FacePtr newFacePtr = meshPtr->createFace();

    const VertexPtr vertexPtrArray[3] = { v1, v2, v3 };

    // Attach the vertices to the face and vice versa.
    for (int index = 0; index < 3; ++index) {
        newFacePtr->addAdjacentVertex(vertexPtrArray[index]);
        vertexPtrArray[index]->addAdjacentFace(newFacePtr);
    }

    // Create any of the edges that do not yet exist.
    EdgePtr edgePtrArray[3] = {
        meshPtr->edgeEnd(),
        meshPtr->edgeEnd(),
        meshPtr->edgeEnd()
    };
    for (int index = 0; index < 3; ++index) {
        VertexPtr vertexPtr = vertexPtrArray[index];
        VertexPtr nextVertexPtr = vertexPtrArray[(index + 1) % 3];

        // For each vertex, check all its edges and see if any of them contain
        // the next vertex in counterclockwise order.
        EdgePtr matchingEdgePtr = GetEdgeLeadingToNeighboringVertex(vertexPtr,
            nextVertexPtr, meshPtr);

        // Create the edge if we didn't find it above.
        if (matchingEdgePtr == meshPtr->edgeEnd()) {
            // The edge doesn't already exist. 
            EdgePtr newEdgePtr = meshPtr->createEdge();
            // Set the adjacent vertices for the new edge.
            newEdgePtr->addAdjacentVertex(vertexPtr);
            newEdgePtr->addAdjacentVertex(nextVertexPtr);
            // For the new edge, add the adjacent edge for the vertices.
            vertexPtr->addAdjacentEdge(newEdgePtr);
            nextVertexPtr->addAdjacentEdge(newEdgePtr);
            edgePtrArray[index] = newEdgePtr;
        } else {
            // The edge already exists.
            edgePtrArray[index] = matchingEdgePtr;
        }

        assert(edgePtrArray[index] != meshPtr->edgeEnd());
    }

    // Attach the edges to the face and vice versa.
    for (int index = 0; index < 3; ++index) {
        newFacePtr->addAdjacentEdge(edgePtrArray[index]);
        edgePtrArray[index]->addAdjacentFace(newFacePtr);
    }

    return newFacePtr;
}

FacePtr
GetNeighboringFaceAcrossEdge(FacePtr facePtr, EdgePtr adjacentEdgePtr)
{
    assert(adjacentEdgePtr->adjacentFaceCount() == 2);
    if (adjacentEdgePtr->adjacentFace(0) == facePtr) {
        return adjacentEdgePtr->adjacentFace(1);
    } else {
        assert(adjacentEdgePtr->adjacentFace(1) == facePtr);
        return adjacentEdgePtr->adjacentFace(0);
    }
}

VertexPtr
GetVertexOppositeEdge(FacePtr facePtr, EdgePtr edgePtr)
{
    assert(edgePtr->adjacentVertexCount() == 2);
    assert(facePtr->adjacentVertexCount() == 3);
    assert(facePtr->adjacentEdgeCount() == 3);
    for (int index = 0; index < facePtr->adjacentVertexCount(); ++index) {
        VertexPtr faceVertexPtr = facePtr->adjacentVertex(index);
        if (faceVertexPtr != edgePtr->adjacentVertex(0)
            && faceVertexPtr != edgePtr->adjacentVertex(1)) {
            return faceVertexPtr;
        }
    }

    // The edge was not found adjacent to the face.
    abort();
}

EdgePtr
GetEdgeOppositeVertex(FacePtr facePtr, VertexPtr vertexPtr)
{
    assert(facePtr->adjacentVertexCount() == 3);
    assert(facePtr->adjacentEdgeCount() == 3);
    for (int index = 0; index < facePtr->adjacentVertexCount(); ++index) {
        if (facePtr->adjacentVertex(index) == vertexPtr) {
            return facePtr->adjacentEdge((index + 1) % 3);
        }
    }

    // The vertex was not found adjacent to the face.
    abort();
}

} // namespace delaunay
