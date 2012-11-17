// Copyright 2010 Retroactive Fiasco.

#include "OutputIlluminationAssigner.h"

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include <mesh/MeshOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/StandardAttributes.h>
#include <cgmath/Constants.h>

#include "FaceOperations.h"

OutputIlluminationAssigner::OutputIlluminationAssigner()
    : mMesh(NULL),
      mSampledIlluminationAttributeKey(),
      mOutputIlluminationAttributeKey(),
      mNormalEpsilon(-1.0),
      mVisitedFaceAttributeKey(),
      mNormal3fAttributeKey(),
      mRadiusAttributeKey(),
      mFaceSet(),
      mVertexSet(),
      mSampleAabbTreeObjectVector(),
      mSampleAabbTree(),
      mVertexAabbTree(),
      mFilterNormal(),
      mFilterTotalWeight(0.0),
      mFilterTotalIllumination(0.0, 0.0, 0.0)
{
}

OutputIlluminationAssigner::~OutputIlluminationAssigner()
{
}

void
OutputIlluminationAssigner::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;

    mVisitedFaceAttributeKey = mMesh->getAttributeKey(
        "__OutputIlluminationAssigner_visitedFace", mesh::AttributeKey::BOOL,
        mesh::AttributeKey::TEMPORARY);

    mNormal3fAttributeKey = mesh::GetNormal3fAttributeKey(*mMesh);

    mRadiusAttributeKey = mMesh->getAttributeKey(
        "__OutputIlluminationAssigner_radius", mesh::AttributeKey::FLOAT,
        mesh::AttributeKey::TEMPORARY);
}

mesh::Mesh *
OutputIlluminationAssigner::mesh() const
{
    return mMesh;
}

void
OutputIlluminationAssigner::setSampledIlluminationAttributeKey(
    const mesh::AttributeKey &sampledIlluminationAttributeKey)
{
    mSampledIlluminationAttributeKey = sampledIlluminationAttributeKey;
}

const mesh::AttributeKey &
OutputIlluminationAssigner::sampledIlluminationAttributeKey() const
{
    return mSampledIlluminationAttributeKey;
}

void
OutputIlluminationAssigner::setOutputIlluminationAttributeKey(
    const mesh::AttributeKey &outputIlluminationAttributeKey)
{
    mOutputIlluminationAttributeKey = outputIlluminationAttributeKey;
}

const mesh::AttributeKey &
OutputIlluminationAssigner::outputIlluminationAttributeKey() const
{
    return mOutputIlluminationAttributeKey;
}

void
OutputIlluminationAssigner::setNormalEpsilon(float normalEpsilon)
{
    mNormalEpsilon = normalEpsilon;
}

float
OutputIlluminationAssigner::normalEpsilon() const
{
    return mNormalEpsilon;
}

void
OutputIlluminationAssigner::shadeMesh()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin(); facePtr != mMesh->faceEnd(); ++facePtr) {
        if (!facePtr->getBool(mVisitedFaceAttributeKey)) {
            shadeContinuousRegionFromFace(facePtr);
        }
    }

    mesh::RemoveAttributeFromMeshAndAllElements(mMesh, mVisitedFaceAttributeKey);
    mesh::RemoveAttributeFromMeshAndAllElements(mMesh, mRadiusAttributeKey);
}

void
OutputIlluminationAssigner::shadeContinuousRegionFromFace(mesh::FacePtr facePtr)
{
    mFaceSet.clear();
    mVertexSet.clear();
    mSampleAabbTreeObjectVector.clear();

    buildContinousRegion(facePtr);

    mSampleAabbTree.initialize(mSampleAabbTreeObjectVector);

    initializeVertexAabbTree();

    for (VertexSet::iterator iterator = mVertexSet.begin();
         iterator != mVertexSet.end(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        vertexPtr->setFloat(mRadiusAttributeKey, 0.0);
    }

    for (VertexSet::iterator iterator = mVertexSet.begin();
         iterator != mVertexSet.end(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;

        float maxDistance = 0.0;

        for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
             iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
            mesh::FacePtr facePtr = *iterator;
            if (mFaceSet.find(facePtr) == mFaceSet.end()) {
                continue;
            }

            for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
                 iterator != facePtr->adjacentVertexEnd(); ++iterator) {
                mesh::VertexPtr adjacentVertexPtr = *iterator;
                if (adjacentVertexPtr == vertexPtr) {
                    continue;
                }

                float distance = (adjacentVertexPtr->position() - vertexPtr->position()).length();
                if (distance > maxDistance) {
                    maxDistance = distance;
                }
            }
        }

        applyRadiusToVerticesInSphere(vertexPtr->position(), maxDistance);
    }

    for (VertexSet::iterator iterator = mVertexSet.begin();
         iterator != mVertexSet.end(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;

        for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
             iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
            mesh::FacePtr facePtr = *iterator;
            if (mFaceSet.find(facePtr) == mFaceSet.end()) {
                continue;
            }

            cgmath::Vector3f filteredIndirectIllumination
                = getFilteredIndirectIllumination(vertexPtr->position(),
                    facePtr->getVertexUnitVector3f(vertexPtr, mNormal3fAttributeKey),
                    vertexPtr->getFloat(mRadiusAttributeKey));

            facePtr->setVertexVector3f(vertexPtr, mOutputIlluminationAttributeKey,
                filteredIndirectIllumination);
        }
    }
}

void
OutputIlluminationAssigner::buildContinousRegion(mesh::FacePtr facePtr)
{
    assert(!facePtr->getBool(mVisitedFaceAttributeKey));

    facePtr->setBool(mVisitedFaceAttributeKey, true);

    mFaceSet.insert(facePtr);

    addSamplesFromFace(facePtr);

    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;

        mVertexSet.insert(vertexPtr);

        cgmath::Vector3f normal = facePtr->getVertexUnitVector3f(vertexPtr, 
            mNormal3fAttributeKey);

        for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
             iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
            mesh::FacePtr neighboringFacePtr = *iterator;
            if (neighboringFacePtr == facePtr) {
                continue;
            }

            if (neighboringFacePtr->getBool(mVisitedFaceAttributeKey)) {
                // We've already visited this face.
                continue;
            }

            cgmath::Vector3f neighboringNormal = neighboringFacePtr->getVertexUnitVector3f(
                vertexPtr, mNormal3fAttributeKey);

            assert(mNormalEpsilon >= 0.0);
            if (normal.equivalent(neighboringNormal, mNormalEpsilon)) {
                buildContinousRegion(neighboringFacePtr);
            }
        }
    }
}

void
OutputIlluminationAssigner::addSamplesFromFace(mesh::FacePtr facePtr)
{
    float faceArea = mesh::GetFaceArea(facePtr);

    Sample sample;
    sample.setPosition(mesh::GetFaceAverageVertexPosition(facePtr));
    sample.setNormal(mesh::GetFaceGeometricNormal(facePtr));
    sample.setIllumination(facePtr->getVector3f(mSampledIlluminationAttributeKey));
    sample.setFaceArea(faceArea);
    mSampleAabbTreeObjectVector.push_back(sample);

    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;

        Sample sample;
        sample.setPosition(GetFaceVertexSamplePosition(facePtr, vertexPtr));
        sample.setNormal(mesh::GetFaceGeometricNormal(facePtr));
        sample.setIllumination(facePtr->getVertexVector3f(vertexPtr, 
                mSampledIlluminationAttributeKey));
        sample.setFaceArea(faceArea);
        mSampleAabbTreeObjectVector.push_back(sample);
    }
}

void
OutputIlluminationAssigner::initializeVertexAabbTree()
{
    VertexAabbTree::ObjectVector vertexAabbTreeNodeVector;
    for (VertexSet::iterator iterator = mVertexSet.begin();
         iterator != mVertexSet.end(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;
        VertexAabbTreeNode vertexAabbTreeNode;
        vertexAabbTreeNode.setVertexPtr(vertexPtr);
        vertexAabbTreeNodeVector.push_back(vertexAabbTreeNode);
    }

    mVertexAabbTree.initialize(vertexAabbTreeNodeVector);
}

void
OutputIlluminationAssigner::applyRadiusToVerticesInSphere(const cgmath::Vector3f &center, 
    float radius)
{
    cgmath::BoundingBox3f boundingBox(center - cgmath::Vector3f::UNIT*radius,
        center + cgmath::Vector3f::UNIT*radius);
    mVertexAabbTree.applyToBoundingBoxIntersection(boundingBox, this);

    // Continued in applyObjectToBoundingBox...
}

bool
OutputIlluminationAssigner::applyObjectToBoundingBox(VertexAabbTreeNode &vertexAabbTreeNode,
    const cgmath::BoundingBox3f &boundingBox)
{
    mesh::VertexPtr vertexPtr = vertexAabbTreeNode.vertexPtr();
    float sphereRadius = boundingBox.sizeX()*0.5;

    if ((boundingBox.center() - vertexPtr->position()).length() <= sphereRadius) {
        if (sphereRadius > vertexPtr->getFloat(mRadiusAttributeKey)) {
            vertexPtr->setFloat(mRadiusAttributeKey, sphereRadius);
        }
    }

    // Keep testing all other vertices.
    return false;
}

cgmath::Vector3f
OutputIlluminationAssigner::getFilteredIndirectIllumination(const cgmath::Vector3f &position,
    const cgmath::Vector3f &normal, float radius)
{
    mFilterNormal = normal;
    mFilterTotalWeight = 0.0;
    mFilterTotalIllumination = cgmath::Vector3f(0.0, 0.0, 0.0);

    cgmath::BoundingBox3f boundingBox(position - cgmath::Vector3f::UNIT*radius,
        position + cgmath::Vector3f::UNIT*radius);

    mSampleAabbTree.applyToBoundingBoxIntersection(boundingBox, this);

    // Continued in applyObjectToBoundingBox...

    if (mFilterTotalWeight == 0.0) {
        return cgmath::Vector3f(0.0, 0.0, 0.0);
    }

    return mFilterTotalIllumination/mFilterTotalWeight;
}

bool 
OutputIlluminationAssigner::applyObjectToBoundingBox(Sample &sample,
    const cgmath::BoundingBox3f &boundingBox)
{
    float sphereRadius = boundingBox.sizeX()*0.5;

    float distance = (boundingBox.center() - sample.position()).length();

    if (distance < sphereRadius) {

        float dot = mFilterNormal.dot(sample.normal());
        if (dot < 0.0) {
            dot = 0.0;
        }

        float weight = dot;

        static const float FILTER_RADIUS = 0.25;

        weight *= gaussian(distance/sphereRadius/FILTER_RADIUS);

        weight *= sample.faceArea();
        
        mFilterTotalWeight += weight;
        mFilterTotalIllumination += weight*sample.illumination();
    }

    // Keep testing all other samples.
    return false;
}

float
OutputIlluminationAssigner::gaussian(float x)
{
    return 1.0/sqrtf(2.0*cgmath::PI)*powf(cgmath::E, -(x*x)/2.0);
}
