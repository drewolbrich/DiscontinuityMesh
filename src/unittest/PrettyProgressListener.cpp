// Copyright 2009 Retroactive Fiasco.

#include "PrettyProgressListener.h"

#include <cassert>
#include <iostream>

#include <CppUnit/Test.h>
#include <CppUnit/TestFailure.h>
#include <CppUnit/Message.h>
#include <CppUnit/Exception.h>
#include <CppUnit/SourceLine.h>

#include <con/Streams.h>
#include <con/LogLevel.h>

namespace unittest {

PrettyProgressListener::PrettyProgressListener()
    : CppUnit::TestListener()
{
}

PrettyProgressListener::~PrettyProgressListener()
{
}

void
PrettyProgressListener::startTest(CppUnit::Test *test)
{
    // Print a notification about which test is running, but only do this
    // if the log level is 'info'. We do this rather than simply use con::info,
    // because we don't want to see the name of the test program
    // repeated over and over again.
    if (con::GetLogLevel() == con::LOG_LEVEL_INFO) {
        con::out << "Running test " << test->getName() << std::endl;
    }
}

void 
PrettyProgressListener::addFailure(const CppUnit::TestFailure &testFailure)
{
    // This function outputs a line of text that, compared to what CppUnit's default
    // CompilerOutput class outputs, more closely resembles GCC's output.

    // The line of text is formatted in a particular way that allows XEmacs
    // to parse the filename and line number, which allows us to directly jump to
    // the point of failure in the unit test.

    CppUnit::SourceLine sourceLine = testFailure.sourceLine();
    if (!sourceLine.isValid()) {
        std::cerr << "(Unknown location)";
    } else {
        std::cerr << sourceLine.fileName() << ":" << sourceLine.lineNumber();
    }

    CppUnit::Message message = testFailure.thrownException()->message();
    std::cerr << ": Unit test " << message.shortDescription() << ":";
    for (int index = 0; index < message.detailCount(); ++index) {
        std::cerr << " " << message.detailAt(index);
    }

    std::cerr << '\n';
}

} // namespace unittest
