// Copyright 2008 Drew Olbrich

#include "SysLogStreamBuf.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <sstream>
#include <algorithm>

#if OS_UNIX
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

#include <boost/thread/mutex.hpp>

#include "ProgramName.h"

namespace con {

// If true, the system log is open.
static bool gSystemLogIsOpen = false;

SysLogStreamBuf::SysLogStreamBuf(const int priority)
    : StreamBuf(),
      mConstructorWasCalled(true),
      mPriority(priority)
{
#if OS_WIN32
    mWin32File = NULL;
#endif
}

SysLogStreamBuf::~SysLogStreamBuf()
{
    Buffer &buffer = threadSpecificBuffer();
    if (!buffer.empty()) {
        sync();
    }

    // The system log is not closed, because there is only
    // one system log shared between all the SysLogStreamBuf objects,
    // and it isn't really necessary to close it.
}

void
SysLogStreamBuf::openSystemLog()
{
    // The system log should not already be open.
    assert(!gSystemLogIsOpen);

#if OS_UNIX
    openlog(GetProgramName().c_str(), 0, LOG_USER);
#endif

#if OS_WIN32
    // Under Windows, the log is written to a file in the C:\ directory,
    // with a file name based on the name of the program and the
    // extension ".log".
    std::ostringstream fileName;
    fileName << "C:\\" << GetProgramName() << ".log";
    mWin32File = fopen(fileName.str().c_str(), "a");
    assert(mWin32File != NULL);
#endif

    gSystemLogIsOpen = true;
}

void
SysLogStreamBuf::closeSystemLog()
{
    // The system log should be open.
    assert(gSystemLogIsOpen);

#if OS_UNIX
    closelog();
#endif

#if OS_WIN32
    int result = fclose(mWin32File);
    assert(result == 0);
    mWin32File = NULL;
#endif

    gSystemLogIsOpen = false;
}

int
SysLogStreamBuf::overflow(int c)
{
    // Make sure that the constructor has been called. This will fail
    // if a global instance of this class is used before it has
    // been constructed, for example, from another global object that
    // was constructed first.
    assert(mConstructorWasCalled);

    Buffer &buffer = threadSpecificBuffer();
    buffer.push_back(c);

    return c;
}

std::streamsize
SysLogStreamBuf::xsputn(const char *s, std::streamsize n)
{
    // Make sure that the constructor has been called. This will fail
    // if a global instance of this class is used before it has
    // been constructed, for example, from another global object that
    // was constructed first.
    assert(mConstructorWasCalled);

    appendString(s, n);

    return n;
}

int
SysLogStreamBuf::sync()
{
    // Make sure that the constructor has been called. This will fail
    // if a global instance of this class is used before it has
    // been constructed, for example, from another global object that
    // was constructed first.
    assert(mConstructorWasCalled);

    if (flushBuffer() < 0) {
        return -1;
    }

    return 0;
}

void 
SysLogStreamBuf::appendString(const char *s, const int n)
{
    // Convert the passed character array into an STL string,
    std::string tempString(s, n);

    // Insert the string at the end of the buffer.
    Buffer &buffer = threadSpecificBuffer();
    buffer.insert(buffer.end(), tempString.begin(), tempString.end());
}

int
SysLogStreamBuf::flushBuffer()
{
    // Lock a Mutex for the remainder of this function
    // so that multiple threads won't conflict with each other.
    // This also deals with the possibility that syslog
    // is not thread safe.
    static boost::mutex mutex;
    boost::mutex::scoped_lock scopedLock(mutex);
    
    // Open the system log if we haven't already.
    if (!gSystemLogIsOpen) {
        openSystemLog();
    }

    // This is the index in the buffer that output will start at.
    // As each line is output, it moves through the buffer.
    unsigned int base = 0;

    // Loop until we reach the end of the buffer.
    Buffer &buffer = threadSpecificBuffer();
    while (base < buffer.size()) {

        // Determine how many characters we'd like to write out on
        // the current line, first by searching through the string for
        // a newline charcter, if any. If we reach the end of the
        // string without finding a newline character, this means we'd
        // like to print out the entire string.
        unsigned int length = 0;
        while (base + length < buffer.size()
            && buffer[base + length] != '\n') {
            length++;
        }

        // The syslog function requires a null-terminated string
        // as input. If the string ends in a newline, replace it with
        // a null character. If not, append a null character.
        if (buffer[base + length] == '\n') {
            buffer[base + length] = '\0';
        } else {
            buffer.push_back('\0');
        }
        length++;

        // Convert the special hard space characters back into
        // regular spaces.
        decodeHardSpaces(base, length);

#if OS_UNIX
        // Send the string to the system log.
        syslog(mPriority, "%s", &buffer[base]);
#endif

#if OS_WIN32
        SYSTEMTIME universalTime;
        GetSystemTime(&universalTime);

        // Convert from Coordinated Universal Time to local time.
        SYSTEMTIME localTime;
        SystemTimeToTzSpecificLocalTime(NULL, &universalTime, &localTime);

        char timeStamp[32];
        sprintf(timeStamp, "%04d/%02d/%02d %02d:%02d:%02d",
            localTime.wYear, localTime.wMonth, localTime.wDay,
            localTime.wHour, localTime.wMinute, localTime.wSecond);
        
        assert(mWin32File != NULL);
        fprintf(mWin32File, "%s %s\n", timeStamp, &buffer[base]);
#endif
        
        // Advance to the position in the buffer where the
        // next line starts.
        base += length;
    }
    
    // Empty the buffer so we are ready for more characters.
    buffer.clear();

    return 0;
}

} // namespace con;
