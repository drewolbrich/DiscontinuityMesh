// Copyright 2009 Drew Olbrich

#ifndef OS__TIME__INCLUDED
#define OS__TIME__INCLUDED

#include <os/TimeValue.h>

namespace os {

// Returns the current time using gettimeofday under Unix. Under Windows,
// GetSystemTimeAsFileTime is used, but it is mapped from Windows epoch 1601 to Unix epoch
// 1970, so the values are consistent and can be compared if transmitted over a network.
TimeValue GetCurrentTime();

// Returns the user time used by the process as reported by 'getrusage'.
TimeValue GetProcessUserTime();

} // namespace os

#endif // OS__TIME__INCLUDED
