// Copyright 2008 Drew Olbrich

#include <cppunit/extensions/HelperMacros.h>

#include <con/StreamBuf.h>
#include <con/ProgramName.h>
#include <con/Streams.h>

using con::StreamBuf;

class StreamBufTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(StreamBufTest);
    CPPUNIT_TEST(testStreamBuf);
    CPPUNIT_TEST_SUITE_END();

public:
    class UnitTestStreamBuf : public con::StreamBuf
    {
    };

    void setUp() {
    }

    void tearDown() {
    }

    void testStreamBuf() {
        UnitTestStreamBuf unitTestStreamBuf;

        StreamBuf::Buffer &buffer = unitTestStreamBuf.threadSpecificBuffer();
        buffer.push_back(con::HARD_SPACE);
        buffer.push_back('B');
        buffer.push_back(con::HARD_SPACE);
        buffer.push_back('C');
        buffer.push_back(con::HARD_SPACE);

        unitTestStreamBuf.decodeHardSpaces(1, 3);
        CPPUNIT_ASSERT(buffer[0] == con::HARD_SPACE);
        CPPUNIT_ASSERT(buffer[1] == 'B');
        CPPUNIT_ASSERT(buffer[2] == ' '); 
        CPPUNIT_ASSERT(buffer[3] == 'C');
        CPPUNIT_ASSERT(buffer[4] == con::HARD_SPACE);

        std::string testString = StreamBuf::unbrokenString("A B C");
        CPPUNIT_ASSERT(testString[0] == 'A');
        CPPUNIT_ASSERT(testString[1] == con::HARD_SPACE);
        CPPUNIT_ASSERT(testString[2] == 'B');
        CPPUNIT_ASSERT(testString[3] == con::HARD_SPACE);
        CPPUNIT_ASSERT(testString[4] == 'C');
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(StreamBufTest);
