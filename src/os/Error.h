// Copyright 2008 Retroactive Fiasco.

#ifndef OS__ERROR__INCLUDED
#define OS__ERROR__INCLUDED

#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>

namespace os {

class ErrorValue;

// Error
//
// This class is a platform-independent wrapper around errno under Unix or the value
// returned by GetLastError under Win32.
//
// Using this class, system errors can be kept track of and reported to the user without
// resorting directly to platform specific mechanisms like errno.

class Error
{
 public:
    Error();
    ~Error();

    // Copy constructor.
    Error(const Error &rhs);

    // Assignment.
    Error &operator=(const Error &rhs);

    // Returns an Error object based on fromUnixErrno or fromWin32LastError, 
    // depending on the platform. Use this where you would usually access the value 
    // of errno or call GetLastError.
    static const Error fromSystemError();

    // Returns true if the object has the value other than the default value that was
    // assigned in the default constructor.
    bool isDefined() const;
    
    // Returns true if the error code represents success and not an actual error.
    bool success() const;

    // Return the error as a human-readable string, using either strerror under Unix or
    // FormatMessage under Win32.
    const std::string asString() const;
    
    // This allows Error objects to be written to C++ output streams.
    friend std::ostream &operator<<(std::ostream &ostr, const Error &error);

private:
    // A pointer to the underlying platform-specific error value.
    // We use a smart pointer so we don't have to worry about leaking.
    boost::scoped_ptr<ErrorValue> mErrorValue;
};

} // namespace os

#endif // OS__ERROR__INCLUDED
