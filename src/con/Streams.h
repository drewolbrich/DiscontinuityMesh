// Copyright 2008 Retroactive Fiasco.

#ifndef CON__STREAMS__INCLUDED
#define CON__STREAMS__INCLUDED

#include <iostream>

#include "SpecialCharacters.h"

namespace con {

// The following streams are unconditionally output, regardless of the logging
// levels described below.
//
// con::err    - Like std::err but word-wrapped.
// con::out    - Like std::out but word-wrapped.
// con::indent - Word-wrapped output, indented after the first line
//     
// The following output streams are intended to be used for logged output. Strings are
// automatically prefixed with the name of the application and an indication of the log
// level. Each line is word-wrapped, and lines of text after the first line are indented.
//
// The names of these streams were chosen to be consistent with the log4j library.
//
// con::fatal  - Fatal errors events that should make the application abort.
// con::error  - Errors that might still allow the application to keep running.
// con::warn   - Warnings about potentially harmful situations.
// con::info   - Informational messages that describe coarse-grained progress of the application.
// con::debug  - Fine-grained Debugging information.
// con::trace  - Extremely fine-grained debugging information.
//
// Logging output may be filtered using the con::SetLogLevel function.
//
// Messages sent to these output streams must end with std::endl,
// or they will remain in the output buffer.
//
// Example:
//     using namespace con;
//     con::error << "File not found." << std::endl;
//
// Output:
//     foo error: File not found.
//
// The special character con::HARD_SPACE may be
// output to indicate a hard space character that should not be
// used as a breakpoint during word-wrapping.
//
// Example:
//     con::error << "The next two" << con::HARD_SPACE << "words";
//     con::error << " will not be separated during word wrapping.";
//     con::error << std::endl;
//
// Output:
//     foo error: The next two words will not be separated during
//         word wrapping.
//
// In the example above, the phrase "two words" will never be broken
// up even if it appears at the end of a line.

extern std::ostream err;
extern std::ostream out;
extern std::ostream indent;

// The names of these streams were chosen to be consistent with the log4j library.
extern std::ostream fatal;
extern std::ostream error;
extern std::ostream warn;
extern std::ostream info;
extern std::ostream debug;
extern std::ostream trace;

} // namespace con

#endif // CON__STREAMS__INCLUDED
