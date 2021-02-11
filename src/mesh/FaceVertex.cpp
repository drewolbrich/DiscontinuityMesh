// Copyright 2008 Drew Olbrich

#include "FaceVertex.h"

namespace mesh {

FaceVertex::FaceVertex()
    : AttributePossessor(),
      mVertexPtr()
{
}

FaceVertex::FaceVertex(const ConstVertexPtr &vertexPtr)
    : AttributePossessor(),
      mVertexPtr(vertexPtr)
{
}

FaceVertex::~FaceVertex()
{
}

ConstVertexPtr
FaceVertex::vertexPtr() const
{
    return mVertexPtr;
}

} // namespace mesh
