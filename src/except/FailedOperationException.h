// Copyright 2008 Drew Olbrich

#ifndef EXCEPT__FAILED_OPERATION_EXCEPTION__INCLUDED
#define EXCEPT__FAILED_OPERATION_EXCEPTION__INCLUDED

#include "ErrorException.h"
#include "ExceptionMessageAppender.h"

namespace except {

// FailedOperationException
//
// This is a catch-all exception indicating failure.
//
// Example:
//     throw except::FailedOperationException(SOURCE_LINE, os::Error::fromSystemError())
//         << "Error reading from file \"" << filename << "\".";

class FailedOperationException 
    : public ErrorException,
        public ExceptionMessageAppender<FailedOperationException>
{
public:
    FailedOperationException(const SourceLine &sourceLine, const os::Error &error = os::Error())
        : ErrorException(sourceLine, error) {
    } 
    ~FailedOperationException() throw() {
    }
};

} // namespace except

#endif // EXCEPT__FAILED_OPERATION_EXCEPTION__INCLUDED
