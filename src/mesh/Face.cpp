// Copyright 2008 Drew Olbrich

#include "Face.h"

#include <cassert>
#include <algorithm>

namespace mesh {

Face::Face()
    : AttributePossessor(),
      mVertexPtrVector(),
      mEdgePtrVector(),
      mFaceVertexVector()
{
}

Face::~Face()
{
    // Delete all the face vertices that we allocated, because
    // we own them.
    for (FaceVertexVector::iterator iterator = mFaceVertexVector.begin();
         iterator != mFaceVertexVector.end(); ++iterator) {
        delete *iterator;
    }
}

void
Face::addAdjacentVertex(VertexPtr vertexPtr)
{
    // The face must not already have this vertex.
    assert(!hasAdjacentVertex(vertexPtr));

    // Add the vertex to the face's vertex vector.
    mVertexPtrVector.push_back(vertexPtr);
}

void
Face::prependAdjacentVertex(VertexPtr vertexPtr)
{
    // The face must not already have this vertex.
    assert(!hasAdjacentVertex(vertexPtr));

    // Add the vertex to the face's vertex vector.
    mVertexPtrVector.insert(mVertexPtrVector.begin(), vertexPtr);
}

void 
Face::insertAdjacentVertex(VertexPtr vertexPtr, VertexPtr beforeVertexPtr)
{
    // The face must not already have this vertex.
    assert(!hasAdjacentVertex(vertexPtr));

    AdjacentVertexVector::iterator iterator 
        = std::find(mVertexPtrVector.begin(), mVertexPtrVector.end(), beforeVertexPtr);

    assert(iterator != mVertexPtrVector.end());
    
    // Add the vertex to the face's vertex vector.
    mVertexPtrVector.insert(iterator, vertexPtr);
}

void 
Face::removeAdjacentVertex(VertexPtr vertexPtr)
{
    // The face must already have this vertex.
    assert(hasAdjacentVertex(vertexPtr));

    // Remove the vertex from the face's vertex vector.
    mVertexPtrVector.erase(std::find(mVertexPtrVector.begin(), 
            mVertexPtrVector.end(), vertexPtr));

    // Remove the FaceVertex corresponding to this VertexPtr.
    eraseFaceVertex(vertexPtr);
}

bool 
Face::hasAdjacentVertex(ConstVertexPtr vertexPtr) const
{
    // Return true if the vertex is in the face's vertex list.
    return std::find(mVertexPtrVector.begin(), mVertexPtrVector.end(), vertexPtr) 
        != mVertexPtrVector.end();
}

unsigned int
Face::adjacentVertexCount() const
{
    return mVertexPtrVector.size();
}

AdjacentVertexIterator
Face::adjacentVertexBegin()
{
    return mVertexPtrVector.begin();
}

AdjacentVertexIterator
Face::adjacentVertexEnd()
{
    return mVertexPtrVector.end();
}

AdjacentVertexConstIterator
Face::adjacentVertexBegin() const
{
    return mVertexPtrVector.begin();
}

AdjacentVertexConstIterator
Face::adjacentVertexEnd() const
{
    return mVertexPtrVector.end();
}

AdjacentVertexReverseIterator
Face::adjacentVertexReverseBegin()
{
    return mVertexPtrVector.rbegin();
}

AdjacentVertexReverseIterator
Face::adjacentVertexReverseEnd()
{
    return mVertexPtrVector.rend();
}

AdjacentVertexConstReverseIterator
Face::adjacentVertexReverseBegin() const
{
    return mVertexPtrVector.rbegin();
}

AdjacentVertexConstReverseIterator
Face::adjacentVertexReverseEnd() const
{
    return mVertexPtrVector.rend();
}

void
Face::addAdjacentEdge(EdgePtr edge)
{
    // The face must not already have this edge.
    assert(!hasAdjacentEdge(edge));

    // Add the edge to the face's edge list.
    mEdgePtrVector.push_back(edge);
}

void 
Face::insertAdjacentEdge(EdgePtr edgePtr, EdgePtr beforeEdgePtr)
{
    // The face must not already have this edge.
    assert(!hasAdjacentEdge(edgePtr));

    AdjacentEdgeVector::iterator iterator 
        = std::find(mEdgePtrVector.begin(), mEdgePtrVector.end(), beforeEdgePtr);

    // Add the edge to the face's edge vector.
    mEdgePtrVector.insert(iterator, edgePtr);
}

void 
Face::removeAdjacentEdge(EdgePtr edge)
{
    // The face must already have this edge.
    assert(hasAdjacentEdge(edge));

    // Remove the edge from the face's edge list.
    mEdgePtrVector.erase(find(mEdgePtrVector.begin(), mEdgePtrVector.end(), edge));
}

bool 
Face::hasAdjacentEdge(ConstEdgePtr edge) const
{
    // Return true if the edge is in the face's edge list.
    return find(mEdgePtrVector.begin(), mEdgePtrVector.end(), edge) != mEdgePtrVector.end();
}

unsigned int
Face::adjacentEdgeCount() const
{
    return mEdgePtrVector.size();
}

AdjacentEdgeIterator
Face::adjacentEdgeBegin()
{
    return mEdgePtrVector.begin();
}

AdjacentEdgeIterator
Face::adjacentEdgeEnd()
{
    return mEdgePtrVector.end();
}

AdjacentEdgeConstIterator
Face::adjacentEdgeBegin() const
{
    return mEdgePtrVector.begin();
}

AdjacentEdgeConstIterator
Face::adjacentEdgeEnd() const
{
    return mEdgePtrVector.end();
}

bool 
Face::hasVertexAttribute(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return false;
    }

    return faceVertex->hasAttribute(key);
}

void 
Face::eraseVertexAttribute(ConstVertexPtr vertexPtr, AttributeKey key)
{
    FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        // No corresponding FaceVertex exists.
        return;
    }

    // Erase the attribute.
    faceVertex->eraseAttribute(key);

    // If the FaceVertex has no attributes left, remove
    // the FaceVertex itself to save memory.
    if (faceVertex->hasNoAttributes()) {
        eraseFaceVertex(vertexPtr);
    }
}

bool
Face::getVertexBool(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_BOOL;
    }

    return faceVertex->getBool(key);
}

int32_t
Face::getVertexInt(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_INT;
    }

    return faceVertex->getInt(key);
}


float
Face::getVertexFloat(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_FLOAT;
    }

    return faceVertex->getFloat(key);
}

cgmath::Vector2f
Face::getVertexVector2f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_VECTOR2F;
    }

    return faceVertex->getVector2f(key);
}

cgmath::Vector3f
Face::getVertexVector3f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_VECTOR3F;
    }

    return faceVertex->getVector3f(key);
}

cgmath::Vector4f
Face::getVertexVector4f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_VECTOR4F;
    }

    return faceVertex->getVector4f(key);
}

cgmath::Matrix3f
Face::getVertexMatrix3f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_MATRIX3F;
    }

    return faceVertex->getMatrix3f(key);
}

cgmath::Matrix4f
Face::getVertexMatrix4f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_MATRIX4F;
    }

    return faceVertex->getMatrix4f(key);
}

std::string
Face::getVertexString(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_STRING;
    }

    return faceVertex->getString(key);
}

cgmath::Vector3f
Face::getVertexUnitVector3f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_UNIT_VECTOR3F;
    }

    return faceVertex->getUnitVector3f(key);
}

cgmath::BoundingBox2f
Face::getVertexBoundingBox2f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_BOUNDINGBOX2F;
    }

    return faceVertex->getBoundingBox2f(key);
}

cgmath::BoundingBox3f
Face::getVertexBoundingBox3f(ConstVertexPtr vertexPtr, AttributeKey key) const
{
    const FaceVertex *faceVertex = findFaceVertex(vertexPtr);
    if (faceVertex == NULL) {
        return AttributeKey::DEFAULT_BOUNDINGBOX3F;
    }

    return faceVertex->getBoundingBox3f(key);
}

void 
Face::setVertexBool(ConstVertexPtr vertexPtr, AttributeKey key, bool value)
{
    getFaceVertex(vertexPtr)->setBool(key, value);
}

void 
Face::setVertexInt(ConstVertexPtr vertexPtr, AttributeKey key, int32_t value)
{
    getFaceVertex(vertexPtr)->setInt(key, value);
}

void 
Face::setVertexFloat(ConstVertexPtr vertexPtr, AttributeKey key, float value)
{
    getFaceVertex(vertexPtr)->setFloat(key, value);
}

void 
Face::setVertexVector2f(ConstVertexPtr vertexPtr, AttributeKey key, const cgmath::Vector2f &value)
{
    getFaceVertex(vertexPtr)->setVector2f(key, value);
}

void 
Face::setVertexVector3f(ConstVertexPtr vertexPtr, AttributeKey key, const cgmath::Vector3f &value)
{
    getFaceVertex(vertexPtr)->setVector3f(key, value);
}

void 
Face::setVertexVector4f(ConstVertexPtr vertexPtr, AttributeKey key, const cgmath::Vector4f &value)
{
    getFaceVertex(vertexPtr)->setVector4f(key, value);
}

void 
Face::setVertexMatrix3f(ConstVertexPtr vertexPtr, AttributeKey key, const cgmath::Matrix3f &value)
{
    getFaceVertex(vertexPtr)->setMatrix3f(key, value);
}

void 
Face::setVertexMatrix4f(ConstVertexPtr vertexPtr, AttributeKey key, const cgmath::Matrix4f &value)
{
    getFaceVertex(vertexPtr)->setMatrix4f(key, value);
}

void 
Face::setVertexString(ConstVertexPtr vertexPtr, AttributeKey key, const std::string &value)
{
    getFaceVertex(vertexPtr)->setString(key, value);
}

void 
Face::setVertexUnitVector3f(ConstVertexPtr vertexPtr, AttributeKey key, 
    const cgmath::Vector3f &value)
{
    getFaceVertex(vertexPtr)->setUnitVector3f(key, value);
}

void
Face::setVertexBoundingBox2f(ConstVertexPtr vertexPtr,
    AttributeKey key, const cgmath::BoundingBox2f &value)
{
    getFaceVertex(vertexPtr)->setBoundingBox2f(key, value);
}

void
Face::setVertexBoundingBox3f(ConstVertexPtr vertexPtr,
    AttributeKey key, const cgmath::BoundingBox3f &value)
{
    getFaceVertex(vertexPtr)->setBoundingBox3f(key, value);
}

Face::FaceVertexVector::size_type 
Face::faceVertexVectorSize() const
{
    return mFaceVertexVector.size();
}

Face::FaceVertexVectorConstIterator 
Face::faceVertexVectorBegin() const
{
    return mFaceVertexVector.begin();
}

Face::FaceVertexVectorConstIterator 
Face::faceVertexVectorEnd() const
{
    return mFaceVertexVector.end();
}

FaceVertex *
Face::getFaceVertex(ConstVertexPtr vertexPtr)
{
    for (FaceVertexVector::iterator iterator = mFaceVertexVector.begin();
         iterator != mFaceVertexVector.end(); ++iterator) {
        if ((*iterator)->vertexPtr() == vertexPtr) {
            return *iterator;
        }
    }

    // A FaceVertex corresponding to the VertexPtr could not
    // be found, so add it to the FaceVertexVector and return   
    // a pointer to it.
    mFaceVertexVector.push_back(new FaceVertex(vertexPtr));

    return mFaceVertexVector.back();
}

FaceVertex *
Face::findFaceVertex(ConstVertexPtr vertexPtr)
{
    for (FaceVertexVector::iterator iterator = mFaceVertexVector.begin();
         iterator != mFaceVertexVector.end(); ++iterator) {
        if ((*iterator)->vertexPtr() == vertexPtr) {
            return *iterator;
        }
    }

    return NULL;
}

const FaceVertex *
Face::findFaceVertex(ConstVertexPtr vertexPtr) const
{
    for (FaceVertexVector::const_iterator iterator = mFaceVertexVector.begin();
         iterator != mFaceVertexVector.end(); ++iterator) {
        if ((*iterator)->vertexPtr() == vertexPtr) {
            return *iterator;
        }
    }

    return NULL;
}

bool
Face::hasFaceVertex(ConstVertexPtr vertexPtr) const
{
    return findFaceVertex(vertexPtr) != NULL;
}

void 
Face::eraseFaceVertex(ConstVertexPtr vertexPtr)
{
    for (FaceVertexVector::iterator iterator = mFaceVertexVector.begin();
         iterator != mFaceVertexVector.end(); ++iterator) {
        if ((*iterator)->vertexPtr() == vertexPtr) {
            delete *iterator;
            mFaceVertexVector.erase(iterator);
            return;
        }
    }
}

} // namespace mesh
