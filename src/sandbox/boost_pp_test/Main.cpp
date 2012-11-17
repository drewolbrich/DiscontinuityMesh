// Copyright 2010 Retroactive Fiasco.

#include <cstdlib>
#include <iostream>

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#define SIZE  5

#define ARGN(n) arg ## n
static void
Foo(
#define ARG(z, n, _) BOOST_PP_COMMA_IF(n) int ARGN(n)
    BOOST_PP_REPEAT(SIZE, ARG, _)
#undef ARG
    )
{
#define PRINT(z, n, _) std::cout << ARGN(n) << std::endl;
    BOOST_PP_REPEAT(SIZE, PRINT, _)
#undef PRINT
}
#undef ARGN

int
main(int, char **)
{
    Foo(2, 4, 6, 8, 10);

    return EXIT_SUCCESS;
}
