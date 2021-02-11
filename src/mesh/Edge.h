// Copyright 2008 Drew Olbrich

#ifndef MESH__EDGE__INCLUDED
#define MESH__EDGE__INCLUDED

#include <cgmath/Vector3f.h>

#include "Types.h"
#include "AttributePossessor.h"

namespace mesh {

// Edge
//
// A edge managed by the Mesh class.

class Edge : public AttributePossessor
{
public:
    Edge();
    ~Edge();

    // Adjacent vertices.
    void addAdjacentVertex(VertexPtr vertexPtr);
    void removeAdjacentVertex(VertexPtr vertexPtr);
    bool hasAdjacentVertex(ConstVertexPtr vertexPtr) const;
    unsigned int adjacentVertexCount() const;
    AdjacentVertexIterator adjacentVertexBegin();
    AdjacentVertexIterator adjacentVertexEnd();
    AdjacentVertexConstIterator adjacentVertexBegin() const;
    AdjacentVertexConstIterator adjacentVertexEnd() const;

    // Adjacent faces.
    void addAdjacentFace(FacePtr facePtr);
    void removeAdjacentFace(FacePtr facePtr);
    bool hasAdjacentFace(ConstFacePtr facePtr) const;
    unsigned int adjacentFaceCount() const;
    AdjacentFaceIterator adjacentFaceBegin();
    AdjacentFaceIterator adjacentFaceEnd();
    AdjacentFaceConstIterator adjacentFaceBegin() const;
    AdjacentFaceConstIterator adjacentFaceEnd() const;

private:
    AdjacentVertexVector mVertexPtrVector;
    AdjacentFaceVector mFacePtrVector;
};

} // namespace mesh

#endif // MESH__EDGE__INCLUDED
