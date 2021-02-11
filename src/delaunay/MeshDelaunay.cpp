// Copyright 2008 Drew Olbrich

#include "MeshDelaunay.h"

#include <exact/GeometricPredicates.h>
#include <cgmath/Vector2f.h>
#include <cgmath/CircleOperations.h>

#include "Mesh.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

using cgmath::Vector2f;

namespace delaunay {

bool
MeshIsDelaunay(MeshPtr meshPtr,
    cgmath::Vector2f *failedTestCircleCenter, float *failedTestCircleRadius)
{
    // This is an inefficient O(n^2) test.

    for (FacePtr facePtr = meshPtr->faceBegin(); facePtr != meshPtr->faceEnd(); ++facePtr) {
        assert(facePtr->adjacentVertexCount() == 3);
        VertexPtr v1 = facePtr->adjacentVertex(0);
        VertexPtr v2 = facePtr->adjacentVertex(1);
        VertexPtr v3 = facePtr->adjacentVertex(2);
        Vector2f p1 = v1->position();
        Vector2f p2 = v2->position();
        Vector2f p3 = v3->position();
        for (VertexPtr vertexPtr = meshPtr->vertexBegin(); 
             vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
            if (vertexPtr != v1
                && vertexPtr != v2
                && vertexPtr != v3) {
                if (exact::TestInCircle(p1, p2, p3, vertexPtr->position()) > 0.0) {
                    // The vertex lies inside, but does not intersect with the edges of,
                    // triangle p1, p2, p3.
                    if (failedTestCircleCenter != NULL
                        && failedTestCircleRadius != NULL) {
                        cgmath::CalculateCircleFromThreePoints(p1, p2, p3,
                            *failedTestCircleCenter, *failedTestCircleRadius);
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

} // namespace delaunay
