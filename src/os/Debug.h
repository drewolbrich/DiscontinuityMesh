// Copyright 2009 Drew Olbrich

#ifndef OS__DEBUG__INCLUDED
#define OS__DEBUG__INCLUDED

#include "Platform.h"

namespace os {

#ifdef OS_UNIX

// Invokes the debugger if it is running.
void Break();

#else

#error "Not yet implemented."

#endif

} // namespace os

#endif // OS__DEBUG__INCLUDED
