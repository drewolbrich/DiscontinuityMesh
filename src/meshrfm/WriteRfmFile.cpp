// Copyright 2008 Retroactive Fiasco.

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
