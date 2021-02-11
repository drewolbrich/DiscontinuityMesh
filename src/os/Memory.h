// Copyright 2010 Drew Olbrich

#ifndef OS__MEMORY__INCLUDED
#define OS__MEMORY__INCLUDED

#include <string>

namespace os {

// Return the number of kilobytes used by the process as reported by 'getrusage'.
unsigned long GetProcessKilobytesUsed();

// Return the amount of memory used by the process as a human-readable string.
std::string GetProcessMemoryUsedAsString();

} // namespace os

#endif // OS__MEMORY__INCLUDED
