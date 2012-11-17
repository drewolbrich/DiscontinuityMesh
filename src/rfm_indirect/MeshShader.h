// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_INDIRECT__MESH_SHADER__INCLUDED
#define RFM_INDIRECT__MESH_SHADER__INCLUDED

#include <vector>
#include <set>
#include <map>

#include <cgmath/Vector3f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/HemisphericalPointDistributor.h>
#include <mesh/Types.h>
#include <mesh/AttributeKey.h>
#include <mesh/MaterialTable.h>
#include <mesh/SplitEdgeTriangulator.h>
#include <meshisect/FaceIntersector.h>

#include "MeshShaderFaceListener.h"
#include "OutputIlluminationAssigner.h"

namespace mesh {
class Mesh;
}

// MeshShader
//
// Computes indirect illumination for a mesh.

class MeshShader
{
public:
    MeshShader();
    ~MeshShader();

    // The mesh to shade.
    void setMesh(mesh::Mesh *mesh);

    // The number of rays to fire from each vertex (actually, per unique normal per vertex).
    static const unsigned DEFAULT_SAMPLES_PER_VERTEX;
    void setSamplesPerVertex(unsigned samplesPerVertex);

    // Error tolerance for adaptive subdivision.
    static const float DEFAULT_ADAPTIVE_SUBDIVISION_ERROR_TOLERANCE;
    void setAdaptiveSubdivisionErrorTolerance(float adaptiveSubdivisionErrorTolerance);
    float adaptiveSubdivisionErrorTolerance() const;

    // Minimum edge length for adaptive subdivision.
    static const float DEFAULT_ADAPTIVE_SUBDIVISION_MINIMUM_EDGE_LENGTH;
    void setAdaptiveSubdivisionMinimumEdgeLength(float adaptiveSubdivisionMinimumEdgeLength);
    float adaptiveSubdivisionMinimumEdgeLength() const;

    // Maximum pass number for adaptive subdivision.
    static const int DEFAULT_ADAPTIVE_SUBDIVISION_MAXIMUM_PASS;
    void setAdaptiveSubdivisionMaximumPass(int adaptiveSubdivisionMaximumPass);
    int adaptiveSubdivisionMaximumPass() const;

    // The color of the sky.
    void setSkyColor(const cgmath::Vector3f &skyColor);
    const cgmath::Vector3f &skyColor() const;

    // Coefficient applied to the existing direct illumination in the discontinuity mesh.
    void setDirectIlluminationScale(float directIlluminationScale);
    float directIlluminationScale() const;

    // Diffuse coefficient, applied to the indirect illumination.
    void setDiffuseCoefficient(float diffuseCoefficient);
    float diffuseCoefficient() const;

    // Number of indirect illumination bounces.
    void setBounces(unsigned bounces);
    unsigned bounces() const;

    // Shade the mesh.
    void shadeMesh();

private:
    // Initialize the AABB tree of faces with the current mesh.
    void initializeFaceIntersector();

    // Shade the mesh faces.
    void shadeFaces();

    // Shade an individual face.
    void shadeFace(mesh::FacePtr facePtr);

    // Subdivide faces whose samples suggest discontinuous illumination.
    bool subdivideFaces();

    // Create a set of all the unique normals amongst the face vertices
    // adjacent to the specified vertex.
    typedef std::vector<cgmath::Vector3f> UniqueNormalVector;
    void createUniqueAdjacentFaceVertexNormalVectorFromVertex(mesh::VertexPtr vertexPtr,
        UniqueNormalVector *uniqueNormalVector);

    // Calculate the indirect illumination for a given point and normal.
    cgmath::Vector3f sampleIndirectIllumination(const cgmath::Vector3f &point,
        const cgmath::Vector3f &normal, mesh::FacePtr facePtr);

    // Create a 3x3 orientation matrix that points the Z axis in a particular direction.
    cgmath::Matrix3f getZAxisOrientationMatrix(const cgmath::Vector3f &zAxisDirection);

    // Compute the angle between the edges leading away from the vertex of a face,
    // in radians.
    float getFaceVertexAngle(mesh::FacePtr facePtr, mesh::VertexPtr vertexPtr);

    // Mark all faces as requiring shading.
    void resetShouldShadeFaces();

    void resetIndirectIllumination();
    void copyDirectIlluminationToInputIllumination();
    void resetSampledIllumination();
    void convertSampledIlluminationToOutputIllumination();
    void addOutputIlluminationToIndirectIllumination();
    void calculateColorAttributes();

    mesh::Mesh *mMesh;

    unsigned mSamplesPerVertex;

    mesh::AttributeKey mNormal3fAttributeKey;
    mesh::AttributeKey mColor3fAttributeKey;

    // Direct illumination, already stored in the mesh file.
    mesh::AttributeKey mIlluminatedColor3fAttributeKey;

    // Indirect illumination on face vertices.
    mesh::AttributeKey mIndirectIllumination3fAttributeKey;

    mesh::MaterialTable mMaterialTable;

    float mMeshBoundingBoxDiameter;

    meshisect::FaceIntersector mFaceIntersector;

    unsigned mTotalSamples;

    cgmath::HemisphericalPointDistributor mHemisphericalPointDistributor;

    float mAdaptiveSubdivisionErrorTolerance;
    float mAdaptiveSubdivisionMinimumEdgeLength;
    int mAdaptiveSubdivisionMaximumPass;
    cgmath::Vector3f mSkyColor;
    float mDirectIlluminationScale;
    float mDiffuseCoefficient;
    unsigned mBounces;

    mesh::AttributeKey mShouldShadeFaceAttributeKey;

    MeshShaderFaceListener mMeshShaderFaceListener;

    mesh::SplitEdgeTriangulator mSplitEdgeTriangulator;

    mesh::AttributeKey mInputIlluminationAttributeKey;
    mesh::AttributeKey mSampledIlluminationAttributeKey;
    mesh::AttributeKey mOutputIlluminationAttributeKey;

    unsigned mCurrentBounce;
};

#endif // RFM_INDIRECT__MESH_SHADER__INCLUDED
