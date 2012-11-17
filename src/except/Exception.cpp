// Copyright 2010 Retroactive Fiasco.

#include "Exception.h"

#include <os/Platform.h>

namespace except {

Exception::Exception()
    : std::exception(),
      mSourceLine(),
      mMessageOutputStream(),
      mStringReturnedByWhat()
{
}

Exception::Exception(const SourceLine &sourceLine)
    : std::exception(),
      mSourceLine(sourceLine),
      mMessageOutputStream(),
      mStringReturnedByWhat() 
{
}

Exception::~Exception() throw()
{
}

Exception::Exception(const Exception &exception)
    : std::exception(),
      mSourceLine(exception.mSourceLine),
      mMessageOutputStream(),
      mStringReturnedByWhat(exception.mStringReturnedByWhat) 
{
    mMessageOutputStream << exception.mMessageOutputStream.str();
}

const SourceLine &
Exception::sourceLine() const
{
    return mSourceLine;
}

std::string
Exception::asString() const
{
    std::stringstream ostr;
    ostr << mMessageOutputStream.str();

    std::string additionalString = getAdditionalString();
    if (additionalString != "") {
        ostr << " (" << additionalString << ")";
    }

    // We only include the source line as part of the string in debug mode,
    // because it's only intended to be seen by developers.
#if DEBUG
    if (mSourceLine.isDefined()) {
        ostr << " (Exception thrown from " << mSourceLine << ")";
    }
#endif

    return ostr.str();
}

const char *
Exception::what() const throw()
{
    // Store a copy of the stream's output before returning a pointer to the C string
    // representation of it. Otherwise, we'd wind up returning a dangling pointer.
    mStringReturnedByWhat = asString();

    return mStringReturnedByWhat.c_str();
}

std::ostream &
operator<<(std::ostream &ostr, const Exception &exception)
{
    return ostr << exception.asString();
}

std::string
Exception::getAdditionalString() const
{
    return "";
}

} // namespace except
