// Copyright 2008 Retroactive Fiasco.

#include "EndpointIdentifier.h"

#include <algorithm>
#include <cassert>

#include <mesh/Vertex.h>
#include <mesh/Edge.h>
#include <mesh/Face.h>

namespace meshretri {

static const uintptr_t UNDEFINED_ID = 0;

EndpointIdentifier::EndpointIdentifier()
    : mType(UNDEFINED),
      mId1(UNDEFINED_ID),
      mId2(UNDEFINED_ID),
      mId3(UNDEFINED_ID),
      mVertexPtr(),
      mEdgePtr(),
      mWedgeVertexPtr()
{
}

EndpointIdentifier::~EndpointIdentifier()
{
}

EndpointIdentifier 
EndpointIdentifier::fromVertexPtr(mesh::VertexPtr vertexPtr)
{
    EndpointIdentifier id;
    id.setVertexPtr(vertexPtr);

    return id;
}

EndpointIdentifier 
EndpointIdentifier::fromVertexPtrPair(mesh::VertexPtr vertexPtr1, 
    mesh::VertexPtr vertexPtr2)
{
    EndpointIdentifier id;
    id.setVertexPtrPair(vertexPtr1, vertexPtr2);

    return id;
}

EndpointIdentifier 
EndpointIdentifier::fromVertexPtrAndIndex(mesh::VertexPtr vertexPtr, unsigned index)
{
    EndpointIdentifier id;
    id.setVertexPtrAndIndex(vertexPtr, index);

    return id;
}

EndpointIdentifier 
EndpointIdentifier::fromEdgePtrAndIndex(mesh::EdgePtr edgePtr, unsigned index)
{
    EndpointIdentifier id;
    id.setEdgePtrAndIndex(edgePtr, index);

    return id;
}

EndpointIdentifier 
EndpointIdentifier::fromEdgePtrPairAndVertex(mesh::EdgePtr edgePtr1, mesh::EdgePtr edgePtr2,
    mesh::VertexPtr vertexPtr)
{
    EndpointIdentifier id;
    id.setEdgePtrPairAndVertex(edgePtr1, edgePtr2, vertexPtr);

    return id;
}

EndpointIdentifier
EndpointIdentifier::fromEdgePtrPairAndIndex(mesh::EdgePtr edgePtr1,
    mesh::EdgePtr edgePtr2, unsigned index)
{
    EndpointIdentifier id;
    id.setEdgePtrPairAndIndex(edgePtr1, edgePtr2, index);

    return id;
}

void
EndpointIdentifier::setVertexPtr(mesh::VertexPtr vertexPtr)
{
    mType = ONE_VERTEX_POINTER;
    mId1 = reinterpret_cast<uintptr_t>(&*vertexPtr);
    mId2 = 0;
    mId3 = 0;

    mVertexPtr = vertexPtr;
}

void
EndpointIdentifier::setVertexPtrPair(mesh::VertexPtr vertexPtr1, mesh::VertexPtr vertexPtr2)
{
    mType = TWO_VERTEX_POINTERS;
    mId1 = reinterpret_cast<uintptr_t>(&*vertexPtr1);
    mId2 = reinterpret_cast<uintptr_t>(&*vertexPtr2);
    mId3 = 0;

    // Make sure the identifiers are in a consistent order so
    // the comparison test won't fail if the vertices were specified
    // in different orders for two EndpointIdentifiers.
    if (mId2 < mId1) {
        std::swap(mId1, mId2);
    }
}

void
EndpointIdentifier::setVertexPtrAndIndex(mesh::VertexPtr vertexPtr, unsigned index)
{
    mType = VERTEX_POINTER_AND_INDEX;
    mId1 = reinterpret_cast<uintptr_t>(&*vertexPtr);
    mId2 = index;
    mId3 = 0;

    mVertexPtr = vertexPtr;
}

void
EndpointIdentifier::setEdgePtrAndIndex(mesh::EdgePtr edgePtr, unsigned index)
{
    mType = EDGE_POINTER_AND_INDEX;
    mId1 = reinterpret_cast<uintptr_t>(&*edgePtr);
    mId2 = index;
    mId3 = 0;

    mEdgePtr = edgePtr;
}

void 
EndpointIdentifier::setEdgePtrPairAndVertex(mesh::EdgePtr edgePtr1, mesh::EdgePtr edgePtr2,
    mesh::VertexPtr vertexPtr)
{
    mType = TWO_EDGE_POINTERS_AND_VERTEX;
    mId1 = reinterpret_cast<uintptr_t>(&*edgePtr1);
    mId2 = reinterpret_cast<uintptr_t>(&*edgePtr2);
    mId3 = reinterpret_cast<uintptr_t>(&*vertexPtr);

    // Make sure the identifiers are in a consistent order so
    // the comparison test won't fail if the edges were specified
    // in different orders for two EndpointIdentifiers.
    if (mId2 < mId1) {
        std::swap(mId1, mId2);
    }
}

void
EndpointIdentifier::setEdgePtrPairAndIndex(mesh::EdgePtr edgePtr1, mesh::EdgePtr edgePtr2,
    unsigned index)
{
    mType = TWO_EDGE_POINTERS_AND_INDEX;
    mId1 = reinterpret_cast<uintptr_t>(&*edgePtr1);
    mId2 = reinterpret_cast<uintptr_t>(&*edgePtr2);
    mId3 = index;

    // Make sure the identifiers are in a consistent order so
    // the comparison test won't fail if the edges were specified
    // in different orders for two EndpointIdentifiers.
    if (mId2 < mId1) {
        std::swap(mId1, mId2);
    }
}

bool 
EndpointIdentifier::getVertexPtr(mesh::VertexPtr *vertexPtr) const
{
    assert(mType != UNDEFINED);

    if (mType != ONE_VERTEX_POINTER
        && mType != VERTEX_POINTER_AND_INDEX) {
        return false;
    }

    *vertexPtr = mVertexPtr;

    return true;
}

bool
EndpointIdentifier::getEdgePtr(mesh::EdgePtr *edgePtr) const
{
    assert(mType != UNDEFINED);

    if (mType != EDGE_POINTER_AND_INDEX) {
        return false;
    }

    *edgePtr = mEdgePtr;

    return true;
}

bool
EndpointIdentifier::isUndefined() const
{
    return mType == UNDEFINED;
}

bool 
EndpointIdentifier::operator==(const EndpointIdentifier &rhs) const
{
    assert(mType != UNDEFINED);

    return mType == rhs.mType
        && mId1 == rhs.mId1 
        && mId2 == rhs.mId2
        && mId3 == rhs.mId3;
}

bool 
EndpointIdentifier::operator!=(const EndpointIdentifier &rhs) const
{
    assert(mType != UNDEFINED);

    return !(*this == rhs);
}

bool 
EndpointIdentifier::operator<(const EndpointIdentifier &rhs) const
{
    assert(mType != UNDEFINED);

    if (mType != rhs.mType) {
        return mType < rhs.mType;
    } else if (mId3 != rhs.mId3) {
        return mId3 < rhs.mId3;
    } else if (mId2 != rhs.mId2) {
        return mId2 < rhs.mId2;
    } else {
        return mId1 < rhs.mId1;
    }
}

std::ostream &
operator<<(std::ostream &ostr, const EndpointIdentifier &rhs)
{
    switch (rhs.mType) {
    case EndpointIdentifier::UNDEFINED:
        // This should never happen.
        abort();
        break;
    case EndpointIdentifier::ONE_VERTEX_POINTER:
        ostr << "ONE_VERTEX_POINTER";
        break;
    case EndpointIdentifier::TWO_VERTEX_POINTERS:
        ostr << "TWO_VERTEX_POINTERS";
        break;
    case EndpointIdentifier::VERTEX_POINTER_AND_INDEX:
        ostr << "VERTEX_POINTER_AND_INDEX";
        break;
    case EndpointIdentifier::EDGE_POINTER_AND_INDEX:
        ostr << "EDGE_POINTER_AND_INDEX";
        break;
    case EndpointIdentifier::TWO_EDGE_POINTERS_AND_VERTEX:
        ostr << "TWO_EDGE_POINTERS_AND_VERTEX";
        break;
    case EndpointIdentifier::TWO_EDGE_POINTERS_AND_INDEX:
        ostr << "TWO_EDGE_POINTERS_AND_INDEX";
        break;
    case EndpointIdentifier::UNIQUE:
        ostr << "UNIQUE";
        break;
    }

    ostr << "(" << uint64_t(rhs.mId1) << ":" << uint64_t(rhs.mId2)
        << ":" << uint64_t(rhs.mId3) << ")";

    return ostr;
}

EndpointIdentifier
EndpointIdentifier::createUniqueIdentifier()
{
    // This is not thread safe.
    static uintptr_t sCounter = 0;

    EndpointIdentifier endpointIdentifier;
    endpointIdentifier.mType = UNIQUE;
    endpointIdentifier.mId1 = sCounter;
    endpointIdentifier.mId2 = 0;
    endpointIdentifier.mId3 = 0;

    ++sCounter;
    assert(sCounter != 0);

    return endpointIdentifier;
}

EndpointIdentifier
EndpointIdentifier::createUndefined()
{
    return EndpointIdentifier();
}

} // namespace meshretri
