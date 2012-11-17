// Copyright 2008 Retroactive Fiasco.

#include "WordWrappedStreamBuf.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <algorithm>

#include <os/Platform.h>

#if OS_UNIX
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

#include <boost/thread/mutex.hpp>

#include "ProgramName.h"

namespace con {

WordWrappedStreamBuf::WordWrappedStreamBuf(std::ostream *outputStream, 
    const std::string &secondaryPrefix, LogLevel logLevel, const unsigned int options)
    : StreamBuf(),
      mConstructorWasCalled(true),
      mOutputStream(outputStream),
      mSecondaryPrefix(secondaryPrefix),
      mLogLevel(logLevel),
      mOptions(options)
{
}

WordWrappedStreamBuf::~WordWrappedStreamBuf()
{
    Buffer &buffer = threadSpecificBuffer();
    if (!buffer.empty()) {
        sync();
    }
}

int
WordWrappedStreamBuf::overflow(int c)
{
    // Make sure that the constructor has been called. This will fail
    // if a global instance of this class is used before it has
    // been constructed, for example, from another global object that
    // was constructed first.
    assert(mConstructorWasCalled);

    if (!outputIsEnabled()) {
        return c;
    }

    Buffer &buffer = threadSpecificBuffer();
    if (buffer.empty()) {
        appendPrefix();
    }

    buffer.push_back(c);

    return c;
}

std::streamsize
WordWrappedStreamBuf::xsputn(const char *s, std::streamsize n)
{
    // Make sure that the constructor has been called. This will fail
    // if a global instance of this class is used before it has
    // been constructed, for example, from another global object that
    // was constructed first.
    assert(mConstructorWasCalled);

    if (!outputIsEnabled()) {
        return n;
    }

    Buffer &buffer = threadSpecificBuffer();
    if (buffer.empty()) {
        appendPrefix();
    }

    appendString(s, n);

    return n;
}

int
WordWrappedStreamBuf::sync()
{
    // Make sure that the constructor has been called. This will fail
    // if a global instance of this class is used before it has
    // been constructed, for example, from another global object that
    // was constructed first.
    assert(mConstructorWasCalled);

    if (!outputIsEnabled()) {
        return 0;
    }

    if (flushBuffer() < 0) {
        return -1;
    }

    return 0;
}

int
WordWrappedStreamBuf::getTerminalWidth()
{
    // If dynamic querying of the terminal width is not requested,
    // just return the default terminal width.
    if (!(mOptions & QUERY_TERMINAL_WIDTH)) {
        return DEFAULT_TERMINAL_WIDTH;
    }

#if OS_UNIX
    struct winsize ws;
    if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) < 0
            && ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) < 0
            && ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
        || ws.ws_col == 0) {
        return DEFAULT_TERMINAL_WIDTH;
    }

    return ws.ws_col;
#endif

#if OS_WIN32
    // Under Windows we don't know how to determine the terminal width,
    // so return the default value.
    return DEFAULT_TERMINAL_WIDTH;
#endif
}

void
WordWrappedStreamBuf::appendPrefix()
{
    bool alreadyOutputSomething = false;

    // Prefix the first line with the name of the program.
    if (mOptions & SHOW_PROGRAM_NAME_PREFIX) {

        std::string programName = GetProgramName();
        appendString(programName.data(), programName.size());
        alreadyOutputSomething = true;
    }

    // Output the secondary prefix, if any.
    if (!mSecondaryPrefix.empty()) {
        if (alreadyOutputSomething) {
            appendString(" ", 1);
        }
        
        // The secondary prefix is usually a string like "warning"
        // or "error".
        appendString(mSecondaryPrefix.data(), mSecondaryPrefix.size());
        alreadyOutputSomething = true;
    }

    // If we've output something already, separate it from the
    // message text that will follow it with a colon and a space.
    if (alreadyOutputSomething) {
        appendString(": ", 2);
    }
}

void 
WordWrappedStreamBuf::appendString(const char *s, const int n)
{
    // Convert the passed character array into an STL string,
    std::string tempString(s, n);

    // Insert the string at the end of the buffer.
    Buffer &buffer = threadSpecificBuffer();
    buffer.insert(buffer.end(), tempString.begin(), tempString.end());
}

int
WordWrappedStreamBuf::flushBuffer()
{
    if (!outputIsEnabled()) {
        return 0;
    }

    // This string will be prepended to every line 
    // except for the first one.
    const std::string indentationString("    ");

    // If true, we should indent each line that is displayed. The
    // first line displayed will not indent, but subsequent lines
    // will.
    bool indent = false;

    // This is the index in the buffer that output will start at.
    // As each line is output, it moves through the buffer.
    int base = 0;

    // This is the return value of the function;
    int result = 0;

    // Lock a Mutex for the remainder of this function
    // so that multiple threads won't conflict with each other.
    static boost::mutex mutex;
    boost::mutex::scoped_lock scopedLock(mutex);
    
    // Get a reference to the thread-specific buffer that holds
    // the characters we want to write out.
    Buffer &buffer = threadSpecificBuffer();

    // Loop until we reach the end of the buffer.
    while (base < static_cast<int>(buffer.size())) {
        
        // This variable keeps track of the column that text is about
        // to be displayed at.
        int column = 0;
        
        // Indent the line if necessary.
        if (indent) {
            *mOutputStream << indentationString;
            if (!mOutputStream->good()) {
                result = -1;
                break;
            }
            column += indentationString.size();
        }
        
        // Determine how many characters we'd like to write out on
        // the current line, first by searching through the string for
        // a newline charcter, if any. If we reach the end of the
        // string without finding a newline character, this means we'd
        // like to print out the entire string.
        int length = 0;
        while (base + length < static_cast<int>(buffer.size())
            && buffer[base + length] != '\n') {
            length++;
        }
        
        // If we stopped at a newline, include it in the span
        // of characters we're considering so that we don't encounter
        // it again next time. Otherwise, we would loop forever.
        if (buffer[base + length] == '\n') {
            length++;
        }
        
        // Use this initial length as an upper bound. This allows
        // us to correctly handle strings that contain sequences
        // of multiple newlines.
        int maxLength = length;
        
        // Determine the column to wrap the text at. This can
        // change if the terminal window is resized.
        int wrapColumn = getTerminalWidth() - RIGHT_MARGIN;
        
        // If the number of characters we're about to write out would
        // exceed the maximum allowed per line, try to modify our output
        // to fit this restriction.
        if (column + length > wrapColumn) {
            
            // Start our analysis of the string at the position where the
            // string would get cut off at the edge of the window.
            length = wrapColumn - column;
            if (length < 0) {
                length = 0;
            }
            
            // Back up through the string, looking for whitespace.
            int index = length;
            while (index >= 0 
                && !isspace(buffer[base + index])) {
                index--;
            }
            
            // If no whitespace was found, next try looking forward through
            // the string for whitespace.
            if (index < 0) {
                index = length;
                while (index < maxLength
                    && !isspace(buffer[base + index])) {
                    index++;
                }
            }
            
            // One way or another, we have determined how many
            // characters we'd like to print out.
            length = index;
        }
        
        // Exclude all of the trailing whitespace at the break position.
        while (length >= 0
            && (length == maxLength
                || isspace(buffer[base + length]))) {
            length--;
        }
        length++;
        
        // Convert the special hard space characters back into
        // regular spaces.
        decodeHardSpaces(base, length);
        
        // Write out the portion of the buffer we've decided on.
        mOutputStream->write(&buffer[base], length);
        if (!mOutputStream->good()) {
            result = -1;
            break;
        }
        
        // Skip over all of the leading whitespace in the remainder
        // of the string.
        while (length < maxLength
            && isspace(buffer[base + length])) {
            length++;
        }
        
        // Advance to the position in the buffer where the
        // next line starts.
        base += length;
        
        // If requested, all lines after the first line should be indented.
        if (mOptions & INDENT_LINES_AFTER_FIRST) {
            indent = true;
        }
        
        // Add the trailing newline.
        mOutputStream->write("\n", 1);
        if (!mOutputStream->good()) {
            result = -1;
            break;
        }
    }
    
    // Make sure the characters are flushed to the output stream.
    if (mOutputStream->good()) {
        mOutputStream->flush();
    }
    
    // Empty the buffer so we are ready for more characters.
    buffer.clear();

    return result;
}

bool
WordWrappedStreamBuf::outputIsEnabled() const
{
    return LogLevelIsEnabled(mLogLevel);
}

} // namespace con
