// Copyright 2010 Drew Olbrich

#ifndef RFM_INDIRECT__OUTPUT_ILLUMINATION_ASSIGNER__INCLUDED
#define RFM_INDIRECT__OUTPUT_ILLUMINATION_ASSIGNER__INCLUDED

#include <set>

#include <mesh/Mesh.h>
#include <mesh/Types.h>
#include <mesh/AttributeKey.h>

#include "VertexAabbTree.h"
#include "SampleAabbTree.h"

// OutputIlluminationAssigner
//
// Computes the output illumination channel for MeshShader,
// based on the sampled illumination.

class OutputIlluminationAssigner 
    : public VertexAabbTree::BoundingBoxListener,
        public SampleAabbTree::BoundingBoxListener
{
public:
    OutputIlluminationAssigner();
    ~OutputIlluminationAssigner();

    // The mesh to compute the output illumination for.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Sampled illumination attribute key.
    void setSampledIlluminationAttributeKey(
        const mesh::AttributeKey &sampledIlluminationAttributeKey);
    const mesh::AttributeKey &sampledIlluminationAttributeKey() const;

    // Output illumination attribute key.
    void setOutputIlluminationAttributeKey(
        const mesh::AttributeKey &outputIlluminationAttributeKey);
    const mesh::AttributeKey &outputIlluminationAttributeKey() const;

    // Epsilon used to determine which normals are equivalent.
    void setNormalEpsilon(float normalEpsilon);
    float normalEpsilon() const;

    // Compute the output illumination.
    void shadeMesh();

private:
    // Shade a region with continuous normals.
    void shadeContinuousRegionFromFace(mesh::FacePtr facePtr);

    void buildContinousRegion(mesh::FacePtr facePtr);

    void addSamplesFromFace(mesh::FacePtr facePtr);

    void initializeVertexAabbTree();

    void applyRadiusToVerticesInSphere(const cgmath::Vector3f &center, float radius);

    // For VertexAabbTree:
    virtual bool applyObjectToBoundingBox(VertexAabbTreeNode &vertexAabbTreeNode,
        const cgmath::BoundingBox3f &boundingBox);

    cgmath::Vector3f getFilteredIndirectIllumination(const cgmath::Vector3f &position,
        const cgmath::Vector3f &normal, float radius);

    // For SampleAabbTree:
    virtual bool applyObjectToBoundingBox(Sample &sample, 
        const cgmath::BoundingBox3f &boundingBox);

    float gaussian(float x);

    mesh::Mesh *mMesh;
    mesh::AttributeKey mSampledIlluminationAttributeKey;
    mesh::AttributeKey mOutputIlluminationAttributeKey;

    float mNormalEpsilon;

    mesh::AttributeKey mVisitedFaceAttributeKey;
    mesh::AttributeKey mNormal3fAttributeKey;
    mesh::AttributeKey mRadiusAttributeKey;

    typedef std::set<mesh::FacePtr> FaceSet;
    FaceSet mFaceSet;

    typedef std::set<mesh::VertexPtr> VertexSet;
    VertexSet mVertexSet;

    SampleAabbTree::ObjectVector mSampleAabbTreeObjectVector;
    SampleAabbTree mSampleAabbTree;

    VertexAabbTree mVertexAabbTree;

    cgmath::Vector3f mFilterNormal;
    float mFilterTotalWeight;
    cgmath::Vector3f mFilterTotalIllumination;
};

#endif // RFM_INDIRECT__OUTPUT_ILLUMINATION_ASSIGNER__INCLUDED
