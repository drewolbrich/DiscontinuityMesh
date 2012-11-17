// Copyright 2010 Retroactive Fiasco.

#ifndef BOXWORLD__WORLD_CREATOR__INCLUDED
#define BOXWORLD__WORLD_CREATOR__INCLUDED

#include <cgmath/BoundingBox3f.h>

#include "Grid.h"

namespace mesh {
class Mesh;
}

// WorldCreator
//
// 

class WorldCreator
{
public:
    WorldCreator();
    ~WorldCreator();

    // The dimensions of the world, as integer multiples of the input mesh (X, Y, Z).
    void setWidth(int width);
    int width() const;
    void setHeight(int height);
    int height() const;
    void setDepth(int depth);
    int depth() const;

    // The input mesh that the output mesh is constructed from.
    void setInputMesh(mesh::Mesh *inputMesh);
    mesh::Mesh *inputMesh() const;

    // The output mesh.
    void setOutputMesh(mesh::Mesh *outputMesh);
    mesh::Mesh *outputMesh() const;

    // Number of boxes to create.
    void setBoxCount(int boxCount);
    int boxCount() const;

    void create();

private:
    int mWidth;
    int mHeight;
    int mDepth;

    mesh::Mesh *mInputMesh;
    mesh::Mesh *mOutputMesh;

    int mBoxCount;

    cgmath::BoundingBox3f mInputMeshBoundingBox;

    Grid mGrid;
};

#endif // BOXWORLD__WORLD_CREATOR__INCLUDED
