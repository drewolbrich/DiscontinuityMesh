// Copyright 2008 Drew Olbrich

#ifndef CON__WORD_WRAPPED_STREAM_BUF__INCLUDED
#define CON__WORD_WRAPPED_STREAM_BUF__INCLUDED

#include <iostream>
#include <string>

#include "StreamBuf.h"
#include "LogLevel.h"

namespace con {

// WordWrappedStreamBuf
//
// This class is an extension of std::streambuf that word-wraps
// text at the edge of the terminal window. It can optionally
// show the name of the currently executing program
// and a prefix like "warning:" or "error:" at the beginning
// of the first line of each block of output. Successive lines
// of text can also optionally be indented.
//
// Example output of a stream based on WordWrappedStreamBuf
// for a narrow terminal window:
//
// obj_view warning: Cannot load OBJ file 
//     "shrek_boxer_shorts.obj" (No such file
//     or directory)
//
// Since WordWrappedStreamBuf inherits from std::streambuf,
// it typically cannot be used by itself. Instead, an object of
// type std::stream is created, with the address of 
// a WordWrappedStreamBuf object passed to its a constructor:
//
// WordWrappedStreamBuf gErrWordWrappedStreamBuf(&clog, "error",
//     WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
//     | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
//     | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);
// std::ostream error(&gErrWordWrappedStreamBuf);
// error << "Cannot load OBj file" << std::endl;
//
// It is important that messages sent to a std::ostream that
// uses WordWrappedStreamBuf always end with std::endl.
// Otherwise, the text remains buffered and will never be printed.
// Ending a string with "\n" by itself is not enough.
//
// Hard spaces, which will not be used as a breakpoint during
// word-wrapping, can be inserted by writing the value
// con::HARD_SPACE to the stream buffer.

class WordWrappedStreamBuf : public StreamBuf
{
public:
    WordWrappedStreamBuf(std::ostream *outputStream, 
        const std::string &secondaryPrefix, 
        LogLevel logLevel,
        const unsigned int options = 0);
    virtual ~WordWrappedStreamBuf();

    // Optional flag arguments passed to the constructor.
    enum Options {
        // The first line of each output block will be prefixed
        // with the name of the executing program.
        SHOW_PROGRAM_NAME_PREFIX  = 0x01,

        // Lines after the first line in an output block will
        // be indented.
        INDENT_LINES_AFTER_FIRST  = 0x02,

        // The width of the terminal will be dynamically queried
        // to determine when to word wrap. If this option is not
        // specified, a fixed value of 80 columns will always be used.
        QUERY_TERMINAL_WIDTH      = 0x04
    };
    
private:
    // For std::streambuf:
    virtual int overflow(int c);
    virtual std::streamsize xsputn(const char *s, std::streamsize n);
    virtual int sync();

    // Returns the width of the terminal, in characters.
    int getTerminalWidth();

    // Appends the program name and message prefix to the
    // end of the buffer.
    void appendPrefix();

    // Appends a string to the buffer. The string does not have
    // to be null-terminated.
    void appendString(const char *s, const int n);

    // Writes the contents of the buffer to the output stream.
    int flushBuffer();

    // Returns true if output is enabled at the current log level setting.
    bool outputIsEnabled() const;

    // The default width of the terminal that we use if we cannot
    // determine the actual value.
    static const int DEFAULT_TERMINAL_WIDTH = 80;

    // This is the width of the margin that should appear on the right
    // side of the terminal window, in characters. Lines of text will
    // be word-wrapped before this point. A small margin is included
    // so that in the event the output text is copied into email and
    // then quoted, it will still take up less than 80 columns.
    static const int RIGHT_MARGIN = 5;

    // If true, this object's constructor has been called.
    // This is used to check if global instances are being used
    // before they have been constructed.
    bool mConstructorWasCalled;

    // The stream to send all output to.
    std::ostream *mOutputStream;

    // This string follows the program name at the beginning
    // of the first line of each output block.
    std::string mSecondaryPrefix;

    // The log level associated with the stream.
    // If the current log level chosen by the user is less than 
    // this log level, stream output is disabled.
    LogLevel mLogLevel;

    // Set of bit flags of optional modes of behavior, defined above.
    unsigned int mOptions;
};

} // namespace con

#endif // CON__WORD_WRAPPED_STREAM_BUF__INCLUDED
