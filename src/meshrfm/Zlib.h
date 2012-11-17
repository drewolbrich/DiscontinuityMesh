// Copyright 2008 Retroactive Fiasco.

#ifndef MESHRFM__ZLIB__INCLUDED
#define MESHRFM__ZLIB__INCLUDED

#include <string>

#include <zlib.h>

namespace except {
class SourceLine;
}

namespace meshrfm {

// Throws an exception based on the error number that
// the function gzerror returns for the specified gzFile.
// The second argument should always be the macro SOURCE_LINE,
// defined in the file src/except/SourceLine.h.
void ThrowZlibErrorException(gzFile file,
    const except::SourceLine &sourceLine, const std::string &message);

} // namespace meshrfm

#endif // MESHRFM__ZLIB__INCLUDED
