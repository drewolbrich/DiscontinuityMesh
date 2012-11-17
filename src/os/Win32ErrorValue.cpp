// Copyright 2010 Retroactive Fiasco.

#include "Win32ErrorValue.h"

#if OS_WIN32
#include <windows.h>
#endif

namespace os {

Win32ErrorValue::Win32ErrorValue()
    : mIsDefined(false)
#if OS_WIN32
    ,
      mWin32ErrorCode(ERROR_SUCCESS)
#endif
{
}

Win32ErrorValue::~Win32ErrorValue()
{
}

Win32ErrorValue *
Win32ErrorValue::clone()
{
    // This assumes the compiler-supplied default copy constructor is sufficient.
    return new Win32ErrorValue(*this);
}

void
Win32ErrorValue::setToSystemError()
{
    mIsDefined = true;
#if OS_WIN32
    mWin32ErrorCode = GetLastError();
#endif    
}

bool
Win32ErrorValue::isDefined() const
{
    return mIsDefined;
}

bool
Win32ErrorValue::success() const
{
#if OS_WIN32
    return mWin32ErrorCode == ERROR_SUCCESS;
#else
    return false;
#endif
}

const std::string
Win32ErrorValue::asString() const
{
    std::string result;

#if OS_WIN32
    LPVOID msgBuf;
    if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
            mWin32ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &msgBuf, 0, NULL)) {
        result = reinterpret_cast<char *>(msgBuf);
        LocalFree(msgBuf);
        
        // Remove the trailing carriage return line feed sequence.
        if (result.size() > 1
            && result[result.size() - 1] == '\n'
            && result[result.size() - 2] == '\r') {
            result.resize(result.size() - 2);
        }
    }
#endif

    return result;
}

} // namespace os
