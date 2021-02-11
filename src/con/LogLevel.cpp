// Copyright 2009 Drew Olbrich

#include "LogLevel.h"

namespace con {

static LogLevel gLogLevel = LOG_LEVEL_WARN;

void
SetLogLevel(LogLevel logLevel)
{
    gLogLevel = logLevel;
}

LogLevel 
GetLogLevel()
{
    return gLogLevel;
}

bool
LogLevelIsEnabled(LogLevel logLevel)
{
    return gLogLevel >= logLevel;
}

} // namespace con
