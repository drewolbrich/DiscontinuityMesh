// Copyright 2008 Drew Olbrich

#ifndef UNITTEST__PRETTY_COMPILER_OUTPUTTER__INCLUDED
#define UNITTEST__PRETTY_COMPILER_OUTPUTTER__INCLUDED

#include <string>

#include <cppunit/CompilerOutputter.h>

namespace unittest {

// PrettyCompilerOutputter
//
// This class is a variation on CppUnit::CompilerOutputter whose output
// isn't quite as ugly.

class PrettyCompilerOutputter : public CppUnit::CompilerOutputter
{
public:
    PrettyCompilerOutputter(CppUnit::TestResultCollector *result, CppUnit::OStream &stream, 
        const std::string &locationFormat = CPPUNIT_COMPILER_LOCATION_FORMAT);
    virtual ~PrettyCompilerOutputter();

    // For CppUnit::CompilerOutputter:
    virtual void printSuccess();
    virtual void printFailureDetail(CppUnit::TestFailure *testFailure);
    virtual void printStatistics();

private:
    CppUnit::TestResultCollector *mTestResultCollector;
};

} // namespace unittest

#endif // UNITTEST__PRETTY_COMPILER_OUTPUTTER__INCLUDED
