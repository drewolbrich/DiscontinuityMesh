// Copyright 2008 Drew Olbrich

#ifndef MESH__FACE__INCLUDED
#define MESH__FACE__INCLUDED

#include <cgmath/Vector3f.h>

#include "Types.h"
#include "AttributePossessor.h"
#include "FaceVertex.h"

namespace mesh {

// Face
//
// A face managed by the Mesh class.

class Face : public AttributePossessor
{
public:
    Face();
    ~Face();

    // Adjacent vertices.
    void addAdjacentVertex(VertexPtr vertexPtr);
    void prependAdjacentVertex(VertexPtr vertexPtr);
    void insertAdjacentVertex(VertexPtr vertexPtr, VertexPtr beforeVertexPtr);
    void removeAdjacentVertex(VertexPtr vertexPtr);
    bool hasAdjacentVertex(ConstVertexPtr vertexPtr) const;
    unsigned int adjacentVertexCount() const;
    AdjacentVertexIterator adjacentVertexBegin();
    AdjacentVertexIterator adjacentVertexEnd();
    AdjacentVertexConstIterator adjacentVertexBegin() const;
    AdjacentVertexConstIterator adjacentVertexEnd() const;
    AdjacentVertexReverseIterator adjacentVertexReverseBegin();
    AdjacentVertexReverseIterator adjacentVertexReverseEnd();
    AdjacentVertexConstReverseIterator adjacentVertexReverseBegin() const;
    AdjacentVertexConstReverseIterator adjacentVertexReverseEnd() const;

    // Adjacent edges.
    void addAdjacentEdge(EdgePtr edgePtr);
    void insertAdjacentEdge(EdgePtr edgePtr, EdgePtr beforeEdgePtr);
    void removeAdjacentEdge(EdgePtr edgePtr);
    bool hasAdjacentEdge(ConstEdgePtr edgePtr) const;
    unsigned int adjacentEdgeCount() const;
    AdjacentEdgeIterator adjacentEdgeBegin();
    AdjacentEdgeIterator adjacentEdgeEnd();
    AdjacentEdgeConstIterator adjacentEdgeBegin() const;
    AdjacentEdgeConstIterator adjacentEdgeEnd() const;

    // The following functions provide for per-vertex attributes for
    // the Face, in addition to the attribute functionality inherited
    // from the AttributePossessor class, which apply to the Face
    // as a whole.
    
    // Returns true if the specified attribute is defined for 
    // a specified face vertex.
    bool hasVertexAttribute(ConstVertexPtr vertexPtr, AttributeKey key) const;

    // Remove the specified attribute for a specified
    // face vertex, if it exists.
    void eraseVertexAttribute(ConstVertexPtr vertexPtr, AttributeKey key);

    // Returns the value of the specified face vertex attribute. If
    // the attribute is not yet defined, a default value is returned
    // (false, zero, the identity matrix, or the empty string.)
    bool getVertexBool(ConstVertexPtr vertexPtr, AttributeKey key) const;
    int32_t getVertexInt(ConstVertexPtr vertexPtr, AttributeKey key) const;
    float getVertexFloat(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::Vector2f getVertexVector2f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::Vector3f getVertexVector3f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::Vector4f getVertexVector4f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::Matrix3f getVertexMatrix3f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::Matrix4f getVertexMatrix4f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    std::string getVertexString(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::Vector3f getVertexUnitVector3f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::BoundingBox2f getVertexBoundingBox2f(ConstVertexPtr vertexPtr, AttributeKey key) const;
    cgmath::BoundingBox3f getVertexBoundingBox3f(ConstVertexPtr vertexPtr, AttributeKey key) const;

    // Set the specified attribute for a face vertex.
    void setVertexBool(ConstVertexPtr vertexPtr, 
        AttributeKey key, bool value);
    void setVertexInt(ConstVertexPtr vertexPtr, 
        AttributeKey key, int32_t value);
    void setVertexFloat(ConstVertexPtr vertexPtr, 
        AttributeKey key, float value);
    void setVertexVector2f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::Vector2f &value);
    void setVertexVector3f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::Vector3f &value);
    void setVertexVector4f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::Vector4f &value);
    void setVertexMatrix3f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::Matrix3f &value);
    void setVertexMatrix4f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::Matrix4f &value);
    void setVertexString(ConstVertexPtr vertexPtr, 
        AttributeKey key, const std::string &value);
    void setVertexUnitVector3f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::Vector3f &value);
    void setVertexBoundingBox2f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::BoundingBox2f &value);
    void setVertexBoundingBox3f(ConstVertexPtr vertexPtr, 
        AttributeKey key, const cgmath::BoundingBox3f &value);

    // The vector of FaceVertex objects used to hold
    // vertex-specific attributes for the Face.
    // These functions should not normally be called
    // by user code. The setVertex* functions above should be
    // used instead.
    // The FaceVertex objects traversed by these iterators do not necessarily have the same
    // ordering as the vertices adjacent to the face.
    typedef std::vector<FaceVertex *> FaceVertexVector;
    typedef FaceVertexVector::const_iterator FaceVertexVectorConstIterator;
    FaceVertexVector::size_type faceVertexVectorSize() const;
    FaceVertexVectorConstIterator faceVertexVectorBegin() const;
    FaceVertexVectorConstIterator faceVertexVectorEnd() const;

    // Return a pointer to the FaceVertex corresponding to the
    // specified VertexPtr, or create one if it does not
    // already exist.
    // This function should not normally be called
    // by user code. The setVertex* functions above should be
    // used instead.
    FaceVertex *getFaceVertex(ConstVertexPtr vertexPtr);

    // Returns a pointer to the FaceVertex corresponding to the
    // specified VertexPtr, or NULL if one does not exist.
    FaceVertex *findFaceVertex(ConstVertexPtr vertexPtr);
    const FaceVertex *findFaceVertex(ConstVertexPtr vertexPtr) const;

    // Returns true if a FaceVertex corresponding to the specified VertexPtr
    // already exists.
    bool hasFaceVertex(ConstVertexPtr vertexPtr) const;

private:
    // Removes the FaceVertex corresponding to the 
    // specified VertexPtr.
    void eraseFaceVertex(ConstVertexPtr vertexPtr);

    AdjacentVertexVector mVertexPtrVector;
    AdjacentEdgeVector mEdgePtrVector;

    FaceVertexVector mFaceVertexVector;
};

} // namespace mesh

#endif // MESH__FACE__INCLUDED
