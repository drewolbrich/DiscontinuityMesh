// Copyright 2010 Drew Olbrich

#include "Debug.h"

namespace os {

#ifdef OS_UNIX

void 
Break() 
{
    asm("int $3"); 
}

#endif

} // namespace os
