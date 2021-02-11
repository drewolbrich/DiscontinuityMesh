// Copyright 2009 Drew Olbrich

#ifndef CON__LOG_LEVEL__INCLUDED
#define CON__LOG_LEVEL__INCLUDED

namespace con {

enum LogLevel {
    LOG_LEVEL_OFF,    // Disable all logging output.
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_ALL     // Enable all logging output.
};

void SetLogLevel(LogLevel logLevel);
LogLevel GetLogLevel();

// Returns true if output at the specified log level is enabled.
bool LogLevelIsEnabled(LogLevel logLevel);

} // namespace con

#endif // CON__LOG_LEVEL__INCLUDED
