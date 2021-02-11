// Copyright 2009 Drew Olbrich

#include "MeshShaderFaceListener.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <algorithm>

#include <cgmath/Tolerance.h>
#include <mesh/FaceOperations.h>

#include "DiscontinuityMesher.h"

MeshShaderFaceListener::MeshShaderFaceListener()
    : mMesh(NULL),
      mDiscontinuityMesher(NULL),
      mRayOrigin(),
      mRayEndpoint(),
      mLocalVertexToIgnore(),
      mEmissiveFaceToIgnore(),
      mLocalFaceToIgnore(),
      mEpsilon()
{
}

MeshShaderFaceListener::~MeshShaderFaceListener()
{
}

void
MeshShaderFaceListener::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

void
MeshShaderFaceListener::setDiscontinuityMesher(DiscontinuityMesher *discontinuityMesher)
{
    mDiscontinuityMesher = discontinuityMesher;
}

void
MeshShaderFaceListener::setRayOrigin(const cgmath::Vector3f &rayOrigin)
{
    mRayOrigin = rayOrigin;
}

void
MeshShaderFaceListener::setRayEndpoint(const cgmath::Vector3f &rayEndpoint)
{
    mRayEndpoint = rayEndpoint;
}

void
MeshShaderFaceListener::setLocalVertexToIgnore(mesh::VertexPtr localVertexToIgnore)
{
    mLocalVertexToIgnore = localVertexToIgnore;
}

void
MeshShaderFaceListener::setEmissiveFaceToIgnore(mesh::FacePtr emissiveFaceToIgnore)
{
    mEmissiveFaceToIgnore = emissiveFaceToIgnore;
}

void
MeshShaderFaceListener::setLocalFaceToIgnore(mesh::FacePtr localFaceToIgnore)
{
    mLocalFaceToIgnore = localFaceToIgnore;
}

void
MeshShaderFaceListener::initialize()
{
    float worldSpaceEpsilon = std::max(1.0f, std::max(mRayOrigin.maxAbs(), mRayEndpoint.maxAbs()))
        *0.0005*cgmath::TOLERANCE;

    // Transform the world space epsilon into an epsilon value in the
    // parameter space of the ray, where t=0 at mRayOrigin and t=1 at mRayEndpoint.
    mEpsilon = worldSpaceEpsilon/(mRayEndpoint - mRayOrigin).length();

    // If the vertex is responsible for creating D0 vertices that are
    // nearly coincident with it, use a slightly larger value of epsilon.
    // This helps resolve situations where objects are in contact with
    // each other, but there is no mesh connectivity. In these situations,
    // geometry may slightly interpenetrate, which would cause false
    // shadowing if we didn't use a larger epsilon.
    if (mLocalVertexToIgnore != mMesh->vertexEnd()
        && mLocalVertexToIgnore->getBool(
            mDiscontinuityMesher->getCreatedNearlyCoincidentDegreeZeroVertexAttributeKey())) {
        mEpsilon *= 10.0f;
    }
}

bool
MeshShaderFaceListener::allowFaceIntersectionTest(mesh::ConstFacePtr facePtr, float t)
{
    // The function 'initialize' must be called first.
    assert(mEpsilon != 0.0);

    if (t <= mEpsilon || t >= 1.0 - mEpsilon) {
        return false;
    }

    // Don't test against the emissive face.
    if (facePtr == mEmissiveFaceToIgnore) {
        return false;
    }

    // Don't test against the local face.
    if (facePtr == mLocalFaceToIgnore) {
        return false;
    }

    // Don't test against the faces neighboring the vertex.
    if (mLocalVertexToIgnore != mMesh->vertexEnd()
        && mLocalVertexToIgnore->hasAdjacentFace(facePtr)) {
        return false;
    }

    // Ignore backfacing polygons.
    if ((mRayEndpoint - mRayOrigin).dot(mesh::GetFaceGeometricNormal(facePtr)) < 0.001) {
        return false;
    }

    return true;
}
