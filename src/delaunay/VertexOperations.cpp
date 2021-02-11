// Copyright 2008 Drew Olbrich

#include "VertexOperations.h"

#include <cassert>

#include <exact/GeometricPredicates.h>

#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "FaceOperations.h"
#include "MeshConvexPerimeter.h"

namespace delaunay {

// Given an edge on the perimeter of a mesh, return the vertex of the adjacent face
// that is not part of the edge.
VertexPtr GetVertexOppositePerimeterEdge(EdgePtr edgePtr);

void
DeleteVertexAndAllOrphanedAdjacentMeshElements(VertexPtr vertexPtr, MeshPtr meshPtr)
{
    // We repeatedly delete the first adjacent face, rather than looping
    // over the array of adjacent faces, because the array will change
    // as each adjacent face is deleted.
    while (vertexPtr->adjacentFaceCount() > 0) {
        DeleteFaceAndAllOrphanedAdjacentMeshElements(vertexPtr->adjacentFace(0), meshPtr);
    }
}

VertexPtr
GetAdjacentVertexAroundPerimeter(VertexPtr vertexPtr, Direction direction)
{
    // Find the two adjacent vertices that lie on the perimeter of the mesh.
    int adjacentPerimeterVertexCount = 0;
    VertexPtr adjacentPerimeterVertexArray[2];
    EdgePtr adjacentPerimeterEdgeArray[2];
    for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = vertexPtr->adjacentEdge(index);
        if (edgePtr->adjacentFaceCount() == 1) {
            assert(adjacentPerimeterVertexCount < 2);
            adjacentPerimeterVertexArray[adjacentPerimeterVertexCount] 
                = GetNeighboringVertexAlongEdge(vertexPtr, edgePtr);
            adjacentPerimeterEdgeArray[adjacentPerimeterVertexCount] = edgePtr;
            ++adjacentPerimeterVertexCount;
        }
    }

    assert(adjacentPerimeterVertexCount == 2);

    VertexPtr oppositeVertexPtr0 = GetVertexOppositePerimeterEdge(adjacentPerimeterEdgeArray[0]);
    float orientation0 = exact::TestOrientation2d(
        vertexPtr->position(),
        adjacentPerimeterVertexArray[0]->position(),
        oppositeVertexPtr0->position());
    assert(orientation0 != 0.0);

#ifdef DEBUG
    VertexPtr oppositeVertexPtr1 = GetVertexOppositePerimeterEdge(adjacentPerimeterEdgeArray[1]);
    float orientation1 = exact::TestOrientation2d(
        adjacentPerimeterVertexArray[1]->position(),
        vertexPtr->position(),
        oppositeVertexPtr1->position());
    assert(orientation1 != 0.0);
    // The two opposite vertices must be on the same side of the mesh perimeter.
    assert((orientation0 > 0.0 && orientation1 > 0.0)
        || (orientation0 < 0.0 && orientation1 < 0.0));
#endif

    if (orientation0 > 0.0) {
        // Points passed to TestOrientation2d were specified in counterclockwise order.
        switch (direction) {
        case COUNTERCLOCKWISE:
            return adjacentPerimeterVertexArray[0];
            break;
        case CLOCKWISE:
            return adjacentPerimeterVertexArray[1];
            break;
        }
    } else {
        // Points passed to TestOrientation2d were specified in clockwise order.
        switch (direction) {
        case COUNTERCLOCKWISE:
            return adjacentPerimeterVertexArray[1];
            break;
        case CLOCKWISE:
            return adjacentPerimeterVertexArray[0];
            break;
        }
    }

    // This should never happen.
    abort();
}

bool
VertexHasVertexAsNeighbor(VertexPtr vertexPtr, VertexPtr neighboringVertexPtr)
{
    for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = vertexPtr->adjacentEdge(index);
        assert(edgePtr->adjacentVertexCount() == 2);
        if (edgePtr->adjacentVertex(0) == neighboringVertexPtr
            || edgePtr->adjacentVertex(1) == neighboringVertexPtr) {
            return true;
        }
    }

    return false;
}

EdgePtr
GetEdgeLeadingToNeighboringVertex(VertexPtr vertexPtr, VertexPtr neighboringVertexPtr,
    MeshPtr meshPtr)
{
    for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
        EdgePtr edgePtr = vertexPtr->adjacentEdge(index);
        assert(edgePtr->adjacentVertexCount() == 2);
        if (edgePtr->adjacentVertex(0) == neighboringVertexPtr
            || edgePtr->adjacentVertex(1) == neighboringVertexPtr) {
            return edgePtr;
        }
    }

    return meshPtr->edgeEnd();
}

VertexPtr
GetNeighboringVertexAlongEdge(VertexPtr vertexPtr, EdgePtr adjacentEdgePtr)
{
    assert(adjacentEdgePtr->adjacentVertexCount() == 2);
    if (adjacentEdgePtr->adjacentVertex(0) == vertexPtr) {
        return adjacentEdgePtr->adjacentVertex(1);
    } else {
        assert(adjacentEdgePtr->adjacentVertex(1) == vertexPtr);
        return adjacentEdgePtr->adjacentVertex(0);
    }
}

VertexPtr 
GetVertexOppositePerimeterEdge(EdgePtr edgePtr)
{
    assert(edgePtr->adjacentFaceCount() == 1);
    FacePtr facePtr = edgePtr->adjacentFace(0);

    return GetVertexOppositeEdge(facePtr, edgePtr);
}

} // namespace delaunay
