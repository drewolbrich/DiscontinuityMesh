// Copyright 2008 Retroactive Fiasco.

#include "Streams.h"

#include <cstdio>
#include <iostream>

#include "LogLevel.h"
#include "WordWrappedStreamBuf.h"

namespace con {

// The output stream clog is used instead of cerr because clog is buffered by line,
// whereas cerr is buffered by character, and is less efficient.

WordWrappedStreamBuf gErrWordWrappedStreamBuf(&std::clog, "", LOG_LEVEL_OFF,
    WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gOutWordWrappedStreamBuf(&std::cout, "", LOG_LEVEL_OFF,
    WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gIndentWordWrappedStreamBuf(&std::cout, "", LOG_LEVEL_OFF,
    WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

std::ostream err(&gErrWordWrappedStreamBuf);
std::ostream out(&gOutWordWrappedStreamBuf);
std::ostream indent(&gIndentWordWrappedStreamBuf);

WordWrappedStreamBuf gFatalWordWrappedStreamBuf(&std::clog, "fatal", LOG_LEVEL_FATAL,
    WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
    | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gErrorWordWrappedStreamBuf(&std::clog, "error", LOG_LEVEL_ERROR,
    WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
    | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gWarnWordWrappedStreamBuf(&std::clog, "warning", LOG_LEVEL_WARN,
    WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
    | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gInfoWordWrappedStreamBuf(&std::cout, "", LOG_LEVEL_INFO,
    WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
    | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gDebugWordWrappedStreamBuf(&std::cout, "", LOG_LEVEL_DEBUG,
    WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
    | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

WordWrappedStreamBuf gTraceWordWrappedStreamBuf(&std::cout, "", LOG_LEVEL_TRACE,
    WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
    | WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST
    | WordWrappedStreamBuf::QUERY_TERMINAL_WIDTH);

std::ostream fatal(&gFatalWordWrappedStreamBuf);
std::ostream error(&gErrorWordWrappedStreamBuf);
std::ostream warn(&gWarnWordWrappedStreamBuf);
std::ostream info(&gInfoWordWrappedStreamBuf);
std::ostream debug(&gDebugWordWrappedStreamBuf);
std::ostream trace(&gTraceWordWrappedStreamBuf);

} // namespace con
