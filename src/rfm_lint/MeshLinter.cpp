// Copyright 2009 Retroactive Fiasco.

#include "MeshLinter.h"

#include <iostream>

#include <cgmath/Tolerance.h>
#include <mesh/FaceOperations.h>
#include <mesh/IsConsistent.h>
#include <mesh/MeshOperations.h>
#include <meshprim/CylinderCreator.h>
#include <cgmath/Matrix4fOperations.h>
#include <cgmath/BoundingBox3f.h>

MeshLinter::MeshLinter()
    : mMesh(NULL),
      mAbsoluteTolerance(cgmath::TOLERANCE),
      mRelativeTolerance(cgmath::TOLERANCE),
      mShouldMarkErrors(false),
      mMarkedFaceVector()
{
}

MeshLinter::~MeshLinter()
{
}

void
MeshLinter::setMesh(mesh::Mesh *mesh)
{
    mMesh = mesh;
}

mesh::Mesh *
MeshLinter::mesh() const
{
    return mMesh;
}

void
MeshLinter::setAbsoluteTolerance(float absoluteTolerance)
{
    mAbsoluteTolerance = absoluteTolerance;
}

float
MeshLinter::absoluteTolerance() const
{
    return mAbsoluteTolerance;
}

void
MeshLinter::setRelativeTolerance(float relativeTolerance)
{
    mRelativeTolerance = relativeTolerance;
}

float
MeshLinter::relativeTolerance() const
{
    return mRelativeTolerance;
}

void
MeshLinter::setShouldMarkErrors(bool shouldMarkErrors)
{
    mShouldMarkErrors = shouldMarkErrors;
}

bool
MeshLinter::shouldMarkErrors() const
{
    return mShouldMarkErrors;
}

bool
MeshLinter::testMesh()
{
    bool success = true;

    mMarkedFaceVector.clear();

    if (!mesh::IsConsistent(*mMesh)) {
        std::cout << "Mesh is not consistent." << std::endl;
        success = false;
    }

    if (meshHasDegenerateFaces(*mMesh)) {
        success = false;
    }

    if (mShouldMarkErrors) {
        markErrors(mMesh);
    }

    return success;
}

bool
MeshLinter::meshHasDegenerateFaces(mesh::Mesh &mesh)
{
    size_t degenerateFaceCount = 0;
    for (mesh::FacePtr facePtr = mesh.faceBegin(); facePtr != mesh.faceEnd(); ++facePtr) {
        if (mesh::FaceIsDegenerate(facePtr,
                mesh::GetEpsilonFromFace(facePtr, mAbsoluteTolerance, mRelativeTolerance))) {
            ++degenerateFaceCount;
            if (mShouldMarkErrors) {
                mMarkedFaceVector.push_back(facePtr);
            }
        }
    }

    if (degenerateFaceCount > 0) {
        std::cout << "Mesh has " << degenerateFaceCount 
            << " degenerate faces." << std::endl;
        return true;
    }

    return false;
}

void
MeshLinter::markErrors(mesh::Mesh *mesh)
{
    meshprim::CylinderCreator cylinderCreator;
    cylinderCreator.setMesh(mesh);

    cgmath::BoundingBox3f bbox = mesh::ComputeBoundingBox(*mesh);
    float sceneDiameter = (bbox.max() - bbox.min()).length();
    cylinderCreator.setRadius(sceneDiameter*0.001f);

    for (FaceVector::const_iterator iterator = mMarkedFaceVector.begin();
         iterator != mMarkedFaceVector.end(); ++iterator) {
        mesh::FacePtr facePtr = *iterator;

        cgmath::BoundingBox3f bbox;
        bbox.reset();
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            bbox.extendByVector3f(vertexPtr->position());
        }

        unsigned longestAxis = 0;
        if (bbox.sizeY() > bbox.sizeX() && bbox.sizeY() > bbox.sizeZ()) {
            longestAxis = 1;
        } else if (bbox.sizeZ() > bbox.sizeX() && bbox.sizeZ() > bbox.sizeY()) {
            longestAxis = 2;
        };

        cgmath::Vector3f p0;
        cgmath::Vector3f p1;
        float min = std::numeric_limits<float>::max();
        float max = -std::numeric_limits<float>::max();
        for (mesh::AdjacentVertexIterator iterator = facePtr->adjacentVertexBegin();
             iterator != facePtr->adjacentVertexEnd(); ++iterator) {
            mesh::VertexPtr vertexPtr = *iterator;
            cgmath::Vector3f position = vertexPtr->position();
            if (position[longestAxis] <= min) {
                min = position[longestAxis];
                p0 = position;
            }
            if (position[longestAxis] >= max) {
                max = position[longestAxis];
                p1 = position;
            }
        }

        cylinderCreator.setTransformationMatrix(
            cgmath::GetPointConnectionMatrix(p0, p1));
        cylinderCreator.create();
    }
}

