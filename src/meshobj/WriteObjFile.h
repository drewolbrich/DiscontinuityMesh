// Copyright 2008 Retroactive Fiasco.

#ifndef MESHOBJ__WRITE_OBJ_FILE__INCLUDED
#define MESHOBJ__WRITE_OBJ_FILE__INCLUDED

#include <string>

namespace mesh {
class Mesh;
}

namespace meshobj {

// Write a Mesh object to a Wavefront OBJ file. If an error occurs
// while writing the file, a FailedOperationException is thrown. In
// addition to the OBJ file, a MTL file with the same root name will
// be written out if the mesh contains material definitions.
void WriteObjFile(const mesh::Mesh &mesh, const std::string &objFilename);

} // namespace meshobj

#endif // MESHOBJ__WRITE_OBJ_FILE__INCLUDED
