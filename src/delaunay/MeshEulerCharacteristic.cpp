// Copyright 2008 Drew Olbrich

#include "MeshEulerCharacteristic.h"

namespace delaunay {

int
GetMeshEulerCharacteristic(MeshPtr meshPtr)
{
    return meshPtr->vertexCount() - meshPtr->edgeCount() + meshPtr->faceCount();
}

} // namespace delaunay
