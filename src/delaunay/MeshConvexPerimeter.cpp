// Copyright 2008 Drew Olbrich

#include "MeshConvexPerimeter.h"

#include <iostream>
#include <limits>

#include <exact/GeometricPredicates.h>

#include "MeshConnected.h"
#include "MeshEulerCharacteristic.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Edge.h"
#include "VertexOperations.h"

namespace delaunay {

// Returns the leftmost vertex of a mesh.
static VertexPtr GetLeftmostVertexPtr(MeshPtr meshPtr);

// Returns true if the angle formed by the specified sequence of counterclockwise vertices
// around the mesh perimeter is concave.
static bool VertexSequenceAngleIsConcave(VertexPtr v1, VertexPtr v2, VertexPtr v3);

bool
MeshHasConvexPerimeter(MeshPtr meshPtr)
{
    // Return false if the mesh is disjoint.
    if (!MeshIsSingleConnectedRegion(meshPtr)) {
        return false;
    }

    // Return false if the mesh has holes in it.
    if (GetMeshEulerCharacteristic(meshPtr) != 1) {
        return false;
    }

    // If the mesh has no vertices, then its perimeter is considered convex.
    if (meshPtr->vertexCount() <= 0) {
        return true;
    }

    // Find the leftmost vertex in the mesh.
    VertexPtr leftmostVertexPtr = GetLeftmostVertexPtr(meshPtr);

    // Loop all the way around the perimeter of the mesh to see if any
    // sequence of three vertices forms a concave angle.
    VertexPtr v1 = leftmostVertexPtr;
    VertexPtr v2 = GetAdjacentVertexAroundPerimeter(v1, COUNTERCLOCKWISE);
    VertexPtr v3 = GetAdjacentVertexAroundPerimeter(v2, COUNTERCLOCKWISE);
    do {
        if (VertexSequenceAngleIsConcave(v1, v2, v3)) {
            return false;
        }
        v1 = v2;
        v2 = v3;
        v3 = GetAdjacentVertexAroundPerimeter(v2, COUNTERCLOCKWISE);
    } while (v1 != leftmostVertexPtr);

    // We went all the way around the perimeter and didn't find any
    // concave angles, so the perimeter must be convex.
    return true;
}

static VertexPtr 
GetLeftmostVertexPtr(MeshPtr meshPtr)
{
    assert(meshPtr->vertexCount() > 0);

    VertexPtr vertexPtr = meshPtr->vertexBegin();
    VertexPtr leftmostVertexPtr = vertexPtr;
    for (; vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
        if (vertexPtr->position()[0] < leftmostVertexPtr->position()[0]
            || (vertexPtr->position()[0] == leftmostVertexPtr->position()[0]
                && vertexPtr->position()[1] < leftmostVertexPtr->position()[1])) {
            leftmostVertexPtr = vertexPtr;
        }
    }

    return leftmostVertexPtr;
}

static bool 
VertexSequenceAngleIsConcave(VertexPtr v1, VertexPtr v2, VertexPtr v3)
{
    return exact::TestOrientation2d(v1->position(), v2->position(), v3->position()) < 0.0;
}

} // namespace delaunay
