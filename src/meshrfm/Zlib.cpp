// Copyright 2008 Retroactive Fiasco.

#include "Zlib.h"

#include <except/FailedOperationException.h>
#include <os/Error.h>

using except::FailedOperationException;
using os::Error;

namespace meshrfm {

void
ThrowZlibErrorException(gzFile file, const except::SourceLine &sourceLine,
    const std::string &message)
{
    int zlibErrorNumber = Z_ERRNO;
    gzerror(file, &zlibErrorNumber);

    switch (zlibErrorNumber) {
    case Z_ERRNO:
        throw FailedOperationException(sourceLine, Error::fromSystemError()) << message;
        break;
    case Z_STREAM_ERROR:
        throw FailedOperationException(sourceLine) << message << " (zlib stream error)";
        break;
    case Z_DATA_ERROR:
        throw FailedOperationException(sourceLine) << message << " (zlib data error)";
        break;
    case Z_MEM_ERROR:
        throw FailedOperationException(sourceLine) << message << " (zlib memory error)";
        break;
    case Z_BUF_ERROR:
        throw FailedOperationException(sourceLine) << message << " (zlib buffer error)";
        break;
    case Z_VERSION_ERROR:
        throw FailedOperationException(sourceLine) << message << " (zlib version error)";
        break;
    default:
        throw FailedOperationException(sourceLine) << message << " (unknown zlib error)";
        break;
    }
}

} // namespace mesh
