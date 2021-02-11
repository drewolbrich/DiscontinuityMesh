// Copyright 2008 Drew Olbrich

#ifndef MESH__VERTEX__INCLUDED
#define MESH__VERTEX__INCLUDED

#include <cgmath/Vector3f.h>

#include "Types.h"
#include "AttributePossessor.h"

namespace mesh {

// Vertex
//
// A vertex managed by the Mesh class.

class Vertex : public AttributePossessor
{
public:
    Vertex();
    ~Vertex();

    // The position of the vertex.
    void setPosition(const cgmath::Vector3f &position);
    const cgmath::Vector3f &position() const;

    // Adjacent edges.
    void addAdjacentEdge(EdgePtr edgePtr);
    void removeAdjacentEdge(EdgePtr edgePtr);
    bool hasAdjacentEdge(ConstEdgePtr edgePtr) const;
    unsigned int adjacentEdgeCount() const;
    AdjacentEdgeIterator adjacentEdgeBegin();
    AdjacentEdgeIterator adjacentEdgeEnd();
    AdjacentEdgeConstIterator adjacentEdgeBegin() const;
    AdjacentEdgeConstIterator adjacentEdgeEnd() const;

    // Adjacent faces.
    void addAdjacentFace(FacePtr facePtr);
    void removeAdjacentFace(FacePtr facePtr);
    bool hasAdjacentFace(ConstFacePtr facePtr) const;
    unsigned int adjacentFaceCount() const;
    AdjacentFaceIterator adjacentFaceBegin();
    AdjacentFaceIterator adjacentFaceEnd();
    AdjacentFaceConstIterator adjacentFaceBegin() const;
    AdjacentFaceConstIterator adjacentFaceEnd() const;

    // Find an adjacent edge spanning this vertex and a specified
    // vertex. If there is no such edge, adjacentEdgeEnd() is
    // returned.
    AdjacentEdgeIterator findAdjacentEdgeByVertex(ConstVertexPtr vertexPtr);
    AdjacentEdgeConstIterator findAdjacentEdgeByVertex(ConstVertexPtr vertexPtr) const;
    
private:
    cgmath::Vector3f mPosition;

    AdjacentEdgeVector mEdgePtrVector;
    AdjacentFaceVector mFacePtrVector;
};

} // namespace mesh

#endif // MESH__VERTEX__INCLUDED
