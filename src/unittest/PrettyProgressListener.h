// Copyright 2009 Drew Olbrich

#ifndef UNITTEST__PRETTY_PROGRESS_LISTENER__INCLUDED
#define UNITTEST__PRETTY_PROGRESS_LISTENER__INCLUDED

#include <string>

#include <cppunit/TestListener.h>

namespace unittest {

// PrettyProgressListener
//
// This class is a variation on CppUnit::TestListener whose output
// isn't quite as ugly.

class PrettyProgressListener : public CppUnit::TestListener
{
public:
    PrettyProgressListener();
    virtual ~PrettyProgressListener();

    // For CppUnit::TestListener:
    virtual void startTest(CppUnit::Test *test);
    virtual void addFailure(const CppUnit::TestFailure &testFailure);
};

} // namespace unittest

#endif // UNITTEST__PRETTY_PROGRESS_LISTENER__INCLUDED
