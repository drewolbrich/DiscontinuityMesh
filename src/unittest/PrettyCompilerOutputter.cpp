// Copyright 2008 Drew Olbrich

#include "PrettyCompilerOutputter.h"

#include <cassert>

#include <cppunit/TestResultCollector.h>

#include <con/Streams.h>

namespace unittest {

PrettyCompilerOutputter::PrettyCompilerOutputter(CppUnit::TestResultCollector *result, 
    CppUnit::OStream &stream, const std::string &locationFormat)
    : CppUnit::CompilerOutputter(result, stream, locationFormat),
      mTestResultCollector(result)
{
}

PrettyCompilerOutputter::~PrettyCompilerOutputter()
{
}

void 
PrettyCompilerOutputter::printSuccess()
{
    con::info << "All tests passed." << std::endl;
}

void
PrettyCompilerOutputter::printFailureDetail(CppUnit::TestFailure *)
{
    // The failure detail is printed by PrettyProgressListener::addFailure
    // immediately after the failure occurs, rather than in this function,
    // which is only called after all the unit tests have run.
    // This is consistent with the behavior of the C++ compiler, which prints out
    // errors as they happen.
    // Consequently, when we put print statements in our unit tests to debug them,
    // they'll show up in the correct order relative to the failure notifications.
}

void 
PrettyCompilerOutputter::printStatistics()
{
    // This function is only called by CppUnit when failures occur.

    con::error << mTestResultCollector->testFailuresTotal()
        << " of " << mTestResultCollector->runTests()
        << " test" << (mTestResultCollector->runTests() == 1 ? "" : "s")
        << " failed." << std::endl;
}

} // namespace unittest
