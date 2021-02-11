// Copyright 2009 Drew Olbrich

#include "Time.h"

#include <cassert>

#include <sys/time.h>

#include <os/Platform.h>

namespace os {

#if OS_WIN32
// Difference between Win32 FILETIME seconds (epoch January 1, 1601)
// and Unix timeval seconds (epoch January 1, 1970).
static const __int64 WIN32_FILETIME_SECONDS_TO_UNIX_TIMEVAL_SECONDS = -11644473600;
#endif

TimeValue 
GetCurrentTime()
{
#if OS_UNIX
    struct timeval timeval;
    gettimeofday(&timeval, 0);

    return TimeValue(timeval);
#endif

#if OS_WIN32
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);

    ULARGE_INTEGER largeTime;
    largeTime.LowPart = fileTime.dwLowDateTime;
    largeTime.HighPart = fileTime.dwHighDateTime;

    // Convert the Windows time value to its Unix equivalent.
    return TimeValue((long int) (largeTime.QuadPart/10000000
            + WIN32_FILETIME_SECONDS_TO_UNIX_TIMEVAL_SECONDS),
        (long int) ((largeTime.QuadPart % 10000000)/10));
#endif
}

TimeValue
GetProcessUserTime()
{
#ifdef OS_UNIX

    struct rusage r_usage;
#ifdef DEBUG
    int result = 
#endif
        getrusage(RUSAGE_SELF, &r_usage);
    assert(result == 0);

    return TimeValue(r_usage.ru_utime);

#else

#error "Not yet implemented."

#endif
}

} // namespace os
