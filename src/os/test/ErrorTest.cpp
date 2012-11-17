// Copyright 2008 Retroactive Fiasco.

#include <cppunit/extensions/HelperMacros.h>
#include <cstdio>
#include <sstream>
#include <errno.h>

#include <os/Platform.h>
#include <os/Error.h>

using os::Error;

class ErrorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ErrorTest);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testFromSystemError);
    CPPUNIT_TEST(testStreamOutput);
    CPPUNIT_TEST(testCopyConstructor);
    CPPUNIT_TEST(testAssignment);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testConstructor() {
        Error error;
        CPPUNIT_ASSERT(!error.isDefined());
        CPPUNIT_ASSERT(error.success());
    }

    void testFromSystemError() {
        FILE *file = fopen("/retro-except-nonexistant-file", "r");
        CPPUNIT_ASSERT(file == NULL);
        Error error = Error::fromSystemError();

        CPPUNIT_ASSERT(error.isDefined());
        CPPUNIT_ASSERT(!error.success());

#if OS_UNIX
        CPPUNIT_ASSERT(error.asString() == "No such file or directory");
#endif        
#if OS_WIN32
        CPPUNIT_ASSERT(error.asString() == "The system cannot find the file specified.");
#endif
    }

    void testStreamOutput() {
        FILE *file = fopen("/retro-except-nonexistant-file", "r");
        CPPUNIT_ASSERT(file == NULL);
        Error error = Error::fromSystemError();

        std::ostringstream ostr;
        ostr << error;
#if OS_UNIX
        CPPUNIT_ASSERT(ostr.str() == "No such file or directory");
#endif
#if OS_WIN32
        CPPUNIT_ASSERT(ostr.str() == "The system cannot find the file specified.");
#endif
    }

    void testCopyConstructor() {
        FILE *file = fopen("/retro-except-nonexistant-file", "r");
        CPPUNIT_ASSERT(file == NULL);
        Error error = Error::fromSystemError();

        Error errorCopy(error);
        CPPUNIT_ASSERT(error.isDefined());
        CPPUNIT_ASSERT(errorCopy.isDefined());
        CPPUNIT_ASSERT(!error.success());
        CPPUNIT_ASSERT(!errorCopy.success());
#if OS_UNIX
        CPPUNIT_ASSERT(error.asString() == "No such file or directory");
        CPPUNIT_ASSERT(errorCopy.asString() == "No such file or directory");
#endif        
#if OS_WIN32
        CPPUNIT_ASSERT(error.asString() == "The system cannot find the file specified.");
        CPPUNIT_ASSERT(errorCopy.asString() == "The system cannot find the file specified.");
#endif
    }

    void testAssignment() {
        FILE *file = fopen("/retro-except-nonexistant-file", "r");
        CPPUNIT_ASSERT(file == NULL);
        Error error = Error::fromSystemError();

        Error errorCopy = error;
        CPPUNIT_ASSERT(error.isDefined());
        CPPUNIT_ASSERT(errorCopy.isDefined());
        CPPUNIT_ASSERT(!error.success());
        CPPUNIT_ASSERT(!errorCopy.success());
#if OS_UNIX
        CPPUNIT_ASSERT(error.asString() == "No such file or directory");
        CPPUNIT_ASSERT(errorCopy.asString() == "No such file or directory");
#endif        
#if OS_WIN32
        CPPUNIT_ASSERT(error.asString() == "The system cannot find the file specified.");
        CPPUNIT_ASSERT(errorCopy.asString() == "The system cannot find the file specified.");
#endif
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorTest);
