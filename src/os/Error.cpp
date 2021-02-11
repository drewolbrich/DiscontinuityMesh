// Copyright 2010 Drew Olbrich

#include "Error.h"

#include <os/Platform.h>

// The type of ErrorValue we use depends on the platform.
#if OS_UNIX
#include "UnixErrorValue.h"
#define ERROR_VALUE_TYPE  UnixErrorValue
#endif
#if OS_WIN32
#include "Win32ErrorValue.h"
#define ERROR_VALUE_TYPE  Win32ErrorValue
#endif

namespace os {

Error::Error()
    : mErrorValue()
{
    mErrorValue.reset(new ERROR_VALUE_TYPE);
}

Error::~Error()
{
}

Error::Error(const Error &rhs)
{
    mErrorValue.reset(rhs.mErrorValue->clone());
}

Error &
Error::operator=(const Error &rhs)
{
    mErrorValue.reset(rhs.mErrorValue->clone());

    return *this;
}

const Error
Error::fromSystemError()
{
    Error error;
    error.mErrorValue->setToSystemError();

    return error;
}

bool
Error::isDefined() const
{
    return mErrorValue->isDefined();
}

bool
Error::success() const
{
    return mErrorValue->success();
}

const std::string
Error::asString() const
{
    return mErrorValue->asString();
}

std::ostream &
operator<<(std::ostream &ostr, const Error &error)
{
    return ostr << error.asString();
}

} // namespace os
