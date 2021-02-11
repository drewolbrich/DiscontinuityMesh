// Copyright 2010 Drew Olbrich

#ifndef EXCEPT__ERROR_EXCEPTION__INCLUDED
#define EXCEPT__ERROR_EXCEPTION__INCLUDED

#include <os/Error.h>

#include "Exception.h"

namespace except {

// ErrorException
//
// Base class for exceptions that take optional os::Error objects in their constructors.
// This class should not be instantiated directly.

class ErrorException : public Exception
{
public:
    ErrorException(const SourceLine &sourceLine, const os::Error &error = os::Error());

    // We define a pure virtual destructor to prevent instantiation,
    // since this class is only intended to be used as a base class.
    virtual ~ErrorException() throw() = 0;

    // Returns the error code passed to the exception.
    const os::Error &error() const;
    
private:
    // This function causes a textual description of the Error object passed to the
    // constructor to be appended to the string returned by Exception::asString.
    virtual std::string getAdditionalString() const;

    os::Error mError;
};

} // namespace except

#endif // EXCEPT__ERROR_EXCEPTION__INCLUDED
