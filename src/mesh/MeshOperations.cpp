// Copyright 2008 Retroactive Fiasco.

#include "MeshOperations.h"

#include <cassert>

#include <cgmath/Matrix4f.h>

#include "Mesh.h"
#include "VertexOperations.h"
#include "EdgeOperations.h"
#include "FaceOperations.h"
#include "StandardAttributes.h"

namespace mesh {

void 
DeleteOrphanedElements(Mesh *mesh)
{
    // Calling these in order of faces, edges, and then vertices
    // is important, because if faces are removed, this may
    // cause additional edges to be removed, which in turn may
    // cause additional vertices to be removed.
    
    DeleteOrphanedFaces(mesh);
    DeleteOrphanedEdges(mesh);
    DeleteOrphanedVertices(mesh);
}

void 
DeleteOrphanedVertices(Mesh *mesh)
{
    VertexPtr vertexPtr = mesh->vertexBegin();
    while (vertexPtr != mesh->vertexEnd()) {
        VertexPtr nextVertexPtr = vertexPtr;
        ++nextVertexPtr;
        if (vertexPtr->adjacentEdgeCount() == 0
            || vertexPtr->adjacentFaceCount() == 0) {
            DeleteVertex(mesh, vertexPtr);
        }
        vertexPtr = nextVertexPtr;
    }
}

void 
DeleteOrphanedEdges(Mesh *mesh)
{
    EdgePtr edgePtr = mesh->edgeBegin();
    while (edgePtr != mesh->edgeEnd()) {
        EdgePtr nextEdgePtr = edgePtr;
        ++nextEdgePtr;
        if (edgePtr->adjacentVertexCount() == 0
            || edgePtr->adjacentFaceCount() == 0) {
            DeleteEdge(mesh, edgePtr);
        }
        edgePtr = nextEdgePtr;
    }
}

void 
DeleteOrphanedFaces(Mesh *mesh)
{
    FacePtr facePtr = mesh->faceBegin();
    while (facePtr != mesh->faceEnd()) {
        FacePtr nextFacePtr = facePtr;
        ++nextFacePtr;
        if (facePtr->adjacentVertexCount() == 0
            || facePtr->adjacentEdgeCount() == 0) {
            DeleteFace(mesh, facePtr);
        }
        facePtr = nextFacePtr;
    }
}

void
Transform(Mesh *mesh, const cgmath::Matrix4f &matrix)
{
    // To correctly transform normals in the mesh, we must multiply
    // them by the inverse transpose of the transformation matrix.
    cgmath::Matrix3f normalMatrix(matrix);
    normalMatrix = normalMatrix.inverse().transpose();

    for (VertexPtr vertexPtr = mesh->vertexBegin(); vertexPtr != mesh->vertexEnd(); ++vertexPtr) {
        vertexPtr->setPosition(matrix*vertexPtr->position());
    }

    AttributeKey normal3fAttributeKey = GetNormal3fAttributeKey(*mesh);

    for (FacePtr facePtr = mesh->faceBegin(); facePtr != mesh->faceEnd(); ++facePtr) {
        if (facePtr->hasAttribute(normal3fAttributeKey)) {
            facePtr->setUnitVector3f(normal3fAttributeKey,
                (normalMatrix*facePtr->getUnitVector3f(normal3fAttributeKey)).normalize());
        }

        for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            VertexPtr vertexPtr = *iterator;
            if (facePtr->hasVertexAttribute(vertexPtr, normal3fAttributeKey)) {
                facePtr->setVertexUnitVector3f(vertexPtr, normal3fAttributeKey,
                    (normalMatrix*facePtr->getVertexUnitVector3f(vertexPtr, 
                        normal3fAttributeKey)).normalize());
            }
        }
    }
}

void
RemoveAttributeFromMeshAndAllElements(Mesh *mesh, AttributeKey attributeKey)
{
    mesh->eraseAttribute(attributeKey);

    for (VertexPtr vertexPtr = mesh->vertexBegin();
         vertexPtr != mesh->vertexEnd(); ++vertexPtr) {
        vertexPtr->eraseAttribute(attributeKey);
    }

    for (EdgePtr edgePtr = mesh->edgeBegin();
         edgePtr != mesh->edgeEnd(); ++edgePtr) {
        edgePtr->eraseAttribute(attributeKey);
    }

    for (FacePtr facePtr = mesh->faceBegin();
         facePtr != mesh->faceEnd(); ++facePtr) {
        facePtr->eraseAttribute(attributeKey);

        for (AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            VertexPtr vertexPtr = *iterator;
            facePtr->eraseVertexAttribute(vertexPtr, attributeKey);
        }
    }
}

cgmath::BoundingBox3f
ComputeBoundingBox(const Mesh &mesh)
{
    cgmath::BoundingBox3f boundingBox;
    boundingBox.reset();

    // Loop over all the vertices in the mesh, and extend the
    // bounding box to include that vertex.
    for (ConstVertexPtr vertex = mesh.vertexBegin();
         vertex != mesh.vertexEnd(); ++vertex) {
        boundingBox.extendByVector3f(vertex->position());
    }
    
    return boundingBox;
}

bool
AllFacesAreTriangles(const Mesh &mesh)
{
    for (ConstFacePtr facePtr = mesh.faceBegin(); facePtr != mesh.faceEnd(); ++facePtr) {
        if (facePtr->adjacentVertexCount() != 3) {
            return false;
        }
    }

    return true;
}

} // namespace mesh
