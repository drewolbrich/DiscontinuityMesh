// Copyright 2008 Retroactive Fiasco.

#ifndef MESHRETRI__ENDPOINT_IDENTIFIER__INCLUDED
#define MESHRETRI__ENDPOINT_IDENTIFIER__INCLUDED

#include <stdint.h>

#include <ostream>

#include <mesh/Types.h>

namespace meshretri {

// EndpointIdentifier
//
// An identifier of an endpoint of a critical line segment, used to
// keep track of the endpoint's origin, so that line segment endpoints
// created from shared edges can be identified as the same point,
// without performing a possibly lossy comparison of the point's Cartesian coordinates
// or using an epsilon test.

class EndpointIdentifier
{
public:
    EndpointIdentifier();
    ~EndpointIdentifier();

    static EndpointIdentifier fromVertexPtr(mesh::VertexPtr vertexPtr);
    static EndpointIdentifier fromVertexPtrPair(mesh::VertexPtr vertexPtr1, 
        mesh::VertexPtr vertexPtr2);
    static EndpointIdentifier fromVertexPtrAndIndex(mesh::VertexPtr vertexPtr, unsigned index);
    static EndpointIdentifier fromEdgePtrAndIndex(mesh::EdgePtr edgePtr, unsigned index);
    static EndpointIdentifier fromEdgePtrPairAndVertex(mesh::EdgePtr edgePtr1, 
        mesh::EdgePtr edgePtr2, mesh::VertexPtr vertexPtr);
    static EndpointIdentifier fromEdgePtrPairAndIndex(mesh::EdgePtr edgePtr1, 
        mesh::EdgePtr edgePtr2, unsigned index);

    void setVertexPtr(mesh::VertexPtr vertexPtr);
    void setVertexPtrPair(mesh::VertexPtr vertexPtr1, mesh::VertexPtr vertexPtr2);
    void setVertexPtrAndIndex(mesh::VertexPtr vertexPtr, unsigned index);
    void setEdgePtrAndIndex(mesh::EdgePtr edgePtr, unsigned index);
    void setEdgePtrPairAndVertex(mesh::EdgePtr edgePtr1, mesh::EdgePtr edgePtr2,
        mesh::VertexPtr vertexPtr);
    void setEdgePtrPairAndIndex(mesh::EdgePtr edgePtr1, mesh::EdgePtr edgePtr2,
        unsigned index);

    // Returns the VertexPtr referenced by the EndpointIdentifier, if any.
    // False is returned if no VertexPtr is referenced.
    bool getVertexPtr(mesh::VertexPtr *vertexPtr) const;

    // Returns the EdgePtr referenced by the EndpointIdentifier,
    // if the EndpointIdentifier was defined by setEdgePtrAndIndex.
    // False is returned if no EdgePtr is referenced.
    bool getEdgePtr(mesh::EdgePtr *edgePtr) const;

    // Returns true if the EndpointIdentifier type is undefined.
    bool isUndefined() const;

    bool operator==(const EndpointIdentifier &rhs) const;
    bool operator!=(const EndpointIdentifier &rhs) const;
    bool operator<(const EndpointIdentifier &rhs) const;

    // Stream output.
    friend std::ostream &operator<<(std::ostream &ostr, const EndpointIdentifier &rhs);

    // Create a unique identifier that cannot conflict with
    // any of the identifiers assigned via pointers.
    static EndpointIdentifier createUniqueIdentifier();

    // Create an EndpointIdentifier of undefined type. Used in unit tests.
    static EndpointIdentifier createUndefined();

private:
    enum { 
        UNDEFINED,
        ONE_VERTEX_POINTER,
        TWO_VERTEX_POINTERS,
        VERTEX_POINTER_AND_INDEX,
        EDGE_POINTER_AND_INDEX,
        TWO_EDGE_POINTERS_AND_VERTEX,
        TWO_EDGE_POINTERS_AND_INDEX,
        UNIQUE
    } mType;

    uintptr_t mId1;
    uintptr_t mId2;
    uintptr_t mId3;

    mesh::VertexPtr mVertexPtr;
    mesh::EdgePtr mEdgePtr;

    mesh::VertexPtr mWedgeVertexPtr;
};

} // namespace meshretri

#endif // MESHRETRI__ENDPOINT_IDENTIFIER__INCLUDED
