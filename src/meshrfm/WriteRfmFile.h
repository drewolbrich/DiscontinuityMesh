// Copyright 2008 Retroactive Fiasco.

#ifndef MESHRFM__WRITE_RFM_FILE__INCLUDED
#define MESHRFM__WRITE_RFM_FILE__INCLUDED

#include <string>

namespace mesh {
class Mesh;
}

namespace meshrfm {

// Write a Mesh to a .rfm file. If the file cannot be written,
// a FailedOperationException is thrown.
void WriteRfmFile(const mesh::Mesh &mesh, const std::string &filename);

} // namespace mesh

#endif // MESHRFM__WRITE_RFM_FILE__INCLUDED
