// Copyright 2008 Drew Olbrich

#include "WriteRfmFile.h"
#include "RfmFileWriter.h"

namespace meshrfm {

void 
WriteRfmFile(const mesh::Mesh &mesh, const std::string &filename)
{
    RfmFileWriter rfmFileWriter;
    rfmFileWriter.writeRfmFile(mesh, filename);
}

}
