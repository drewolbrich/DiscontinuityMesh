// Copyright 2010 Drew Olbrich

#ifndef OS__ERROR_VALUE__INCLUDED
#define OS__ERROR_VALUE__INCLUDED

#include <string>

namespace os {

// ErrorValue
//
// Interface for platform-specific error values contained within Error objects.

class ErrorValue
{
public:
    ErrorValue() {}
    virtual ~ErrorValue() {}

    // Return a copy of the error value. This is needed by Error's copy constructor
    // and assignment operator.
    virtual ErrorValue *clone() = 0;

    // Set the error value to that of the current system error, using platform-specific
    // mechanisms like (Unix) errno or (Win32) GetLastError.
    virtual void setToSystemError() = 0;

    // Returns true if the object has the value other than the default value that was
    // assigned in the default constructor.
    virtual bool isDefined() const = 0;

    // Returns true if the error code represents success and not an actual error.
    virtual bool success() const = 0;

    // Return the error as a human-readable string, using either strerror under Unix or
    // FormatMessage under Win32.
    virtual const std::string asString() const = 0;
};

} // namespace os

#endif // OS__ERROR_VALUE__INCLUDED
