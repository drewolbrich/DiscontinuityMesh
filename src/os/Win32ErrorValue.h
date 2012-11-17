// Copyright 2010 Retroactive Fiasco.

#ifndef OS__WIN32_ERROR_VALUE__INCLUDED
#define OS__WIN32_ERROR_VALUE__INCLUDED

#include <os/Platform.h>

#if OS_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ErrorValue.h"

namespace os {

// Win32ErrorValue
//
// Implementation of ErrorValue for Win32.

class Win32ErrorValue : public ErrorValue
{
public:
    Win32ErrorValue();
    virtual ~Win32ErrorValue();

    // For ErrorValue:
    virtual Win32ErrorValue *clone();
    virtual void setToSystemError();
    virtual bool isDefined() const;
    virtual bool success() const;
    virtual const std::string asString() const;

private:
    bool mIsDefined;
#if OS_WIN32
    DWORD mWin32ErrorCode;
#endif
};

} // namespace os

#endif // OS__WIN32_ERROR_VALUE__INCLUDED
