// Copyright 2009 Retroacive Fiasco.

#ifndef CGMATH__TRIBOX2__INCLUDED
#define CGMATH__TRIBOX2__INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// The following is a prototype for the function in tribox2.c,
// which didn't come with a header file.

int triBoxOverlap(float boxcenter[3], float boxhalfsize[3], float triverts[3][3]);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CGMATH__TRIBOX2__INCLUDED
