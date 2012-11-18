// Copyright 2008 Retroactive Fiasco.

#ifndef EXACT__PREDICATES__INCLUDED
#define EXACT__PREDICATES__INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// The following are prototypes for the functions in the file predicates.c,
// which didn't come with a header file.

void exactinit();
float orient2d(float *pa, float *pb, float *pc);
float orient3d(float *pa, float *pb, float *pc, float *pd);
float incircle(float *pa, float *pb, float *pc, float *pd);
float insphere(float *pa, float *pb, float *pc, float *pd, float *pe);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EXACT__PREDICATES__INCLUDED
