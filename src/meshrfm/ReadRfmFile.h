// Copyright 2008 Retroactive Fiasco.

#ifndef MESHRFM__READ_RFM_FILE__INCLUDED
#define MESHRFM__READ_RFM_FILE__INCLUDED

#include <string>

namespace mesh {
class Mesh;
}

namespace meshrfm {

class RfmFileHeader;

// Read a Mesh from a .rfm file. If the file cannot be opened,
// an OpenFileException is thrown. If the file contains errors,
// a FailedOperationException is thrown.
void ReadRfmFile(mesh::Mesh *mesh, const std::string &filename);

// Read in a Mesh file's attributes and header, but do not
// read its vertices, edges, or faces. This is used by 
// the program mesh_print to display a summary of the mesh file
// contents without the cost of loading all of the mesh data.
// It could also be used to query a mesh's bounding box before
// deciding the load the whole thing.
void ReadRfmFileAttributesAndHeader(mesh::Mesh *mesh, 
    RfmFileHeader *meshFileHeader, const std::string &filename);

} // namespace mesh

#endif // MESHRFM__READ_RFM_FILE__INCLUDED
