// Copyright 2008 Drew Olbrich

#ifndef EXCEPT__EXCEPTION__INCLUDED
#define EXCEPT__EXCEPTION__INCLUDED

#include <string>
#include <sstream>
#include <exception>
#include <ostream>

#include "SourceLine.h"

namespace except {

// Exception
//
// Base class for all exceptions. This class should not be instantiated directly.
//
// This class is compatible with the STL std::exception class.
//
// Derived classes are intended to be used with the SOURCE_LINE macro defined in
// SourceLine.h, so that the source file and line number that the exception was thrown
// from can be easily determined.
//
// For example:
//     throw except::FailedOperationException(SOURCE_LINE) << "Unexpected end of file.";
//
// The ability to assign descriptive messages to the exceptions before they are thrown
// is supplied by the ExceptionMessageAppender class template, which all Exception-derived
// classes should derive from.

class Exception : public std::exception
{
private:
    // Defining the default constructor requires the use of the SOURCE_LINE macro
    // whenever exceptions are thrown.
    Exception();

public:
    Exception(const SourceLine &sourceLine);

    // We define a pure virtual destructor to prevent instantiation,
    // since this class is only intended to be used as a base class.
    virtual ~Exception() throw() = 0;

    // Copy constructor. This is required for ExceptionMessageAppender to work.
    Exception(const Exception &exception);

    // Returns the source file, line, and function that the exception was thrown from.
    const SourceLine &sourceLine() const;
    
    // Returns a string representation of the corresponding human-readable error message. 
    // Derived classes can augment the value returned by this function by defining the
    // virtual function getAdditionalString (see below).
    std::string asString() const;

    // This method provides compatibility with the STL exception class.
    virtual const char *what() const throw();

    // Outputs the exception to a stream.
    friend std::ostream &operator<<(std::ostream &ostr, const Exception &exception);

    // This is called by ExceptionMessageAppender.
    template<typename TYPE> void appendToMessage(TYPE value) {
        mMessageOutputStream << value;
    }

private:
    // This function can be overridden by derived classes to return an additional
    // string that is appended to the string returned by asString.
    virtual std::string getAdditionalString() const;

    SourceLine mSourceLine;

    std::stringstream mMessageOutputStream;

    mutable std::string mStringReturnedByWhat;
};

} // namespace except

#endif // EXCEPT__EXCEPTION__INCLUDED
