// Copyright 2010 Retroactive Fiasco.

#include <os/Platform.h>

#if OS_UNIX
#include <errno.h>
#endif

#include "UnixErrorValue.h"

namespace os {

UnixErrorValue::UnixErrorValue()
    : mIsDefined(false),
      mErrno(0)
{
}

UnixErrorValue::~UnixErrorValue()
{
}

UnixErrorValue *
UnixErrorValue::clone()
{
    // This assumes the compiler-supplied default copy constructor is sufficient.
    return new UnixErrorValue(*this);
}

void
UnixErrorValue::setToSystemError()
{
#if OS_UNIX
    mIsDefined = true;
    mErrno = errno;
#endif
}

bool
UnixErrorValue::isDefined() const
{
    return mIsDefined;
}

bool
UnixErrorValue::success() const
{
    return mErrno == 0;
}

const std::string
UnixErrorValue::asString() const
{
    return std::string(strerror(mErrno));
}

} // namespace os
