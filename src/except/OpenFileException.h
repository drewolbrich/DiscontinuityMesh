// Copyright 2008 Retroactive Fiasco.

#ifndef EXCEPT__OPEN_FILE_EXCEPTION__INCLUDED
#define EXCEPT__OPEN_FILE_EXCEPTION__INCLUDED

#include "ErrorException.h"
#include "ExceptionMessageAppender.h"

namespace except {

// OpenFileException
//
// This exception indicates that a file could not be opened, for
// example because the file is missing, or the user does not have the
// right permissions.
//
// OpenFileException should not be used to indicate a syntax error or
// other error within a file. FailedOperationException should be used
// in that case.
//
// Example:
//     throw except::OpenFileException(SOURCE_LINE, os::Error::fromSystemError())
//         << "Could not open file \"" << filename << "\" for reading.";

class OpenFileException : public ErrorException,
    public ExceptionMessageAppender<OpenFileException>
{
public:
    OpenFileException(const SourceLine &sourceLine, const os::Error &error = os::Error())
        : ErrorException(sourceLine, error) {
    }
    ~OpenFileException() throw() {
    }
};

} // namespace except

#endif // EXCEPT__OPEN_FILE_EXCEPTION__INCLUDED
