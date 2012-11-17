// Copyright 2010 Retroactive Fiasco.

#include "ErrorException.h"

namespace except {

ErrorException::ErrorException(const SourceLine &sourceLine, const os::Error &error)
    : Exception(sourceLine),
      mError(error)
{
}

ErrorException::~ErrorException() throw()
{
}

const os::Error &
ErrorException::error() const
{
    return mError;
}

std::string
ErrorException::getAdditionalString() const
{
    if (mError.isDefined()) {
        return mError.asString();
    } else {
        return "";
    } 
}

} // namespace except
