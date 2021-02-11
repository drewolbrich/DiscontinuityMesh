// Copyright 2009 Drew Olbrich

#ifndef OS__PLATFORM__INCLUDED
#define OS__PLATFORM__INCLUDED

#ifdef _WIN32
#define OS_WIN32  1
#else
#define OS_WIN32  0
#endif

#if defined(__unix) || defined(__APPLE__)
#define OS_UNIX  1
#else
#define OS_UNIX  0
#endif

#ifdef __APPLE__
#define OS_APPLE  1
#else
#define OS_APPLE  0
#endif

#endif // OS__PLATFORM__INCLUDED
