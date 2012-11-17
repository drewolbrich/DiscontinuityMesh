// Copyright 2008 Retroactive Fiasco.

#ifndef MESHOBJ__READ_OBJ_FILE__INCLUDED
#define MESHOBJ__READ_OBJ_FILE__INCLUDED

#include <string>

namespace mesh {
class Mesh;
}

namespace meshobj {

// Load a Mesh object from a Wavefront OBJ file. If the OBJ file
// cannot be opened, an OpenFileException is thrown. If the file
// contains errors, a FailedOperationException is thrown.
void ReadObjFile(mesh::Mesh *mesh, const std::string &objFilename);

} // namespace meshobj

#endif // MESHOBJ__READ_OBJ_FILE__INCLUDED
