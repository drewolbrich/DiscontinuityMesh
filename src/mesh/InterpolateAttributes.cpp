// Copyright 2008 Retroactive Fiasco.

#include "InterpolateAttributes.h"

#include <cmath>
#include <cassert>
#include <algorithm>
#include <limits>

#include <cgmath/Vector2f.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Vector4f.h>

#include "Vertex.h"
#include "FaceVertex.h"
#include "Face.h"

using cgmath::Vector2f;
using cgmath::Vector3f;
using cgmath::Vector4f;

namespace mesh {

typedef std::vector<ConstVertexPtr> ConstVertexPtrVector;
typedef std::vector<float> WeightVector;
typedef std::vector<const AttributePossessor *> ConstAttributePossessorVector;

// Using mean value coordinates, get an attribute weight vector
// for a set of neighboring vertices, based on their position relative
// to a reference vertex.
void GetMeanValueCoordinateWeights(
    const ConstVertexPtrVector &vertexPtrVector,
    ConstVertexPtr vertexPtr, WeightVector &weightVector);

// Compute the tangent of half the angle between two vectors.
static float GetHalfAngleTangent(Vector3f a, Vector3f b);

// Assign an vertex attribute whose value is the weighted
// average of the attributes of other vertices.
static void AssignWeightedVertexAttribute(
    AttributePossessor &attributePossessor,
    const AttributeKey attributeKey, 
    const ConstAttributePossessorVector &constAttributePossessorVector,
    const WeightVector &weightVector);

void 
AssignInterpolatedVertexAttributes(VertexPtr vertexPtr, 
    const ConstVertexPtrVector &vertexPtrVector)
{
    // Handle the case when there are no reference vertices.
    if (vertexPtrVector.empty()) {
        return;
    }
    
    // Get the mean value coordinate weights for the neighboring
    // vertices, based on their position relative to the
    // reference vertex.
    WeightVector weightVector;
    GetMeanValueCoordinateWeights(vertexPtrVector,
        vertexPtr, weightVector);

    // We only want to assign the attributes that all of the source
    // vertices have in common. Only these attributes will be assigned
    // to the target vertex. The mean value coordinate weights assume
    // that all of the vertices are represented.

    // Loop over all the attributes of the first vertex, and make
    // sure that all the other vertices have this same attribute.
    ConstVertexPtr firstVertexPtr = *(vertexPtrVector.begin());
    for (AttributePossessor::const_iterator attributeDataIterator
             = firstVertexPtr->attributeDataBegin();
         attributeDataIterator != firstVertexPtr->attributeDataEnd();
         ++attributeDataIterator) {

        // Create an attribute key that corresponds to this attribute.
        const AttributeData &attributeData(*attributeDataIterator);
        const AttributeKey attributeKey(attributeData.handle(), 
            attributeData.type());

        // Check to see if all of the other vertices have the attribute.
        bool allVerticesHaveAttribute = true;
        ConstVertexPtrVector::const_iterator iterator 
            = vertexPtrVector.begin();
        for (++iterator; iterator != vertexPtrVector.end(); ++iterator) {
            if (!(*iterator)->hasAttribute(attributeKey)) {
                allVerticesHaveAttribute = false;
                break;
            }
        }

        // If all of the vertices have this attribute, 
        // compute the weighted average of the attributes of the
        // vertices and assign them to the reference vertex.
        if (allVerticesHaveAttribute) {
            ConstAttributePossessorVector constAttributePossessorVector;
            for (ConstVertexPtrVector::const_iterator iterator
                     = vertexPtrVector.begin();
                 iterator != vertexPtrVector.end(); ++iterator) {
                constAttributePossessorVector.push_back(&(**iterator));
            }
            AssignWeightedVertexAttribute(*vertexPtr, attributeKey,
                constAttributePossessorVector, weightVector);
        }
    }
}

void 
AssignInterpolatedFaceVertexAttributes(FacePtr facePtr, 
    VertexPtr vertexPtr, FacePtr vertexPtrVectorFacePtr,
    const std::vector<ConstVertexPtr> &vertexPtrVector)
{
    // Handle the case when there are no reference vertices.
    if (vertexPtrVector.empty()) {
        return;
    }
    
    // Get the mean value coordinate weights for the neighboring
    // vertices, based on their position relative to the
    // reference vertex.
    WeightVector weightVector;
    GetMeanValueCoordinateWeights(vertexPtrVector,
        vertexPtr, weightVector);

    // We only want to assign the attributes that all of the source
    // face vertices have in common. Only these attributes will be
    // assigned to the target face vertex. The mean value coordinate
    // weights assume that all of the vertices are represented.

    // Loop over all the attributes of the first face vertex, and make
    // sure that all the other face vertices have this same attribute.
    ConstVertexPtr firstVertexPtr = *(vertexPtrVector.begin());
    FaceVertex *firstFaceVertex 
        = vertexPtrVectorFacePtr->getFaceVertex(firstVertexPtr);

    for (AttributePossessor::const_iterator attributeDataIterator
             = firstFaceVertex->attributeDataBegin();
         attributeDataIterator != firstFaceVertex->attributeDataEnd();
         ++attributeDataIterator) {

        // Create an attribute key that corresponds to this attribute.
        const AttributeData &attributeData(*attributeDataIterator);
        const AttributeKey attributeKey(attributeData.handle(), 
            attributeData.type());

        // Check to see if all of the other vertices have the attribute.
        bool allFaceVerticesHaveAttribute = true;
        ConstVertexPtrVector::const_iterator iterator 
            = vertexPtrVector.begin();
        for (++iterator; iterator != vertexPtrVector.end(); ++iterator) {
            FaceVertex *faceVertex 
                = vertexPtrVectorFacePtr->getFaceVertex(*iterator);
            if (!faceVertex->hasAttribute(attributeKey)) {
                allFaceVerticesHaveAttribute = false;
                break;
            }
        }

        // If all of the face vertices have this attribute, 
        // compute the weighted average of the attributes of the
        // face vertices and assign them to the reference vertex.
        if (allFaceVerticesHaveAttribute) {
            FaceVertex *faceVertex = facePtr->getFaceVertex(vertexPtr);
            ConstAttributePossessorVector constAttributePossessorVector;
            for (ConstVertexPtrVector::const_iterator iterator
                     = vertexPtrVector.begin();
                 iterator != vertexPtrVector.end(); ++iterator) {
                constAttributePossessorVector.push_back(
                    vertexPtrVectorFacePtr->getFaceVertex(*iterator));
            }
            AssignWeightedVertexAttribute(*faceVertex, attributeKey,
                constAttributePossessorVector, weightVector);
        }
    }
}

void 
GetMeanValueCoordinateWeights(
    const ConstVertexPtrVector &vertexPtrVector,
    ConstVertexPtr vertexPtr, WeightVector &weightVector)
{
    // Compute an approprite value of epsilon for later floating point equality comparisons.
    float max = 0.0;
    for (ConstVertexPtrVector::const_iterator iterator = vertexPtrVector.begin();
         iterator != vertexPtrVector.end(); ++iterator) {
        max = std::max(max, (*iterator)->position().maxAbs());
    }
    max = std::max(max, vertexPtr->position().maxAbs());
    float epsilon = std::max(sqrtf(std::numeric_limits<float>::epsilon()),
        max*sqrtf(std::numeric_limits<float>::epsilon()));

    const Vector3f &vertex(vertexPtr->position());

    weightVector.clear();

    // First, check if the vertex is coincindent with any of
    // the vertices that define the contour. If that's the case,
    // create a weight vector directly for that specific vertex.
    // The code below for the general case would wind up
    // dividing by zero, which isn't so great.
    for (ConstVertexPtrVector::const_iterator iterator
             = vertexPtrVector.begin();
         iterator != vertexPtrVector.end(); ++iterator) {
        if (vertex.equivalent((*iterator)->position(), epsilon)) {
            for (ConstVertexPtrVector::const_iterator 
                     secondaryIterator = vertexPtrVector.begin();
                 secondaryIterator != vertexPtrVector.end(); 
                 ++secondaryIterator) {
                if (secondaryIterator == iterator) {
                    weightVector.push_back(1.0);
                } else {
                    weightVector.push_back(0.0);
                }
            }
            return;
        }
    }

    // Secondly, check if the vertex lies along any of the edges
    // of the polygon. If that's true, create weights directly based
    // on the distance of the vertex between the two edges.
    // If we didn't do this, the general-case code below would
    // wind up computing a tangent of 90 degrees (infinity),
    // and using that in its calculations.
    for (ConstVertexPtrVector::const_iterator iterator
             = vertexPtrVector.begin();
         iterator != vertexPtrVector.end(); ++iterator) {
        ConstVertexPtrVector::const_iterator 
            nextIterator = iterator;
        ++nextIterator;
        if (nextIterator == vertexPtrVector.end()) {
            nextIterator = vertexPtrVector.begin();
        }
        const Vector3f &p0((*iterator)->position());
        const Vector3f &p1((*nextIterator)->position());

        // The vector between the two vertices on the edge.
        Vector3f dp(p1 - p0);

        // The length of the edge.
        float edgeLength = dp.length();

        // If the edge is degenerate, skip it.
        if (dp == Vector3f::ZERO) {
            continue;
        }
        dp.normalize();

        float dot = (vertex - p0).dot(dp);

        // The nearest point on the edge.
        Vector3f nearest(p0 + dot*dp);

        // Check if the vertex is on an edge and handle that case.
        if (vertex.equivalent(nearest, epsilon)) {
            float t = dot/edgeLength;
            for (ConstVertexPtrVector::const_iterator secondaryIterator
                     = vertexPtrVector.begin();
                 secondaryIterator != vertexPtrVector.end(); 
                 ++secondaryIterator) {
                if (secondaryIterator == iterator) {
                    weightVector.push_back(1.0 - t);
                } else if (secondaryIterator == nextIterator) {
                    weightVector.push_back(t);
                } else {
                    weightVector.push_back(0.0);
                }
            }
            return;
        }
    }

    // Handle the general case, where the vertex lies inside
    // or outside the polygon.
    float totalWeight = 0.0;
    for (ConstVertexPtrVector::const_iterator i0
             = vertexPtrVector.begin();
         i0 != vertexPtrVector.end(); ++i0) {

        // Get an iterator for the next vertex.
        ConstVertexPtrVector::const_iterator i1 = i0;
        ++i1;
        if (i1 == vertexPtrVector.end()) {
            i1 = vertexPtrVector.begin();
        }

        // Get an iterator for the next vertex.
        ConstVertexPtrVector::const_iterator i2 = i1;
        ++i2;
        if (i2 == vertexPtrVector.end()) {
            i2 = vertexPtrVector.begin();
        }

        Vector3f v0 = (*i0)->position() - vertex;
        Vector3f v1 = (*i1)->position() - vertex;
        Vector3f v2 = (*i2)->position() - vertex;

        float weight = (GetHalfAngleTangent(v0, v1)
            + GetHalfAngleTangent(v1, v2))/v1.length();

        // The weight must not be NaN.
        assert(weight*0.0 == 0.0);

        weightVector.push_back(weight);

        totalWeight += weight;
    }

    for (WeightVector::iterator iterator = weightVector.begin();
         iterator != weightVector.end(); ++iterator) {
        *iterator /= totalWeight;
    }

    // The loop places the weight for the second vertex
    // at the beginning of the vector. To get the vertices
    // in the right order, we have to remove the weight at the
    // end of the vector and insert it at the beginning.
    weightVector.insert(weightVector.begin(), weightVector.back());
    weightVector.pop_back();
}

static float
GetHalfAngleTangent(Vector3f a, Vector3f b)
{
    a.normalize();
    b.normalize();
    float dot = a.dot(b);
    
    return sqrt(1.0 - dot*dot)/(1.0 + dot);
}

static 
void AssignWeightedVertexAttribute(AttributePossessor &attributePossessor,
    const AttributeKey attributeKey, 
    const ConstAttributePossessorVector &constAttributePossessorVector,
    const WeightVector &weightVector)
{
    // The number of vertices must match up with the number of weights.
    assert(constAttributePossessorVector.size() == weightVector.size());
    
    float floatValue = 0.0;
    Vector2f vector2fValue(Vector2f::ZERO);
    Vector3f vector3fValue(Vector3f::ZERO);
    Vector4f vector4fValue(Vector4f::ZERO);

    WeightVector::const_iterator weightIterator = weightVector.begin();

    switch (attributeKey.type()) {

    case AttributeKey::UNDEFINED:
        // This should never happen.
        abort();
        break;

    case AttributeKey::BOOL:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            floatValue += (*weightIterator)
                *(*iterator)->getBool(attributeKey);
            ++weightIterator;
        }
        attributePossessor.setBool(attributeKey, floatValue >= 0.5);
        break;

    case AttributeKey::INT:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            floatValue += (*weightIterator)
                *(*iterator)->getInt(attributeKey);
            ++weightIterator;
        }
        attributePossessor.setInt(attributeKey, (int) (floatValue + 0.5));
        break;

    case AttributeKey::FLOAT:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            floatValue += (*weightIterator)
                *(*iterator)->getFloat(attributeKey);
            ++weightIterator;
        }
        attributePossessor.setFloat(attributeKey, floatValue);
        break;

    case AttributeKey::VECTOR2F:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            vector2fValue += (*weightIterator)
                *(*iterator)->getVector2f(attributeKey);
            ++weightIterator;
        }
        attributePossessor.setVector2f(attributeKey, vector2fValue);
        break;

    case AttributeKey::VECTOR3F:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            vector3fValue += (*weightIterator)
                *(*iterator)->getVector3f(attributeKey);
            ++weightIterator;
        }
        attributePossessor.setVector3f(attributeKey, vector3fValue);
        break;

    case AttributeKey::VECTOR4F:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            vector4fValue += (*weightIterator)
                *(*iterator)->getVector4f(attributeKey);
            ++weightIterator;
        }
        attributePossessor.setVector4f(attributeKey, vector4fValue);
        break;

    case AttributeKey::MATRIX3F:
        // This case is not handled yet. I think this would
        // involve interpolating between quaternions.
        abort();
        break;

    case AttributeKey::MATRIX4F:
        // This case is not handled yet. I think this would
        // involve interpolating between quaternions.
        abort();
        break;

    case AttributeKey::STRING:
        // This case would be pretty meaningless. Maybe it could
        // just copy the string associated with the first vertex,
        // and assume they all have the same value.
        abort();
        break;

    case AttributeKey::UNIT_VECTOR3F:
        for (ConstAttributePossessorVector::const_iterator iterator 
                 = constAttributePossessorVector.begin();
             iterator != constAttributePossessorVector.end(); ++iterator) {
            vector3fValue += (*weightIterator)
                *(*iterator)->getUnitVector3f(attributeKey);
            ++weightIterator;
        }
        vector3fValue.normalize();
        attributePossessor.setUnitVector3f(attributeKey, 
            vector3fValue);
        break;

    default:
        // This attribute type is not handled yet.
        abort();
        break;
    }
}

} // namespace mesh
