// Copyright 2008 Drew Olbrich

#ifndef CON__SYS_LOG_STREAM_BUF__INCLUDED
#define CON__SYS_LOG_STREAM_BUF__INCLUDED

#include <iostream>
#include <string>

#include <os/Platform.h>

#if OS_UNIX
#include <syslog.h>
#endif

#if OS_WIN32
// These allow Windows to use some of the same log symbols that are
// defined under Unix.
#define LOG_CRIT     2    // con::fatal
#define LOG_ERR      3    // con::error
#define LOG_WARNING  4    // con::warn
#define LOG_INFO     6    // con::info
#define LOG_DEBUG    7    // con::debug and con::trace
#endif

#include "StreamBuf.h"

namespace con {

// SysLogStreamBuf
//
// This class is a stream buffer that redirects output to the system
// log via the syslog system call.

class SysLogStreamBuf : public con::StreamBuf
{
 public:
    SysLogStreamBuf(const int priority);
    virtual ~SysLogStreamBuf();

    // Open and close the system log. It is not usually to necessary
    // to call these, as they are called automatically when
    // a message is written or the destructor is called.
    void openSystemLog();
    void closeSystemLog();

 private:
    // These functions override those in std::streambuf.
    virtual int overflow(int c);
    virtual std::streamsize xsputn(const char *s, std::streamsize n);
    virtual int sync();

    // Appends a string to the buffer. The string does not have
    // to be null-terminated.
    void appendString(const char *s, const int n);

    // Writes the contents of the buffer to the output stream.
    int flushBuffer();

    // If true, this object's constructor has been called.
    // This is used to check if global instances are being used
    // before they have been constructed.
    bool mConstructorWasCalled;

    // syslog priority.
    int mPriority;

#if OS_WIN32
    // The file that the log is written to under Windows.
    FILE *mWin32File;
#endif
};

} // namespace con

#endif // CON__SYS_LOG_STREAM_BUF__INCLUDED
