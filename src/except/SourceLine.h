// Copyright 2008 Retroactive Fiasco.

#ifndef EXCEPT__SOURCE_LINE__INCLUDED
#define EXCEPT__SOURCE_LINE__INCLUDED

#include <iostream>
#include <string>

// Pass the SOURCE_LINE macro to exceptions that you're throwing, to embed the current
// file, line number.
//
// For example:
//     throw except::FailedOperationException(SOURCE_LINE) << "Could not open socket.";

#define SOURCE_LINE except::SourceLine(__FILE__, __LINE__)

namespace except {

// SourceLine
//
// This class holds information about the file and line number
// that an exception was thrown from.

class SourceLine
{
public:
    SourceLine();

    SourceLine(const std::string &file, unsigned line);

    ~SourceLine();

    // True if the source line was defined using the above constructor that has parameters, 
    // as opposed to holding default values.
    bool isDefined() const;

    // The name of the file the exception was thrown from.
    const std::string &file() const;

    // The line the exception was thrown from.
    unsigned line() const;

    // Print the file and line number to a stream, separated by commas.
    friend std::ostream &operator<<(std::ostream &ostr, const SourceLine &sourceLine);

private:
    bool mIsDefined;
    std::string mFile;
    unsigned mLine;
};

} // namespace except

#endif // EXCEPT__SOURCE_LINE__INCLUDED
