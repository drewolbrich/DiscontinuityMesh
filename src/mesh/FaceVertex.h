// Copyright 2008 Drew Olbrich

#ifndef MESH__FACE_VERTEX__INCLUDED
#define MESH__FACE_VERTEX__INCLUDED

#include "AttributePossessor.h"
#include "Types.h"

namespace mesh {

// FaceVertex
//
// This class holds face attributes that are specified
// on a per-vertex basis.

class FaceVertex : public AttributePossessor
{
public:
    FaceVertex();
    explicit FaceVertex(const ConstVertexPtr &vertexPtr);
    ~FaceVertex();

    // The vertex that the FaceVertex corresponds to.
    ConstVertexPtr vertexPtr() const;
    
private:
    ConstVertexPtr mVertexPtr;
};

} // namespace mesh

#endif // MESH__FACE_VERTEX__INCLUDED
