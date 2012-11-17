// Copyright 2008 Retroactive Fiasco.

#include "ReadObjFile.h"
#include "ObjFileReader.h"

namespace meshobj {

void
ReadObjFile(mesh::Mesh *mesh, const std::string &objFilename)
{
    ObjFileReader objFileReader;
    objFileReader.readObjFile(mesh, objFilename);
}

} // namespace meshobj
