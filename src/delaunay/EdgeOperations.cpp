// Copyright 2008 Retroactive Fiasco.

#include "EdgeOperations.h"

#include <cassert>
#include <iostream>

#include <exact/GeometricPredicates.h>

#include "Mesh.h"
#include "Edge.h"
#include "Face.h"
#include "Vertex.h"

namespace delaunay {

// Function to handle the special case of splitting an edge that only has
// one adjacent face.
static VertexPtr SplitEdgeWithOneAdjacentFace(EdgePtr edgePtr, 
    const cgmath::Vector2f &newVertexPosition, MeshPtr meshPtr);

void
SwapEdge(EdgePtr edgePtr)
{
    // See the diagram in retro/src/delaunay/notes/Mesh-Operations.pdf.

    EdgePtr e1 = edgePtr;

    assert(e1->adjacentFaceCount() == 2);
    FacePtr f1 = e1->adjacentFace(0);
    FacePtr f2 = e1->adjacentFace(1);

    assert(f1->adjacentVertexCount() == 3);
    assert(f1->adjacentEdgeCount() == 3);
    assert(f1->adjacentEdge(0) == e1
        || f1->adjacentEdge(1) == e1
        || f1->adjacentEdge(2) == e1);

    assert(f2->adjacentVertexCount() == 3);
    assert(f2->adjacentEdgeCount() == 3);
    assert(f2->adjacentEdge(0) == e1
        || f2->adjacentEdge(1) == e1
        || f2->adjacentEdge(2) == e1);

    EdgePtr e2;
    EdgePtr e3;
    if (f1->adjacentEdge(0) == e1) {
        e2 = f1->adjacentEdge(1);
        e3 = f1->adjacentEdge(2);
    } else if (f1->adjacentEdge(1) == e1) {
        e2 = f1->adjacentEdge(2);
        e3 = f1->adjacentEdge(0);
    } else if (f1->adjacentEdge(2) == e1) {
        e2 = f1->adjacentEdge(0);
        e3 = f1->adjacentEdge(1);
    }

    assert(e2->adjacentVertexCount() == 2);
    assert(e3->adjacentVertexCount() == 2);

    EdgePtr e4;
    EdgePtr e5;
    if (f2->adjacentEdge(0) == e1) {
        e4 = f2->adjacentEdge(1);
        e5 = f2->adjacentEdge(2);
    } else if (f2->adjacentEdge(1) == e1) {
        e4 = f2->adjacentEdge(2);
        e5 = f2->adjacentEdge(0);
    } else if (f2->adjacentEdge(2) == e1) {
        e4 = f2->adjacentEdge(0);
        e5 = f2->adjacentEdge(1);
    }

    assert(e4->adjacentVertexCount() == 2);
    assert(e5->adjacentVertexCount() == 2);

    VertexPtr v1;
    VertexPtr v2;
    if (e3->adjacentVertex(0) == e1->adjacentVertex(0)
        || e3->adjacentVertex(1) == e1->adjacentVertex(0)) {
        v1 = e1->adjacentVertex(0);
        v2 = e1->adjacentVertex(1);
    } else {
        v1 = e1->adjacentVertex(1);
        v2 = e1->adjacentVertex(0);
    }

    assert(e4->adjacentVertex(0) == v1
        || e4->adjacentVertex(1) == v1);
    assert(e2->adjacentVertex(0) == v2
        || e2->adjacentVertex(1) == v2);
    assert(e5->adjacentVertex(0) == v2
        || e5->adjacentVertex(1) == v2);

    VertexPtr v3;
    if ((f1->adjacentVertex(0) == v1
            && f1->adjacentVertex(1) == v2)
        || (f1->adjacentVertex(0) == v2
            && f1->adjacentVertex(1) == v1)) {
        v3 = f1->adjacentVertex(2);
    } else if ((f1->adjacentVertex(1) == v1
            && f1->adjacentVertex(2) == v2)
        || (f1->adjacentVertex(1) == v2
            && f1->adjacentVertex(2) == v1)) {
        v3 = f1->adjacentVertex(0);
    } else if ((f1->adjacentVertex(0) == v1
            && f1->adjacentVertex(2) == v2)
        || (f1->adjacentVertex(0) == v2
            && f1->adjacentVertex(2) == v1)) {
        v3 = f1->adjacentVertex(1);
    }

    VertexPtr v4;
    if ((f2->adjacentVertex(0) == v1
            && f2->adjacentVertex(1) == v2)
        || (f2->adjacentVertex(0) == v2
            && f2->adjacentVertex(1) == v1)) {
        v4 = f2->adjacentVertex(2);
    } else if ((f2->adjacentVertex(1) == v1
            && f2->adjacentVertex(2) == v2)
        || (f2->adjacentVertex(1) == v2
            && f2->adjacentVertex(2) == v1)) {
        v4 = f2->adjacentVertex(0);
    } else if ((f2->adjacentVertex(0) == v1
            && f2->adjacentVertex(2) == v2)
        || (f2->adjacentVertex(0) == v2
            && f2->adjacentVertex(2) == v1)) {
        v4 = f2->adjacentVertex(1);
    }

    assert(e2->adjacentVertex(0) == v3
        || e2->adjacentVertex(1) == v3);
    assert(e3->adjacentVertex(0) == v3
        || e3->adjacentVertex(1) == v3);
    assert(e4->adjacentVertex(0) == v4
        || e4->adjacentVertex(1) == v4);
    assert(e5->adjacentVertex(0) == v4
        || e5->adjacentVertex(1) == v4);

    // The faces we'll wind up with after the swap must not be inverted.
    assert(exact::TestOrientation2d(v4->position(), v2->position(), v3->position()) > 0.0);
    assert(exact::TestOrientation2d(v3->position(), v1->position(), v4->position()) > 0.0);

    // Set the new vertices for the swapped edge.
    e1->removeAdjacentVertex(v1);
    e1->removeAdjacentVertex(v2);
    e1->addAdjacentVertex(v3);
    e1->addAdjacentVertex(v4);

    // Set the new vertices for the faces.
    f1->removeAdjacentVertex(v2);
    f1->removeAdjacentVertex(v3);
    f1->removeAdjacentVertex(v1);
    f1->addAdjacentVertex(v3);
    f1->addAdjacentVertex(v1);
    f1->addAdjacentVertex(v4);

    f2->removeAdjacentVertex(v1);
    f2->removeAdjacentVertex(v4);
    f2->removeAdjacentVertex(v2);
    f2->addAdjacentVertex(v4);
    f2->addAdjacentVertex(v2);
    f2->addAdjacentVertex(v3);

    // e2 is v2 v3
    assert(e2->adjacentVertex(0) == v2
        || e2->adjacentVertex(1) == v2);
    assert(e2->adjacentVertex(0) == v3
        || e2->adjacentVertex(1) == v3);

    // e3 is v1 v3
    assert(e3->adjacentVertex(0) == v1
        || e3->adjacentVertex(1) == v1);
    assert(e3->adjacentVertex(0) == v3
        || e3->adjacentVertex(1) == v3);

    // e4 is v1 v4
    assert(e4->adjacentVertex(0) == v1
        || e4->adjacentVertex(1) == v1);
    assert(e4->adjacentVertex(0) == v4
        || e4->adjacentVertex(1) == v4);

    // e5 is v2 v4
    assert(e5->adjacentVertex(0) == v2
        || e5->adjacentVertex(1) == v2);
    assert(e5->adjacentVertex(0) == v4
        || e5->adjacentVertex(1) == v4);

    // f1 was (counterclockwise) v3 e3 v1 e1 v2 e2 (v3)
    // f2 was (counterclockwise) v4 e5 v2 e1 v1 e4 (v4)
    // f1 is now (counterclockwise) v4 e1 v3 e3 v1 e4 (v4)
    // f2 is now (counterclockwise) v3 e1 v4 e5 v2 e2 (v3)

    // Set the new edges for the faces.
    f1->removeAdjacentEdge(e1);
    f1->removeAdjacentEdge(e2);
    f1->removeAdjacentEdge(e3);
    f1->addAdjacentEdge(e3);
    f1->addAdjacentEdge(e4);
    f1->addAdjacentEdge(e1);

    f2->removeAdjacentEdge(e1);
    f2->removeAdjacentEdge(e4);
    f2->removeAdjacentEdge(e5);
    f2->addAdjacentEdge(e5);
    f2->addAdjacentEdge(e2);
    f2->addAdjacentEdge(e1);

    // Two of the five edges switch faces.
    e2->removeAdjacentFace(f1);
    e2->addAdjacentFace(f2);
    e4->removeAdjacentFace(f2);
    e4->addAdjacentFace(f1);

    v1->removeAdjacentEdge(e1);
    v2->removeAdjacentEdge(e1);
    v3->addAdjacentEdge(e1);
    v4->addAdjacentEdge(e1);

    v1->removeAdjacentFace(f2);
    v2->removeAdjacentFace(f1);
    v3->addAdjacentFace(f2);
    v4->addAdjacentFace(f1);
}

VertexPtr
SplitEdge(EdgePtr edgePtr, const cgmath::Vector2f &newVertexPosition, MeshPtr meshPtr)
{
    // See the diagram in retro/src/delaunay/notes/Mesh-Operations.pdf.

    EdgePtr e1 = edgePtr;

    if (e1->adjacentFaceCount() == 1) {
        return SplitEdgeWithOneAdjacentFace(edgePtr, newVertexPosition, meshPtr);
    }

    // Get pointers to the faces adjacent to the shared edge.

    assert(e1->adjacentFaceCount() == 2);
    FacePtr f1 = e1->adjacentFace(0);
    FacePtr f2 = e1->adjacentFace(1);

    // Get pointers to the edges and vertices shared by the adjacent triangles.

    VertexPtr v1;
    VertexPtr v2;
    VertexPtr v3;
    EdgePtr e2;
    EdgePtr e3;
    assert(f1->adjacentEdgeCount() == 3);
    assert(f1->adjacentVertexCount() == 3);
    if (f1->adjacentEdge(0) == e1) {
        e2 = f1->adjacentEdge(1);
        e3 = f1->adjacentEdge(2);
        v1 = f1->adjacentVertex(0);
        v2 = f1->adjacentVertex(1);
        v3 = f1->adjacentVertex(2);
    } else if (f1->adjacentEdge(1) == e1) {
        e2 = f1->adjacentEdge(2);
        e3 = f1->adjacentEdge(0);
        v1 = f1->adjacentVertex(1);
        v2 = f1->adjacentVertex(2);
        v3 = f1->adjacentVertex(0);
    } else {
        assert(f1->adjacentEdge(2) == e1);
        e2 = f1->adjacentEdge(0);
        e3 = f1->adjacentEdge(1);
        v1 = f1->adjacentVertex(2);
        v2 = f1->adjacentVertex(0);
        v3 = f1->adjacentVertex(1);
    }

    assert(e1->adjacentVertexCount() == 2);
    assert(e2->adjacentVertexCount() == 2);
    assert(e3->adjacentVertexCount() == 2);
    assert(e1->adjacentVertex(0) == v1
        || e1->adjacentVertex(1) == v1);
    assert(e1->adjacentVertex(0) == v2
        || e1->adjacentVertex(1) == v2);
    assert(e2->adjacentVertex(0) == v2
        || e2->adjacentVertex(1) == v2);
    assert(e2->adjacentVertex(0) == v3
        || e2->adjacentVertex(1) == v3);
    assert(e3->adjacentVertex(0) == v3
        || e3->adjacentVertex(1) == v3);
    assert(e3->adjacentVertex(0) == v1
        || e3->adjacentVertex(1) == v1);

    VertexPtr v4;
    EdgePtr e4;
    EdgePtr e5;
    assert(f2->adjacentEdgeCount() == 3);
    assert(f2->adjacentVertexCount() == 3);
    if (f2->adjacentEdge(0) == e1) {
        e4 = f2->adjacentEdge(1);
        e5 = f2->adjacentEdge(2);
        assert(f2->adjacentVertex(0) == v2);
        assert(f2->adjacentVertex(1) == v1);
        v4 = f2->adjacentVertex(2);
    } else if (f2->adjacentEdge(1) == e1) {
        e4 = f2->adjacentEdge(2);
        e5 = f2->adjacentEdge(0);
        assert(f2->adjacentVertex(1) == v2);
        assert(f2->adjacentVertex(2) == v1);
        v4 = f2->adjacentVertex(0);
    } else {
        assert(f2->adjacentEdge(2) == e1);
        e4 = f2->adjacentEdge(0);
        e5 = f2->adjacentEdge(1);
        assert(f2->adjacentVertex(2) == v2);
        assert(f2->adjacentVertex(0) == v1);
        v4 = f2->adjacentVertex(1);
    }

    // Assert that the specified new vertex is not coincident with any
    // of the existing vertices.
    assert(v1->position() != newVertexPosition);
    assert(v2->position() != newVertexPosition);
    assert(v3->position() != newVertexPosition);
    assert(v4->position() != newVertexPosition);

    // The new vertex position must be inside one of the two faces or on the
    // edge shared by the two faces.
    // Face 1:
    assert(exact::TestOrientation2d(v1->position(), v2->position(), newVertexPosition) >= 0.0);
    assert(exact::TestOrientation2d(v2->position(), v3->position(), newVertexPosition) > 0.0);
    assert(exact::TestOrientation2d(v3->position(), v1->position(), newVertexPosition) > 0.0);
    // Face 2:
    assert(exact::TestOrientation2d(v2->position(), v1->position(), newVertexPosition) >= 0.0);
    assert(exact::TestOrientation2d(v1->position(), v4->position(), newVertexPosition) > 0.0);
    assert(exact::TestOrientation2d(v4->position(), v2->position(), newVertexPosition) > 0.0);

    // Unzip the existing faces from the mesh.

    assert(f1->adjacentVertexCount() == 3);
    f1->removeAdjacentVertex(v1);
    f1->removeAdjacentVertex(v2);
    f1->removeAdjacentVertex(v3);
    assert(f1->adjacentVertexCount() == 0);

    assert(f2->adjacentVertexCount() == 3);
    f2->removeAdjacentVertex(v1);
    f2->removeAdjacentVertex(v2);
    f2->removeAdjacentVertex(v4);
    assert(f2->adjacentVertexCount() == 0);

    assert(f1->adjacentEdgeCount() == 3);
    f1->removeAdjacentEdge(e1);
    f1->removeAdjacentEdge(e2);
    f1->removeAdjacentEdge(e3);
    assert(f1->adjacentEdgeCount() == 0);

    assert(f2->adjacentEdgeCount() == 3);
    f2->removeAdjacentEdge(e1);
    f2->removeAdjacentEdge(e4);
    f2->removeAdjacentEdge(e5);
    assert(f2->adjacentEdgeCount() == 0);

    // Unzip the shared edge from the mesh.

    assert(e1->adjacentVertexCount() == 2);
    e1->removeAdjacentVertex(v1);
    e1->removeAdjacentVertex(v2);
    assert(e1->adjacentVertexCount() == 0);

    assert(e1->adjacentFaceCount() == 2);
    e1->removeAdjacentFace(f1);
    e1->removeAdjacentFace(f2);
    assert(e1->adjacentFaceCount() == 0);

    // Unzip the perimeter edges from the mesh.
    e2->removeAdjacentFace(f1);
    e3->removeAdjacentFace(f1);
    e4->removeAdjacentFace(f2);
    e5->removeAdjacentFace(f2);

    // Unzip the edges from the vertices.
    v1->removeAdjacentEdge(e1);
    v1->removeAdjacentEdge(e3);
    v1->removeAdjacentEdge(e4);
    v2->removeAdjacentEdge(e1);
    v2->removeAdjacentEdge(e2);
    v2->removeAdjacentEdge(e5);
    v3->removeAdjacentEdge(e2);
    v3->removeAdjacentEdge(e3);
    v4->removeAdjacentEdge(e4);
    v4->removeAdjacentEdge(e5);

    // Unzip the faces from the vertices.
    v1->removeAdjacentFace(f1);
    v1->removeAdjacentFace(f2);
    v2->removeAdjacentFace(f1);
    v2->removeAdjacentFace(f2);
    v3->removeAdjacentFace(f1);
    v4->removeAdjacentFace(f2);
    
    // Create the new mesh elements.

    FacePtr f3 = meshPtr->createFace();
    FacePtr f4 = meshPtr->createFace();

    EdgePtr e6 = meshPtr->createEdge();
    EdgePtr e7 = meshPtr->createEdge();
    EdgePtr e8 = meshPtr->createEdge();

    VertexPtr v5 = meshPtr->createVertex();
    v5->setPosition(newVertexPosition);

    // Wire up all the mesh elements.

    f1->addAdjacentVertex(v3);
    f1->addAdjacentVertex(v1);
    f1->addAdjacentVertex(v5);
    f1->addAdjacentEdge(e3);
    f1->addAdjacentEdge(e1);
    f1->addAdjacentEdge(e7);

    f2->addAdjacentVertex(v1);
    f2->addAdjacentVertex(v4);
    f2->addAdjacentVertex(v5);
    f2->addAdjacentEdge(e4);
    f2->addAdjacentEdge(e8);
    f2->addAdjacentEdge(e1);

    f3->addAdjacentVertex(v2);
    f3->addAdjacentVertex(v3);
    f3->addAdjacentVertex(v5);
    f3->addAdjacentEdge(e2);
    f3->addAdjacentEdge(e7);
    f3->addAdjacentEdge(e6);

    f4->addAdjacentVertex(v4);
    f4->addAdjacentVertex(v2);
    f4->addAdjacentVertex(v5);
    f4->addAdjacentEdge(e5);
    f4->addAdjacentEdge(e6);
    f4->addAdjacentEdge(e8);

    e2->addAdjacentFace(f3);
    e3->addAdjacentFace(f1);
    e4->addAdjacentFace(f2);
    e5->addAdjacentFace(f4);

    e1->addAdjacentVertex(v1);
    e1->addAdjacentVertex(v5);
    e1->addAdjacentFace(f1);
    e1->addAdjacentFace(f2);

    e6->addAdjacentVertex(v2);
    e6->addAdjacentVertex(v5);
    e6->addAdjacentFace(f3);
    e6->addAdjacentFace(f4);

    e7->addAdjacentVertex(v3);
    e7->addAdjacentVertex(v5);
    e7->addAdjacentFace(f1);
    e7->addAdjacentFace(f3);

    e8->addAdjacentVertex(v4);
    e8->addAdjacentVertex(v5);
    e8->addAdjacentFace(f2);
    e8->addAdjacentFace(f4);

    v1->addAdjacentEdge(e1);
    v1->addAdjacentEdge(e3);
    v1->addAdjacentEdge(e4);
    v1->addAdjacentFace(f1);
    v1->addAdjacentFace(f2);

    v2->addAdjacentEdge(e2);
    v2->addAdjacentEdge(e5);
    v2->addAdjacentEdge(e6);
    v2->addAdjacentFace(f3);
    v2->addAdjacentFace(f4);

    v3->addAdjacentEdge(e2);
    v3->addAdjacentEdge(e3);
    v3->addAdjacentEdge(e7);
    v3->addAdjacentFace(f1);
    v3->addAdjacentFace(f3);

    v4->addAdjacentEdge(e4);
    v4->addAdjacentEdge(e5);
    v4->addAdjacentEdge(e8);
    v4->addAdjacentFace(f2);
    v4->addAdjacentFace(f4);

    v5->addAdjacentEdge(e1);
    v5->addAdjacentEdge(e6);
    v5->addAdjacentEdge(e7);
    v5->addAdjacentEdge(e8);
    v5->addAdjacentFace(f1);
    v5->addAdjacentFace(f2);
    v5->addAdjacentFace(f3);
    v5->addAdjacentFace(f4);

    return v5;
}

bool
EdgeHasAdjacentVertex(EdgePtr edgePtr, VertexPtr vertexPtr)
{
    return edgePtr->adjacentVertex(0) == vertexPtr
        || edgePtr->adjacentVertex(1) == vertexPtr;
}

bool
EdgeHasAdjacentVertices(EdgePtr edgePtr, VertexPtr v1, VertexPtr v2)
{
    return (edgePtr->adjacentVertex(0) == v1
        && edgePtr->adjacentVertex(1) == v2)
        || (edgePtr->adjacentVertex(0) == v2
            && edgePtr->adjacentVertex(1) == v1);
}

static VertexPtr 
SplitEdgeWithOneAdjacentFace(EdgePtr edgePtr, 
    const cgmath::Vector2f &newVertexPosition, MeshPtr meshPtr)
{
    EdgePtr e1 = edgePtr;

    assert(e1->adjacentFaceCount() == 1);
    FacePtr f1 = e1->adjacentFace(0);

    assert(f1->adjacentVertexCount() == 3);
    assert(f1->adjacentEdgeCount() == 3);

    VertexPtr v1;
    VertexPtr v2;
    VertexPtr v3;
    EdgePtr e2;
    EdgePtr e3;

    if (f1->adjacentEdge(0) == e1) {
        e2 = f1->adjacentEdge(1);
        e3 = f1->adjacentEdge(2);
        v1 = f1->adjacentVertex(0);
        v2 = f1->adjacentVertex(1);
        v3 = f1->adjacentVertex(2);
    } else if (f1->adjacentEdge(1) == e1) {
        e2 = f1->adjacentEdge(2);
        e3 = f1->adjacentEdge(0);
        v1 = f1->adjacentVertex(1);
        v2 = f1->adjacentVertex(2);
        v3 = f1->adjacentVertex(0);
    } else {
        assert(f1->adjacentEdge(2) == e1);
        e2 = f1->adjacentEdge(0);
        e3 = f1->adjacentEdge(1);
        v1 = f1->adjacentVertex(2);
        v2 = f1->adjacentVertex(0);
        v3 = f1->adjacentVertex(1);
    }

    // Assert that the specified new vertex is not coincident with any
    // of the existing vertices.
    assert(v1->position() != newVertexPosition);
    assert(v2->position() != newVertexPosition);
    assert(v3->position() != newVertexPosition);

    // The position of the new vertex must be such that the two newly created
    // faces will not be inverted.
    assert(exact::TestOrientation2d(v2->position(), v3->position(), newVertexPosition) > 0.0);
    assert(exact::TestOrientation2d(v3->position(), v1->position(), newVertexPosition) > 0.0);

    // Unzip the existing mesh elements.

    f1->removeAdjacentVertex(v1);
    f1->removeAdjacentVertex(v2);
    f1->removeAdjacentVertex(v3);
    f1->removeAdjacentEdge(e1);
    f1->removeAdjacentEdge(e2);
    f1->removeAdjacentEdge(e3);

    e1->removeAdjacentFace(f1);
    e2->removeAdjacentFace(f1);
    e3->removeAdjacentFace(f1);

    e1->removeAdjacentVertex(v1);
    e1->removeAdjacentVertex(v2);

    v1->removeAdjacentEdge(e1);
    v1->removeAdjacentEdge(e3);

    v2->removeAdjacentEdge(e1);
    v2->removeAdjacentEdge(e2);

    v3->removeAdjacentEdge(e2);
    v3->removeAdjacentEdge(e3);

    v1->removeAdjacentFace(f1);
    v2->removeAdjacentFace(f1);
    v3->removeAdjacentFace(f1);

    // Create the new mesh elements and wire them up to the mesh.

    VertexPtr v5 = meshPtr->createVertex();
    v5->setPosition(newVertexPosition);

    EdgePtr e6 = meshPtr->createEdge();
    EdgePtr e7 = meshPtr->createEdge();
    FacePtr f3 = meshPtr->createFace();

    f1->addAdjacentVertex(v3);
    f1->addAdjacentVertex(v1);
    f1->addAdjacentVertex(v5);
    f1->addAdjacentEdge(e3);
    f1->addAdjacentEdge(e1);
    f1->addAdjacentEdge(e7);

    f3->addAdjacentVertex(v2);
    f3->addAdjacentVertex(v3);
    f3->addAdjacentVertex(v5);
    f3->addAdjacentEdge(e2);
    f3->addAdjacentEdge(e7);
    f3->addAdjacentEdge(e6);

    e1->addAdjacentFace(f1);
    e3->addAdjacentFace(f1);
    e7->addAdjacentFace(f1);

    e6->addAdjacentFace(f3);
    e2->addAdjacentFace(f3);
    e7->addAdjacentFace(f3);

    e1->addAdjacentVertex(v1);
    e1->addAdjacentVertex(v5);
    e6->addAdjacentVertex(v2);
    e6->addAdjacentVertex(v5);
    e7->addAdjacentVertex(v3);
    e7->addAdjacentVertex(v5);

    v1->addAdjacentEdge(e1);
    v1->addAdjacentEdge(e3);

    v2->addAdjacentEdge(e2);
    v2->addAdjacentEdge(e6);

    v3->addAdjacentEdge(e2);
    v3->addAdjacentEdge(e3);
    v3->addAdjacentEdge(e7);

    v5->addAdjacentEdge(e1);
    v5->addAdjacentEdge(e6);
    v5->addAdjacentEdge(e7);

    v1->addAdjacentFace(f1);
    v2->addAdjacentFace(f3);
    v3->addAdjacentFace(f1);
    v3->addAdjacentFace(f3);
    v5->addAdjacentFace(f1);
    v5->addAdjacentFace(f3);

    return v5;
}

} // namespace delaunay
