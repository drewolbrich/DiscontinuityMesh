// Copyright 2008 Retroactive Fiasco.

#include <iostream>
#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <con/WordWrappedStreamBuf.h>
#include <con/StreamBuf.h>
#include <con/ProgramName.h>

using con::WordWrappedStreamBuf;

class WordWrappedStreamBufTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(WordWrappedStreamBufTest);
    CPPUNIT_TEST(testWordWrappedStreamBuf);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {
    }

    void testWordWrappedStreamBuf() {
        struct SafePushProgramName {
            SafePushProgramName() { con::PushProgramName(); }
            ~SafePushProgramName() { con::PopProgramName(); }
        } safePushProgramName;

        con::SetProgramName("test");

        // This class sets the log level and restores it when the class instance
        // goes out of scope, even if an exception is thrown.
        class SafeLogLevelSetter {
        public:
            SafeLogLevelSetter(con::LogLevel logLevel) {
                mOldLogLevel = con::GetLogLevel();
                con::SetLogLevel(logLevel);
            }
            ~SafeLogLevelSetter() {
                con::SetLogLevel(mOldLogLevel);
            }
        private:
            con::LogLevel mOldLogLevel;
        } safeLogLevelSetter(con::LOG_LEVEL_ALL);

        std::ostringstream sstr;

        con::WordWrappedStreamBuf wordWrappedStreamBuf(&sstr,
            "prefix", con::LOG_LEVEL_OFF,
            con::WordWrappedStreamBuf::SHOW_PROGRAM_NAME_PREFIX 
            | con::WordWrappedStreamBuf::INDENT_LINES_AFTER_FIRST);
        std::ostream ostr(&wordWrappedStreamBuf);

        ostr << "This is a test of a line that is very long and should "
            << "wrap around at 80 characters." << std::endl;

        CPPUNIT_ASSERT(sstr.str() == "test prefix: This is a "
            "test of a line that is very long and should wrap\n    around "
            "at 80 characters.\n");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WordWrappedStreamBufTest);
