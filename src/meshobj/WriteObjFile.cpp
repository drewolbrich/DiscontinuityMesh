// Copyright 2008 Retroactive Fiasco.

#include "WriteObjFile.h"
#include "ObjFileWriter.h"

namespace meshobj {

void 
WriteObjFile(const mesh::Mesh &mesh, const std::string &objFilename)
{
    ObjFileWriter objFileWriter;
    objFileWriter.writeObjFile(mesh, objFilename);
}

} // namespace meshobj
