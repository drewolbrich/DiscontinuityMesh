// Copyright 2008 Drew Olbrich

#include "MeshOperations.h"

#include <cassert>

#include "Mesh.h"
#include "Vertex.h"

namespace delaunay {

cgmath::BoundingBox2f
GetMeshBoundingBox(MeshPtr meshPtr)
{
    assert(meshPtr.get() != NULL);

    cgmath::BoundingBox2f bbox;
    bbox.reset();

    for (VertexPtr vertexPtr = meshPtr->vertexBegin();
         vertexPtr != meshPtr->vertexEnd(); ++vertexPtr) {
        bbox.extendByVector2f(vertexPtr->position());
    }

    return bbox;
}

} // namespace delaunay
