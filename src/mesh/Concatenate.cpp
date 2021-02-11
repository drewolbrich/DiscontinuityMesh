// Copyright 2009 Drew Olbrich

#include "Concatenate.h"

#include <map>
#include <iostream>
#include <cassert>

#include "Types.h"
#include "Mesh.h"
#include "IsConsistent.h"

namespace mesh {

typedef std::map<ConstVertexPtr, VertexPtr> ConstVertexPtrToVertexPtrMap;
typedef std::map<ConstEdgePtr, EdgePtr> ConstEdgePtrToEdgePtrMap;
typedef std::map<ConstFacePtr, FacePtr> ConstFacePtrToFacePtrMap;

// Copies attributes of an AttributePossessor between meshes,
// creating new AttributeKeys in the target mesh, if necessary.
static void CopyAttributesBetweenMeshes(Mesh *targetMesh, const Mesh &sourceMesh,
    AttributePossessor *targetAttributePossessor, 
    const AttributePossessor &sourceAttributePossessor);

void
Concatenate(Mesh *targetMesh, const Mesh &sourceMesh)
{
    // Copy the mesh attributes.
    CopyAttributesBetweenMeshes(targetMesh, sourceMesh, targetMesh, sourceMesh);

    // Copy all vertices.
    ConstVertexPtrToVertexPtrMap vertexMap;
    for (ConstVertexPtr sourceVertexPtr = sourceMesh.vertexBegin();
         sourceVertexPtr != sourceMesh.vertexEnd(); ++sourceVertexPtr) {

        VertexPtr targetVertexPtr = targetMesh->createVertex();
        vertexMap[sourceVertexPtr] = targetVertexPtr;

        targetVertexPtr->setPosition(sourceVertexPtr->position());

        CopyAttributesBetweenMeshes(targetMesh, sourceMesh, &*targetVertexPtr, *sourceVertexPtr);
    }

    // Copy all edges.
    ConstEdgePtrToEdgePtrMap edgeMap;
    for (ConstEdgePtr sourceEdgePtr = sourceMesh.edgeBegin();
         sourceEdgePtr != sourceMesh.edgeEnd(); ++sourceEdgePtr) {

        EdgePtr targetEdgePtr = targetMesh->createEdge();
        edgeMap[sourceEdgePtr] = targetEdgePtr;

        CopyAttributesBetweenMeshes(targetMesh, sourceMesh, &*targetEdgePtr, *sourceEdgePtr);
    }

    // Copy all faces.
    ConstFacePtrToFacePtrMap faceMap;
    for (ConstFacePtr sourceFacePtr = sourceMesh.faceBegin();
         sourceFacePtr != sourceMesh.faceEnd(); ++sourceFacePtr) {

        FacePtr targetFacePtr = targetMesh->createFace();
        faceMap[sourceFacePtr] = targetFacePtr;

        CopyAttributesBetweenMeshes(targetMesh, sourceMesh, &*targetFacePtr, *sourceFacePtr);
    }

    // Copy adjacent elements of all vertices.
    for (ConstVertexPtr sourceVertexPtr = sourceMesh.vertexBegin();
         sourceVertexPtr != sourceMesh.vertexEnd(); ++sourceVertexPtr) {

        VertexPtr targetVertexPtr = vertexMap[sourceVertexPtr];

        // Copy all adjacent edge pointers.
        for (AdjacentEdgeConstIterator iterator = sourceVertexPtr->adjacentEdgeBegin();
             iterator != sourceVertexPtr->adjacentEdgeEnd(); ++iterator) {
            targetVertexPtr->addAdjacentEdge(edgeMap[*iterator]);
        }

        // Copy all adjacent face pointers.
        for (AdjacentFaceConstIterator iterator = sourceVertexPtr->adjacentFaceBegin();
             iterator != sourceVertexPtr->adjacentFaceEnd(); ++iterator) {
            targetVertexPtr->addAdjacentFace(faceMap[*iterator]);
        }
    }

    // Copy adjacent elements of all edges.
    for (ConstEdgePtr sourceEdgePtr = sourceMesh.edgeBegin();
         sourceEdgePtr != sourceMesh.edgeEnd(); ++sourceEdgePtr) {

        EdgePtr targetEdgePtr = edgeMap[sourceEdgePtr];

        // Copy all adjacent vertex pointers.
        for (AdjacentVertexConstIterator iterator = sourceEdgePtr->adjacentVertexBegin();
             iterator != sourceEdgePtr->adjacentVertexEnd(); ++iterator) {
            targetEdgePtr->addAdjacentVertex(vertexMap[*iterator]);
        }

        // Copy all adjacent face pointers.
        for (AdjacentFaceConstIterator iterator = sourceEdgePtr->adjacentFaceBegin();
             iterator != sourceEdgePtr->adjacentFaceEnd(); ++iterator) {
            targetEdgePtr->addAdjacentFace(faceMap[*iterator]);
        }
    }

    // Copy adjacent elements of all faces.
    for (ConstFacePtr sourceFacePtr = sourceMesh.faceBegin();
         sourceFacePtr != sourceMesh.faceEnd(); ++sourceFacePtr) {

        FacePtr targetFacePtr = faceMap[sourceFacePtr];

        // Copy all adjacent vertex pointers.
        for (AdjacentVertexConstIterator iterator = sourceFacePtr->adjacentVertexBegin();
             iterator != sourceFacePtr->adjacentVertexEnd(); ++iterator) {
            targetFacePtr->addAdjacentVertex(vertexMap[*iterator]);
        }

        // Copy all adjacent edge pointers.
        for (AdjacentEdgeConstIterator iterator = sourceFacePtr->adjacentEdgeBegin();
             iterator != sourceFacePtr->adjacentEdgeEnd(); ++iterator) {
            targetFacePtr->addAdjacentEdge(edgeMap[*iterator]);
        }
    }

    // Copy all face vertex attributes.
    for (ConstFacePtr sourceFacePtr = sourceMesh.faceBegin();
         sourceFacePtr != sourceMesh.faceEnd(); ++sourceFacePtr) {
        FacePtr targetFacePtr = faceMap[sourceFacePtr];

        for (AdjacentVertexConstIterator iterator = sourceFacePtr->adjacentVertexBegin();
             iterator != sourceFacePtr->adjacentVertexEnd(); ++iterator) {

            ConstVertexPtr sourceVertexPtr = *iterator;
            VertexPtr targetVertexPtr = vertexMap[sourceVertexPtr];

            const FaceVertex *sourceFaceVertex = sourceFacePtr->findFaceVertex(sourceVertexPtr);
            if (sourceFaceVertex != NULL) {
                CopyAttributesBetweenMeshes(targetMesh, sourceMesh, 
                    &*targetFacePtr->getFaceVertex(targetVertexPtr),
                    *sourceFaceVertex);
            }
        }
    }
}

static void 
CopyAttributesBetweenMeshes(Mesh *targetMesh, const Mesh &sourceMesh,
    AttributePossessor *targetAttributePossessor,
    const AttributePossessor &sourceAttributePossessor)
{
    for (AttributePossessor::const_iterator iterator 
             = sourceAttributePossessor.attributeDataBegin();
         iterator != sourceAttributePossessor.attributeDataEnd(); ++iterator) {
        const AttributeData &sourceAttributeData = *iterator;

        const std::string *sourceAttributeKeyName = NULL;
        const AttributeKey *sourceAttributeKey = NULL;

#ifdef DEBUG        
        bool attributeWasFound = 
#endif
            sourceMesh.findAttributeNameAndKeyFromHandle(sourceAttributeData.handle(),
                &sourceAttributeKeyName, &sourceAttributeKey);
        assert(attributeWasFound);

        AttributeKey targetAttributeKey = targetMesh->getAttributeKey(
            *sourceAttributeKeyName, (*sourceAttributeKey).type(),
            (*sourceAttributeKey).flags());

        AttributeData *targetAttributeData 
           = targetAttributePossessor->findOrCreateAttributeData(targetAttributeKey);

        (*targetAttributeData).copyData(sourceAttributeData);
    }
}

} // namespace mesh
