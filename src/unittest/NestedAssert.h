// Copyright 2008 Drew Olbrich

#ifndef UNITTEST__NESTED_ASSERT__INCLUDED
#define UNITTEST__NESTED_ASSERT__INCLUDED

#include <string>
#include <boost/lexical_cast.hpp>

namespace unittest {

// Within a CppUnit test, calls an auxilliary function and asserts that the return
// value of that function is true, including a message assigned by the
// UNITTEST_NESTED_ASSERT macro, below, that returns the line number of the
// failed test within the called function.
#define UNITTEST_NESTED_CALL(x) \
    if (!(x)) { \
        CPPUNIT_FAIL(std::string("Failed assertion expression: ") \
            + #x + " " + unittest::GetNestedAssertMessage()); \
    }

// Like an assertion but sets a global message with the current line number
// that is intended to be later used by the UNITTEST_NESTED_CALL macro above,
// and then returns false so that a real CppUnit assertion can be triggered
// by that macro.
#define UNITTEST_NESTED_ASSERT(x) \
    if (!(x)) { \
        unittest::SetNestedAssertMessage( \
            std::string("(Nested assertion failed at line ") \
            + boost::lexical_cast<std::string>(__LINE__) \
            + std::string(")")); \
        return false; \
    }

void SetNestedAssertMessage(const std::string &message);
const std::string &GetNestedAssertMessage();

} // namespace unittest

#endif // UNITTEST__NESTED_ASSERT__INCLUDED
