// Copyright 2009 Drew Olbrich

#include "DeletedElementTracker.h"

#include <cassert>

namespace mesh {

DeletedElementTracker::DeletedElementTracker()
    : mElementMask(ALL),
      mVertexPtrSet(),
      mEdgePtrSet(),
      mFacePtrSet()
{
}

DeletedElementTracker::~DeletedElementTracker()
{
}

void
DeletedElementTracker::clear()
{
    mVertexPtrSet.clear();
    mEdgePtrSet.clear();
    mFacePtrSet.clear();
}

void
DeletedElementTracker::setElementMask(unsigned elementMask)
{
    mElementMask = elementMask;
}

unsigned
DeletedElementTracker::elementMask() const
{
    return mElementMask;
}

void
DeletedElementTracker::addVertex(VertexPtr vertexPtr)
{
    if (mElementMask & VERTICES) {
        mVertexPtrSet.insert(vertexPtr);
    }
}

void
DeletedElementTracker::addEdge(EdgePtr edgePtr)
{
    if (mElementMask & EDGES) {
        mEdgePtrSet.insert(edgePtr);
    }
}

void
DeletedElementTracker::addFace(FacePtr facePtr)
{
    if (mElementMask & FACES) {
        mFacePtrSet.insert(facePtr);
    }
}

bool
DeletedElementTracker::hasVertex(VertexPtr vertexPtr)
{
    assert(mElementMask & VERTICES);

    return mVertexPtrSet.find(vertexPtr) != mVertexPtrSet.end();
}

bool
DeletedElementTracker::hasEdge(EdgePtr edgePtr)
{
    assert(mElementMask & EDGES);

    return mEdgePtrSet.find(edgePtr) != mEdgePtrSet.end();
}

bool
DeletedElementTracker::hasFace(FacePtr facePtr)
{
    assert(mElementMask & FACES);

    return mFacePtrSet.find(facePtr) != mFacePtrSet.end();
}

DeletedElementTracker::VertexIterator
DeletedElementTracker::vertexBegin() const
{
    assert(mElementMask & VERTICES);

    return mVertexPtrSet.begin();
}

DeletedElementTracker::VertexIterator
DeletedElementTracker::vertexEnd() const
{
    assert(mElementMask & VERTICES);

    return mVertexPtrSet.end();
}

DeletedElementTracker::EdgeIterator
DeletedElementTracker::edgeBegin() const
{
    assert(mElementMask & EDGES);

    return mEdgePtrSet.begin();
}

DeletedElementTracker::EdgeIterator
DeletedElementTracker::edgeEnd() const
{
    assert(mElementMask & EDGES);

    return mEdgePtrSet.end();
}

DeletedElementTracker::FaceIterator
DeletedElementTracker::faceBegin() const
{
    assert(mElementMask & FACES);

    return mFacePtrSet.begin();
}

DeletedElementTracker::FaceIterator
DeletedElementTracker::faceEnd() const
{
    assert(mElementMask & FACES);

    return mFacePtrSet.end();
}

void
DeletedElementTracker::addElementsFromElementTracker(const DeletedElementTracker &rhs)
{
    // The mask of DeletedElementTracker whose elements are being added must
    // include all of the element types that we ourselves are tracking.
    assert((mElementMask & rhs.mElementMask) == mElementMask);

    if (mElementMask & VERTICES) {
        for (VertexIterator iterator = rhs.vertexBegin(); 
             iterator != rhs.vertexEnd(); ++iterator) {
            addVertex(*iterator);
        }
    }

    if (mElementMask & EDGES) {
        for (EdgeIterator iterator = rhs.edgeBegin(); 
             iterator != rhs.edgeEnd(); ++iterator) {
            addEdge(*iterator);
        }
    }

    if (mElementMask & FACES) {
        for (FaceIterator iterator = rhs.faceBegin(); 
             iterator != rhs.faceEnd(); ++iterator) {
            addFace(*iterator);
        }
    }
}

} // namespace mesh
