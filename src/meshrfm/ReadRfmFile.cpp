// Copyright 2008 Drew Olbrich

#include "ReadRfmFile.h"
#include "RfmFileReader.h"

namespace meshrfm {

void 
ReadRfmFile(mesh::Mesh *mesh, const std::string &filename)
{
    // Read the Mesh file, but do not return its header.
    RfmFileReader rfmFileReader;
    RfmFileHeader meshFileHeader;
    rfmFileReader.readRfmFile(mesh, &meshFileHeader, filename, true);
}

void 
ReadRfmFileAttributesAndHeader(mesh::Mesh *mesh, 
    RfmFileHeader *meshFileHeader, const std::string &filename)
{
    // Read the Mesh file's attributes, but do not read
    // it's vertices, edges, or faces. Also, the Mesh file's
    // header is returned.
    RfmFileReader rfmFileReader;
    rfmFileReader.readRfmFile(mesh, meshFileHeader, filename, false);
}

} // namespace mesh
