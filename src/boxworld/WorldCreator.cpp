// Copyright 2010 Retroactive Fiasco.

#include "WorldCreator.h"

#include <cstdlib>
#include <cmath>

#include <mesh/Mesh.h>
#include <mesh/MeshOperations.h>
#include <mesh/Concatenate.h>
#include <con/Streams.h>
#include <cgmath/Matrix4f.h>

WorldCreator::WorldCreator()
    : mWidth(0),
      mHeight(0),
      mDepth(0),
      mInputMesh(NULL),
      mOutputMesh(NULL),
      mBoxCount(0),
      mInputMeshBoundingBox(),
      mGrid()
{
}

WorldCreator::~WorldCreator()
{
}

void
WorldCreator::setWidth(int width)
{
    mWidth = width;
}

int
WorldCreator::width() const
{
    return mWidth;
}

void
WorldCreator::setHeight(int height)
{
    mHeight = height;
}

int
WorldCreator::height() const
{
    return mHeight;
}

void
WorldCreator::setDepth(int depth)
{
    mDepth = depth;
}

int
WorldCreator::depth() const
{
    return mDepth;
}

void
WorldCreator::setInputMesh(mesh::Mesh *inputMesh)
{
    mInputMesh = inputMesh;
}

mesh::Mesh *
WorldCreator::inputMesh() const
{
    return mInputMesh;
}

void
WorldCreator::setOutputMesh(mesh::Mesh *outputMesh)
{
    mOutputMesh = outputMesh;
}

mesh::Mesh *
WorldCreator::outputMesh() const
{
    return mOutputMesh;
}

void
WorldCreator::setBoxCount(int boxCount)
{
    mBoxCount = boxCount;
}

int
WorldCreator::boxCount() const
{
    return mBoxCount;
}

void
WorldCreator::create()
{
    if (mBoxCount > mWidth*mHeight*mDepth) {    
        con::error << "Box count " << mBoxCount << " exceeds total mesh size "
            << mWidth*mHeight*mDepth << "." << std::endl;
        exit(EXIT_FAILURE);
    }

    mInputMeshBoundingBox = mesh::ComputeBoundingBox(*mInputMesh);

    mGrid.setWidth(mWidth);
    mGrid.setHeight(mHeight);
    mGrid.setDepth(mDepth);
    mGrid.initialize();

    for (int x = 0; x < mWidth; ++x) {
        for (int y = 0; y < mHeight; ++y) {
            mGrid.set(x, y, 0, 1);
        }
    }

    for (int x = 0; x < mWidth; ++x) {
        for (int z = 0; z < mDepth; ++z) {
            mGrid.set(x, 0, z, 1);
        }
    }

    for (int y = 0; y < mHeight; ++y) {
        for (int z = 0; z < mDepth; ++z) {
            mGrid.set(0, y, z, 1);
        }
    }

    int boxCount = 1;
    while (boxCount < mBoxCount) {

        int x = mWidth - 1;
        int y = mHeight - 1;
        int z = mDepth - 1;

        switch (random() % 3) {
        case 0:
            x = random() % mWidth;
            y = random() % mHeight;
            break;
        case 1:
            x = random() % mWidth;
            z = random() % mDepth;
            break;
        case 2:
            y = random() % mHeight;
            z = random() % mDepth;
            break;
        }

        while (mGrid.neighbors(x, y, z) == 0) {
            x += (random() % 2) - 1;
            y += (random() % 2) - 1;
            z += (random() % 2) - 1;

            if (x < 0) {
                x = 0;
            }
            if (x > mWidth - 1) {
                x = mWidth - 1;
            }
            if (y < 0) {
                y = 0;
            }
            if (y > mHeight - 1) {
                y = mHeight - 1;
            }
            if (z < 0) {
                z = 0;
            }
            if (z > mDepth - 1) {
                z = mDepth - 1;
            }
        }

        ++boxCount;

        mGrid.set(x, y, z, 1);
    }

    for (int x = 0; x < mWidth; ++x) {
        for (int y = 0; y < mHeight; ++y) {
            for (int z = 0; z < mDepth; ++z) {
                if (mGrid.get(x, y, z) == 0) {
                    continue;
                }

                int neighbors = mGrid.neighbors(x, y, z);

                con::debug << x << " " << y << " " << z << std::endl;

                if (neighbors == 0) {
                    continue;
                }

                cgmath::Vector3f position(
                    x*mInputMeshBoundingBox.sizeX(),
                    y*mInputMeshBoundingBox.sizeY(),
                    z*mInputMeshBoundingBox.sizeY());

                position += cgmath::Vector3f(
                    -mWidth*0.5*mInputMeshBoundingBox.sizeX(),
                    -mHeight*0.5*mInputMeshBoundingBox.sizeY(),
                    -mDepth*0.5*mInputMeshBoundingBox.sizeZ());

                float scale = 0.98;
                if (neighbors == 1) {
                    scale = 0.5;
                } else if (neighbors < 4) {
                    scale = 0.75;
                }

                scale *= 0.98;

                mesh::Mesh transformedBox;
                mesh::Concatenate(&transformedBox, *mInputMesh);
                mesh::Transform(&transformedBox, 
                    cgmath::Matrix4f::fromTranslation(position)
                    *cgmath::Matrix4f::fromScale(cgmath::Vector3f::UNIT*scale));

                mesh::Concatenate(mOutputMesh, transformedBox);
            }
        }
    }
}
