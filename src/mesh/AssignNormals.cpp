// Copyright 2008 Drew Olbrich

#include "AssignNormals.h"

#include <cmath>
#include <cassert>

#include <cgmath/Vector3f.h>
#include <cgmath/Constants.h>

#include "FaceOperations.h"
#include "StandardAttributes.h"
#include "Mesh.h"

using cgmath::Vector3f;

namespace mesh {

// Loop through the faces adjacent to the specified vertex,
// starting at the specified face, traversing in the direction
// of the specified edge, accumulating weighted vertex normals.
// If we loop back around to the original face, true is returned.
// If an edge is encounted with an angle between adjacent faces
// that is greater than the specified crease angle, false is returned.
static bool AccumulateWeightedFaceNormals(ConstVertexPtr vertexPtr,
    ConstFacePtr facePtr, ConstEdgePtr edgePtr, float creaseAngle, AttributeKey faceNormal3fKey, 
    AttributeKey faceAreaKey, Vector3f &vertexNormal);

// Return a face's normal, multiplied by its area.
static Vector3f GetWeightedFaceNormal(ConstFacePtr facePtr, AttributeKey faceNormal3fKey, 
    AttributeKey faceAreaKey);

void 
AssignNormals(Mesh *mesh, float creaseAngle)
{
    // This algorithm is loosely based on the March 1999 Journal of
    // Graphics Tools paper "Weights for Computing Vertex Normals from
    // Facet Normals", (Nelson Max, Lawrence Livermore National
    // Laboratory).
    
    // Convert the crease angle to radians.
    creaseAngle *= cgmath::PI/180.0;

    AttributeKey normal3fKey = GetNormal3fAttributeKey(*mesh);

    AttributeKey faceNormal3fKey = mesh->getAttributeKey("__AssignNormals_faceNormal3f", 
        AttributeKey::VECTOR3F);

    AttributeKey faceAreaKey = mesh->getAttributeKey("__AssignNormals_area", 
        AttributeKey::FLOAT);

    // Precompute the face normals and areas, because it's expensive.
    for (FacePtr facePtr = mesh->faceBegin(); facePtr != mesh->faceEnd(); ++facePtr) {
        facePtr->setVector3f(faceNormal3fKey, GetFaceGeometricNormal(facePtr));
        facePtr->setFloat(faceAreaKey, GetFaceArea(facePtr));
    }
    
    for (FacePtr parentFacePtr = mesh->faceBegin();
         parentFacePtr != mesh->faceEnd(); ++parentFacePtr) {

        // If the face is degenerate, skip it.
        if (parentFacePtr->adjacentVertexCount() < 3) {
            continue;
        }

        // Loop over all the vertices adjacent to this face.
        for (AdjacentVertexIterator iterator = parentFacePtr->adjacentVertexBegin();
             iterator != parentFacePtr->adjacentVertexEnd(); ++iterator) {
            ConstVertexPtr vertexPtr(*iterator);

            // Get the next vertex around the face.
            AdjacentVertexIterator nextIterator(iterator);
            ++nextIterator;
            if (nextIterator == parentFacePtr->adjacentVertexEnd()) {
                nextIterator = parentFacePtr->adjacentVertexBegin();
            }
            ConstVertexPtr nextVertexPtr(*nextIterator);

            // Find the two edges of the face that are adjacent
            // to the current vertex.
            ConstEdgePtr leftEdge = mesh->edgeEnd();
            ConstEdgePtr rightEdge = mesh->edgeEnd();
            for (AdjacentEdgeConstIterator iterator = vertexPtr->adjacentEdgeBegin();
                 iterator != vertexPtr->adjacentEdgeEnd(); ++iterator) {
                ConstEdgePtr edgePtr(*iterator);
                if (edgePtr->hasAdjacentFace(parentFacePtr)) {
                    if (edgePtr->hasAdjacentVertex(nextVertexPtr)) {
                        leftEdge = edgePtr;
                    } else {
                        rightEdge = edgePtr;
                    }
                }
            }

            // Start with the weighted face normal for 
            // the current face.
            Vector3f vertexNormal(GetWeightedFaceNormal(parentFacePtr,
                    faceNormal3fKey, faceAreaKey));

            if (leftEdge != mesh->edgeEnd()
                && rightEdge != mesh->edgeEnd()
                && leftEdge != rightEdge) {
                // Accumulate the weighted face normals counterclockwise
                // around the vertex.
                if (!AccumulateWeightedFaceNormals(vertexPtr, parentFacePtr,
                        leftEdge, creaseAngle, faceNormal3fKey, faceAreaKey, vertexNormal)) {
                    // If we didn't loop all the way back around to the
                    // original face, accumulate weighted face normals
                    // clockwise around the vertex.
                    AccumulateWeightedFaceNormals(vertexPtr, parentFacePtr,
                        rightEdge, creaseAngle, faceNormal3fKey, faceAreaKey, vertexNormal);
                }
            }

            vertexNormal.normalize();

            parentFacePtr->setVertexUnitVector3f(vertexPtr, normal3fKey, vertexNormal);
        }
    }

    // Remove the temporary face normal and area attributes.
    for (FacePtr facePtr = mesh->faceBegin();facePtr != mesh->faceEnd(); ++facePtr) {
        facePtr->eraseAttribute(faceNormal3fKey);
        facePtr->eraseAttribute(faceAreaKey);
    }
}

static bool 
AccumulateWeightedFaceNormals(ConstVertexPtr vertexPtr, ConstFacePtr facePtr, 
    ConstEdgePtr edgePtr, float creaseAngle, AttributeKey faceNormal3fKey, 
    AttributeKey faceAreaKey, Vector3f &vertexNormal)
{
    ConstFacePtr currentFacePtr = facePtr;
    ConstEdgePtr currentEdgePtr = edgePtr;

    Vector3f currentFaceNormal(currentFacePtr->getVector3f(faceNormal3fKey));

    // As we loop through the faces, we count backward
    // through the adjacent faces. If we run out of faces,
    // we stop looping. This deals with the case when
    // the vertex is surrounded by nonmanifold topology.
    int adjacentFaceCount = vertexPtr->adjacentFaceCount();

    int count = 0;
    
    // Loop through the faces adjacent to the specified vertex,
    // starting with the specified face, and traversing
    // in the direction of the specified edge.
    while (true) {

        ++count;

        // Step over the current edge and find the
        // next face around the vertex.
        bool foundNextFace = false;
        ConstFacePtr nextFacePtr;
        for (AdjacentFaceConstIterator iterator = currentEdgePtr->adjacentFaceBegin();
             iterator != currentEdgePtr->adjacentFaceEnd(); ++iterator) {
            ConstFacePtr adjacentFacePtr(*iterator);
            if (adjacentFacePtr != currentFacePtr) {
                nextFacePtr = adjacentFacePtr;
                foundNextFace = true;
                break;
            }
        }

        // If no adjacent face was found, we're done looping
        // this direction.
        if (!foundNextFace) {
            return false;
        }

        // Check if we looped all the way back around 
        // to the original face.
        if (nextFacePtr == facePtr) {
            return true;
        }
        
        // If the angle between the faces on either side of the edge
        // exceeds the crease angle, stop looping.
        Vector3f nextFaceNormal(nextFacePtr->getVector3f(faceNormal3fKey));
        float angle = acosf(currentFaceNormal.dot(nextFaceNormal));
        if (angle > creaseAngle) {
            return false;
        }

        currentFacePtr = nextFacePtr;
        currentFaceNormal = nextFaceNormal;

        vertexNormal += GetWeightedFaceNormal(currentFacePtr,
            faceNormal3fKey, faceAreaKey);

        // Find the next adjacent edge.
        bool foundNextEdge = false;
        EdgePtr nextEdgePtr;
        for (AdjacentEdgeConstIterator iterator = currentFacePtr->adjacentEdgeBegin();
             iterator != currentFacePtr->adjacentEdgeEnd(); ++iterator) {
            EdgePtr adjacentEdgePtr(*iterator);
            if (adjacentEdgePtr->hasAdjacentVertex(vertexPtr)
                && adjacentEdgePtr != currentEdgePtr) {
                nextEdgePtr = adjacentEdgePtr;
                foundNextEdge = true;
            }
        }

        // There must be an adjacent edge if the face
        // is not degenerate.
        assert(foundNextEdge);

        currentEdgePtr = nextEdgePtr;

        // If we looped through more than the number of
        // adjacent faces, we're probably stuck looping around
        // nonmanifold topology, so return.
        --adjacentFaceCount;
        if (adjacentFaceCount < 0) {
            return false;
        }
    }
}

static Vector3f
GetWeightedFaceNormal(ConstFacePtr facePtr, AttributeKey faceNormal3fKey, 
    AttributeKey faceAreaKey)
{
    return facePtr->getVector3f(faceNormal3fKey)*facePtr->getFloat(faceAreaKey);
}

} // namespace mesh
