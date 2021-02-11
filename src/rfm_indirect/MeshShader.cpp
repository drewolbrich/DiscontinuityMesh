// Copyright 2009 Drew Olbrich

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <set>
#include <algorithm>

#include <con/Streams.h>
#include <mesh/Types.h>
#include <mesh/Mesh.h>
#include <mesh/StandardAttributes.h>
#include <mesh/MeshOperations.h>
#include <mesh/FaceOperations.h>
#include <mesh/EdgeOperations.h>
#include <cgmath/Vector3f.h>
#include <cgmath/Matrix3f.h>
#include <cgmath/ColorOperations.h>
#include <cgmath/BarycentricCoordinates.h>

#include "MeshShader.h"
#include "FaceOperations.h"

// This must be a perfect square.
const unsigned MeshShader::DEFAULT_SAMPLES_PER_VERTEX = 100;

// Default error tolernace for adaptive subdivision.
// (This is a multiple of a power of two so that it prints out nicely in the usage message.)
const float MeshShader::DEFAULT_ADAPTIVE_SUBDIVISION_ERROR_TOLERANCE = 0.03125;

// Default minimum edge length for adaptive subdivision.
// (This is a multiple of a power of two so that it prints out nicely in the usage message.)
const float MeshShader::DEFAULT_ADAPTIVE_SUBDIVISION_MINIMUM_EDGE_LENGTH = 0.5;

// Default maximum pass number for adaptive subdivision.
const int MeshShader::DEFAULT_ADAPTIVE_SUBDIVISION_MAXIMUM_PASS = 1;

// Used to determine which adjacent face vertex normals are equivalent.
static const float NORMAL_EPSILON = 0.001;

MeshShader::MeshShader()
    : mMesh(NULL),
      mSamplesPerVertex(DEFAULT_SAMPLES_PER_VERTEX),
      mNormal3fAttributeKey(),
      mColor3fAttributeKey(),
      mIlluminatedColor3fAttributeKey(),
      mIndirectIllumination3fAttributeKey(),
      mMaterialTable(),
      mMeshBoundingBoxDiameter(0.0),
      mFaceIntersector(),
      mTotalSamples(0),
      mHemisphericalPointDistributor(),
      mAdaptiveSubdivisionErrorTolerance(DEFAULT_ADAPTIVE_SUBDIVISION_ERROR_TOLERANCE),
      mAdaptiveSubdivisionMinimumEdgeLength(DEFAULT_ADAPTIVE_SUBDIVISION_MINIMUM_EDGE_LENGTH),
      mAdaptiveSubdivisionMaximumPass(DEFAULT_ADAPTIVE_SUBDIVISION_MINIMUM_EDGE_LENGTH),
      mSkyColor(1, 1, 1),
      mDirectIlluminationScale(0.7),
      mDiffuseCoefficient(0.3),
      mBounces(1),
      mShouldShadeFaceAttributeKey(),
      mMeshShaderFaceListener(),
      mSplitEdgeTriangulator(),
      mInputIlluminationAttributeKey(),
      mSampledIlluminationAttributeKey(),
      mOutputIlluminationAttributeKey(),
      mCurrentBounce(0)
{
}

MeshShader::~MeshShader()
{
}

void
MeshShader::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;

    mNormal3fAttributeKey = mesh::GetNormal3fAttributeKey(*mMesh);
    mColor3fAttributeKey = mesh::GetColor3fAttributeKey(*mMesh);

    mIlluminatedColor3fAttributeKey = mMesh->getAttributeKey(
        "illuminatedColor3f", mesh::AttributeKey::VECTOR3F);
    mIndirectIllumination3fAttributeKey = mMesh->getAttributeKey(
        "indirectIllumination3f", mesh::AttributeKey::VECTOR3F);

    mShouldShadeFaceAttributeKey = mMesh->getAttributeKey(
        "__MeshShader_shouldShadeFace", mesh::AttributeKey::BOOL,
        mesh::AttributeKey::TEMPORARY);

    mInputIlluminationAttributeKey = mMesh->getAttributeKey(
        "__MeshShader_inputIllumination", mesh::AttributeKey::VECTOR3F,
        mesh::AttributeKey::TEMPORARY);
    mSampledIlluminationAttributeKey = mMesh->getAttributeKey(
        "__MeshShader_sampledIllumination", mesh::AttributeKey::VECTOR3F,
        mesh::AttributeKey::TEMPORARY);
    mOutputIlluminationAttributeKey = mMesh->getAttributeKey(
        "__MeshShader_outputIllumination", mesh::AttributeKey::VECTOR3F,
        mesh::AttributeKey::TEMPORARY);

    cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(*mMesh);
    mMeshBoundingBoxDiameter = (bbox.max() - bbox.min()).length();

    mMaterialTable.initialize(*mesh);
}

void
MeshShader::setSamplesPerVertex(unsigned samplesPerVertex)
{
    // HemisphericalPointDistributor expects that the number of samples
    // must be a perfect square, so we round to the nearest perfect square.
    mSamplesPerVertex = int(powf(rint(sqrtf(samplesPerVertex)), 2.0));
}

void
MeshShader::setAdaptiveSubdivisionErrorTolerance(float adaptiveSubdivisionErrorTolerance)
{
    mAdaptiveSubdivisionErrorTolerance = adaptiveSubdivisionErrorTolerance;
}

float
MeshShader::adaptiveSubdivisionErrorTolerance() const
{
    return mAdaptiveSubdivisionErrorTolerance;
}

void
MeshShader::setAdaptiveSubdivisionMinimumEdgeLength(float adaptiveSubdivisionMinimumEdgeLength)
{
    mAdaptiveSubdivisionMinimumEdgeLength = adaptiveSubdivisionMinimumEdgeLength;
}

float
MeshShader::adaptiveSubdivisionMinimumEdgeLength() const
{
    return mAdaptiveSubdivisionMinimumEdgeLength;
}

void
MeshShader::setAdaptiveSubdivisionMaximumPass(int adaptiveSubdivisionMaximumPass)
{
    mAdaptiveSubdivisionMaximumPass = adaptiveSubdivisionMaximumPass;
}

int
MeshShader::adaptiveSubdivisionMaximumPass() const
{
    return mAdaptiveSubdivisionMaximumPass;
}

void
MeshShader::setSkyColor(const cgmath::Vector3f &skyColor)
{
    mSkyColor = skyColor;
}

const cgmath::Vector3f &
MeshShader::skyColor() const
{
    return mSkyColor;
}

void
MeshShader::setDirectIlluminationScale(float directIlluminationScale)
{
    mDirectIlluminationScale = directIlluminationScale;
}

float
MeshShader::directIlluminationScale() const
{
    return mDirectIlluminationScale;
}

void
MeshShader::setDiffuseCoefficient(float diffuseCoefficient)
{
    mDiffuseCoefficient = diffuseCoefficient;
}

float
MeshShader::diffuseCoefficient() const
{
    return mDiffuseCoefficient;
}

void
MeshShader::setBounces(unsigned bounces)
{
    mBounces = bounces;
}

unsigned
MeshShader::bounces() const
{
    return mBounces;
}

void
MeshShader::shadeMesh()
{
    mHemisphericalPointDistributor.setPointCount(mSamplesPerVertex);
    mHemisphericalPointDistributor.initialize();

    mSplitEdgeTriangulator.setMesh(mMesh);
    mSplitEdgeTriangulator.initialize();

    mTotalSamples = 0;

    con::info << "Samples per unique normal per vertex: " 
        << mHemisphericalPointDistributor.pointCount() << std::endl;

    if (mAdaptiveSubdivisionMaximumPass > 1) {
        con::info << "Adaptive subdivision error tolerance: "
            << mAdaptiveSubdivisionErrorTolerance << std::endl;
        con::info << "Adaptive subdivision minimum edge length: "
            << mAdaptiveSubdivisionMinimumEdgeLength << std::endl;
    }

    resetIndirectIllumination();

    copyDirectIlluminationToInputIllumination();

    for (mCurrentBounce = 1; mCurrentBounce <= mBounces; ++mCurrentBounce) {

        con::info << "Bounce " << mCurrentBounce << "." << std::endl;

        resetSampledIllumination();

        resetShouldShadeFaces();

        int adaptiveSubdivisionPass = 1;
        do {

            if (mCurrentBounce == 1
                && mAdaptiveSubdivisionMaximumPass > 1) {
                con::info << "Adaptive subdivision pass "
                    << adaptiveSubdivisionPass << "." << std::endl;
            }

            initializeFaceIntersector();

            shadeFaces();

            convertSampledIlluminationToOutputIllumination();
        
            ++adaptiveSubdivisionPass;

            if (mCurrentBounce > 1
                || adaptiveSubdivisionPass > mAdaptiveSubdivisionMaximumPass) {
                break;
            }

        } while (subdivideFaces());

        addOutputIlluminationToIndirectIllumination();
    }

    calculateColorAttributes();

    con::info << "Total samples: " << mTotalSamples << std::endl;
}

void
MeshShader::initializeFaceIntersector()
{
    mFaceIntersector.setMesh(mMesh);
    mFaceIntersector.setIntersectorFaceListener(&mMeshShaderFaceListener);
    mFaceIntersector.initialize();
}

void
MeshShader::shadeFaces()
{
    size_t unshadedFaceCount = 0;
    for (mesh::FacePtr facePtr = mMesh->faceBegin(); 
         facePtr != mMesh->faceEnd(); ++facePtr) {
        if (facePtr->getBool(mShouldShadeFaceAttributeKey)) {
            ++unshadedFaceCount;
        }
    }

    if (unshadedFaceCount == mMesh->faceCount()) {
        con::info << "Shading " << unshadedFaceCount
            << " faces." << std::endl;
    } else {
        con::info << "Shading " << unshadedFaceCount
            << " of " << mMesh->faceCount() << " faces." << std::endl;
    }

    for (mesh::FacePtr facePtr = mMesh->faceBegin(); 
         facePtr != mMesh->faceEnd(); ++facePtr) {

        if (!facePtr->getBool(mShouldShadeFaceAttributeKey)) {
            // We already shaded this face.
            continue;
        }

        shadeFace(facePtr);
    }
}

void
MeshShader::shadeFace(mesh::FacePtr facePtr)
{
    // Sample the midpoint of the face.
    cgmath::Vector3f position = mesh::GetFaceAverageVertexPosition(facePtr);
    cgmath::Vector3f normal = mesh::GetFaceGeometricNormal(facePtr);

    cgmath::Vector3f indirectIllumination = sampleIndirectIllumination(
        position, normal, facePtr);

    facePtr->setVector3f(mSampledIlluminationAttributeKey, indirectIllumination);

    // Sample points near the vertices of the face.
    for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
         iterator != facePtr->adjacentVertexEnd(); ++iterator) {
        mesh::VertexPtr vertexPtr = *iterator;

        mesh::VertexPtr nextVertexPtr;
        mesh::VertexPtr previousVertexPtr;
        GetFaceOtherVertices(facePtr, vertexPtr, &nextVertexPtr, &previousVertexPtr);

        cgmath::Vector3f nextEdgeMidpoint = (vertexPtr->position() 
            + nextVertexPtr->position())*0.5;
        cgmath::Vector3f previousEdgeMidpoint = (vertexPtr->position() 
            + previousVertexPtr->position())*0.5;

        cgmath::Vector3f position = (vertexPtr->position()
            + nextEdgeMidpoint + previousEdgeMidpoint)/3.0;

        cgmath::Vector3f indirectIllumination = sampleIndirectIllumination(
            position, normal, facePtr);

        facePtr->setVertexVector3f(vertexPtr, mSampledIlluminationAttributeKey, 
            indirectIllumination);
    }

    facePtr->setBool(mShouldShadeFaceAttributeKey, false);
}

bool
MeshShader::subdivideFaces()
{
    con::info << "Subdividing faces." << std::endl;

    // Set of edges to split.
    typedef std::set<mesh::EdgePtr> SplitEdgePtrSet;
    SplitEdgePtrSet splitEdgePtrSet;

    unsigned splitFaceCount = 0;

    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        // The illumination at the center of the face.
        cgmath::Vector3f faceIllumination = facePtr->getVector3f(
            mSampledIlluminationAttributeKey);

        // Compute the average illumination of the face vertices.
        cgmath::Vector3f faceVertexAverageIllumination(0, 0, 0);
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            faceVertexAverageIllumination += facePtr->getVertexVector3f(
                vertexPtr, mOutputIlluminationAttributeKey);
        }
        faceVertexAverageIllumination /= 3.0;

        float faceLuminance = cgmath::GammaColorToLuminance(
            faceIllumination);
        float faceVertexAverageLuminance = cgmath::GammaColorToLuminance(
            faceVertexAverageIllumination);

        bool foundEdgeToSplit = false;
        if (fabs(faceLuminance - faceVertexAverageLuminance) 
            > mAdaptiveSubdivisionErrorTolerance) {

            for (mesh::AdjacentEdgeIterator iterator = facePtr->adjacentEdgeBegin();
                 iterator != facePtr->adjacentEdgeEnd(); ++iterator) {
                mesh::EdgePtr edgePtr = *iterator;

                if (mesh::GetEdgeLength(edgePtr) > mAdaptiveSubdivisionMinimumEdgeLength) {
                    // We'll split this edge.
                    splitEdgePtrSet.insert(edgePtr);
                    foundEdgeToSplit = true;
                }
            }
        }
        if (foundEdgeToSplit) {
            facePtr->setBool(mShouldShadeFaceAttributeKey, true);
            ++splitFaceCount;
        }
    }

    if (!splitEdgePtrSet.empty()) {

        // Split the edges.
        for (SplitEdgePtrSet::const_iterator iterator = splitEdgePtrSet.begin();
             iterator != splitEdgePtrSet.end(); ++iterator) {
            mesh::EdgePtr edgePtr = *iterator;

            mesh::VertexPtr v0;
            mesh::VertexPtr v1;
            mesh::GetEdgeAdjacentVertices(edgePtr, &v0, &v1);
            cgmath::Vector3f midpoint = (v0->position() + v1->position())*0.5;
            mesh::VertexPtr newVertexPtr;
            mesh::SplitEdge(mMesh, edgePtr, midpoint, &newVertexPtr, NULL);

            // Mark the vertex as having been created in the middle of a split edge,
            // which is required by SplitEdgeTriangulator.
            newVertexPtr->setBool(mSplitEdgeTriangulator.splitEdgeVertexAttributeKey(), true);
        }

        // Retriangulate the mesh.
        mSplitEdgeTriangulator.triangulate();
    }

    con::info << "Subdivided " << splitFaceCount << " faces." << std::endl;
    con::info << "Split " << splitEdgePtrSet.size() << " edges." << std::endl;

    return !splitEdgePtrSet.empty();
}

void
MeshShader::createUniqueAdjacentFaceVertexNormalVectorFromVertex(mesh::VertexPtr vertexPtr,
    UniqueNormalVector *uniqueNormalVector)
{
    assert(mNormal3fAttributeKey.isDefined());

    for (mesh::AdjacentFaceIterator iterator = vertexPtr->adjacentFaceBegin();
         iterator != vertexPtr->adjacentFaceEnd(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;
        if (facePtr->hasVertexAttribute(vertexPtr, mNormal3fAttributeKey)) {
            cgmath::Vector3f normal = facePtr->getVertexUnitVector3f(vertexPtr, 
                mNormal3fAttributeKey);

            for (UniqueNormalVector::const_iterator iterator = uniqueNormalVector->begin();
                 iterator != uniqueNormalVector->end(); ++iterator) {
                const cgmath::Vector3f &knownNormal = *iterator;
                if (knownNormal.equivalent(normal, NORMAL_EPSILON)) {
                    // We've already seen this normal before.
                    goto nextFaceVertex;
                }
            }

            // We haven't seen this normal before.
            uniqueNormalVector->push_back(normal);
        }

    nextFaceVertex:
        ;
    }
}

cgmath::Vector3f
MeshShader::sampleIndirectIllumination(const cgmath::Vector3f &point,
    const cgmath::Vector3f &normal, mesh::FacePtr facePtr)
{
    // HemisphericalPointDistributor creates a hemisphere pointing
    // in the direction of the Z axis. To orient the hemisphere in the right direction,
    // we need a matrix that takes the Z axis and points it in the direction 
    // of our normal vector.
    cgmath::Matrix3f hemisphereOrientation = getZAxisOrientationMatrix(normal);

    cgmath::Vector3f totalIllumination(0, 0, 0);
    for (unsigned sample = 0; sample < mSamplesPerVertex; ++sample) {

        ++mTotalSamples;
        
        // The point of intersection.
        cgmath::Vector3f intersectionPoint;

        // The face that is intersected by the ray.
        mesh::FacePtr intersectedFacePtr;

        cgmath::Vector3f direction = hemisphereOrientation
            *mHemisphericalPointDistributor.getPoint(sample);

        cgmath::Vector3f endpoint = point + direction*mMeshBoundingBoxDiameter;

        mMeshShaderFaceListener.setFacePtrToIgnore(facePtr);

        if (mFaceIntersector.intersectsRaySegment(point, endpoint,
                &intersectionPoint, &intersectedFacePtr)) {

            // The ray intersects scene geometry. We sample the direct illumination
            // assumed to be already encoded in the discontinuity mesh.

            cgmath::Vector3f p0;
            cgmath::Vector3f p1;
            cgmath::Vector3f p2;
            mesh::GetTriangularFaceVertexPositions(intersectedFacePtr, &p0, &p1, &p2);

            cgmath::Vector3f barycentricCoordinates 
                = cgmath::GetBarycentricCoordinatesOfPointOnTriangle3f(intersectionPoint, 
                    p0, p1, p2);

            mesh::VertexPtr v0;
            mesh::VertexPtr v1;
            mesh::VertexPtr v2;
            mesh::GetTriangularFaceAdjacentVertices(intersectedFacePtr, &v0, &v1, &v2);

            assert(intersectedFacePtr->hasVertexAttribute(v0, mInputIlluminationAttributeKey));
            assert(intersectedFacePtr->hasVertexAttribute(v1, mInputIlluminationAttributeKey));
            assert(intersectedFacePtr->hasVertexAttribute(v2, mInputIlluminationAttributeKey));

            cgmath::Vector3f direct0 = intersectedFacePtr->getVertexVector3f(
                v0, mInputIlluminationAttributeKey);
            cgmath::Vector3f direct1 = intersectedFacePtr->getVertexVector3f(
                v1, mInputIlluminationAttributeKey);
            cgmath::Vector3f direct2 = intersectedFacePtr->getVertexVector3f(
                v2, mInputIlluminationAttributeKey);

            cgmath::Vector3f directIllumination
                = barycentricCoordinates[0]*direct0
                + barycentricCoordinates[1]*direct1
                + barycentricCoordinates[2]*direct2;

            totalIllumination += directIllumination;

        } else {

            // Ray intersects the sky.
            if (mCurrentBounce == 1) {
                // Only receive illumination from the sky on the first bounce.
                totalIllumination += mSkyColor;
            }
        }
    }

    cgmath::Vector3f illumination = totalIllumination/mSamplesPerVertex;

    // TODO: The following calculation does not yet incorporate
    // diffuse face colors, or face vertex colors defined in the mesh.
    // Face vertex colors are a problem because rfm_discmesh overwrites them
    // with the calculated direct illumination. Once it's modified
    // not to do that, we can call mesh::MaterialTable::getFaceVertexDiffuseColor
    // to sample the direct illumination at each face vertex,
    // and use barycentric coordinates to compute the composite diffuse color
    // at a point on the face.

    illumination *= mDiffuseCoefficient;

    illumination *= cgmath::Vector3f(mMaterialTable.getMaterialFromFace(facePtr).mDiffuse);

    return illumination;
}

cgmath::Matrix3f 
MeshShader::getZAxisOrientationMatrix(const cgmath::Vector3f &zAxisDirection)
{
    cgmath::Vector3f z = zAxisDirection.normalized();
    cgmath::Vector3f x;
    if (fabsf(z[0]) > fabsf(z[1]) && fabsf(z[0]) > fabsf(z[2])) {
        x = cgmath::Vector3f(0, 1, 0);
    } else {
        x = cgmath::Vector3f(1, 0, 0);
    }
    cgmath::Vector3f y = z.cross(x).normalized();
    x = y.cross(z).normalized();

    return cgmath::Matrix3f(x, y, z);
}

float
MeshShader::getFaceVertexAngle(mesh::FacePtr facePtr, mesh::VertexPtr vertexPtr)
{
    mesh::VertexPtr nextVertexPtr;
    mesh::VertexPtr previousVertexPtr;
    GetFaceOtherVertices(facePtr, vertexPtr, &nextVertexPtr, &previousVertexPtr);

    cgmath::Vector3f nextVector = nextVertexPtr->position() - vertexPtr->position();
    cgmath::Vector3f previousVector = previousVertexPtr->position() - vertexPtr->position();

    nextVector.normalize();
    previousVector.normalize();

    float dotProduct = nextVector.dot(previousVector);

    // For nearly degenerate faces, it's possible that the dot product
    // will be slightly outside the -1.0 to 1.0 range, in which case
    // acosf would return NaN, so we must correct for that.
    if (dotProduct > 1.0) {
        dotProduct = 1.0;
    } else if (dotProduct < -1.0) {
        dotProduct = -1.0;
    }

    return acosf(dotProduct);
}

void
MeshShader::resetShouldShadeFaces()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {
        facePtr->setBool(mShouldShadeFaceAttributeKey, true);
    }
}

void
MeshShader::resetIndirectIllumination()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;

            facePtr->setVertexVector3f(vertexPtr, mIndirectIllumination3fAttributeKey, 
                cgmath::Vector3f(0, 0, 0));
        }
    }
}

void
MeshShader::copyDirectIlluminationToInputIllumination()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;

            cgmath::Vector3f directIllumination = facePtr->getVertexVector3f(vertexPtr, 
                mIlluminatedColor3fAttributeKey);
            facePtr->setVertexVector3f(vertexPtr, mInputIlluminationAttributeKey,
                directIllumination);
        }
    }
}

void
MeshShader::resetSampledIllumination()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        facePtr->setVector3f(mSampledIlluminationAttributeKey, 
            cgmath::Vector3f(0, 0, 0));

        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;

            facePtr->setVertexVector3f(vertexPtr, mSampledIlluminationAttributeKey,
                cgmath::Vector3f(0, 0, 0));
        }
    }
}

void
MeshShader::convertSampledIlluminationToOutputIllumination()
{
    OutputIlluminationAssigner outputIlluminationAssigner;
    outputIlluminationAssigner.setMesh(mMesh);
    outputIlluminationAssigner.setSampledIlluminationAttributeKey(
        mSampledIlluminationAttributeKey);
    outputIlluminationAssigner.setOutputIlluminationAttributeKey(
        mOutputIlluminationAttributeKey);
    outputIlluminationAssigner.setNormalEpsilon(NORMAL_EPSILON);
    outputIlluminationAssigner.shadeMesh();
}

void
MeshShader::addOutputIlluminationToIndirectIllumination()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;

            // Add the output illumination to the indirect illumination.
            cgmath::Vector3f oldIndirectIllumination 
                = facePtr->getVertexVector3f(vertexPtr, mIndirectIllumination3fAttributeKey);
            cgmath::Vector3f outputIllumination
                = facePtr->getVertexVector3f(vertexPtr, mOutputIlluminationAttributeKey);
            facePtr->setVertexVector3f(vertexPtr, mIndirectIllumination3fAttributeKey,
                oldIndirectIllumination + outputIllumination);

            // Copy the output illumination to the input illumination
            // so it can be used as input for the next bounce.
            facePtr->setVertexVector3f(vertexPtr, mInputIlluminationAttributeKey,
                facePtr->getVertexVector3f(vertexPtr, mOutputIlluminationAttributeKey));
        }
    }
}

void
MeshShader::calculateColorAttributes()
{
    for (mesh::FacePtr facePtr = mMesh->faceBegin();
         facePtr != mMesh->faceEnd(); ++facePtr) {

        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;

            assert(facePtr->hasVertexAttribute(vertexPtr, mIlluminatedColor3fAttributeKey));
            cgmath::Vector3f directIllumination = facePtr->getVertexVector3f(
                vertexPtr, mIlluminatedColor3fAttributeKey);

            assert(facePtr->hasVertexAttribute(vertexPtr, mIndirectIllumination3fAttributeKey));
            cgmath::Vector3f indirectIllumination = facePtr->getVertexVector3f(
                vertexPtr, mIndirectIllumination3fAttributeKey);

            facePtr->setVertexVector3f(vertexPtr, mColor3fAttributeKey,
                directIllumination*mDirectIlluminationScale + indirectIllumination);
        }
    }
}
