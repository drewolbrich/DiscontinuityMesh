// Copyright 2008 Retroactive Fiasco.

#include "MeshSnapshot.h"

#include <cassert>

#include "Mesh.h"

namespace delaunay {

MeshSnapshot::MeshSnapshot()
    : mMeshPtr(),
      mCircleCenter(0.0, 0.0),
      mCircleRadius(0.0),
      mPointVector()
{
}

MeshSnapshot::~MeshSnapshot()
{
}

void
MeshSnapshot::setMeshPtr(MeshPtr meshPtr)
{
    mMeshPtr = meshPtr;
}

MeshPtr
MeshSnapshot::meshPtr() const
{
    return mMeshPtr;
}

void
MeshSnapshot::setCircleCenter(const cgmath::Vector2f &circleCenter)
{
    mCircleCenter = circleCenter;
}

const cgmath::Vector2f &
MeshSnapshot::circleCenter() const
{
    return mCircleCenter;
}

void
MeshSnapshot::setCircleRadius(float circleRadius)
{
    mCircleRadius = circleRadius;
}

float
MeshSnapshot::circleRadius() const
{
    return mCircleRadius;
}

void
MeshSnapshot::addPoint(const cgmath::Vector2f &point)
{
    mPointVector.push_back(point);
}

const MeshSnapshot::PointVector &
MeshSnapshot::pointVector() const
{
    return mPointVector;
}

} // namespace delaunay
