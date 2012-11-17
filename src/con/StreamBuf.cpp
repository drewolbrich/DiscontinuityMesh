// Copyright 2008 Retroactive Fiasco.

#include "StreamBuf.h"

#include <cassert>

#include <errno.h>

#include "SpecialCharacters.h"

namespace con {

StreamBuf::StreamBuf()
    : std::streambuf(),
      mThreadSpecificBuffer()
{
}

StreamBuf::~StreamBuf()
{
}

void 
StreamBuf::decodeHardSpaces(unsigned int base, unsigned int length)
{
    Buffer &buffer = threadSpecificBuffer();

    // The specified range must lie within the allocated
    // size of the buffer.
    assert(base < buffer.size());
    assert(base + length <= buffer.size());

    // Convert all the hard spaces back into regular spaces.
    for (unsigned int i = base; i < base + length; i++) {
        if (buffer[i] == HARD_SPACE) {
            buffer[i] = ' ';
        }
    }
}

StreamBuf::Buffer &
StreamBuf::threadSpecificBuffer()
{
    if (mThreadSpecificBuffer.get() == NULL) {
        mThreadSpecificBuffer.reset(new Buffer);
    }

    return *mThreadSpecificBuffer;
}

std::string 
StreamBuf::unbrokenString(std::string string)
{
    for (std::string::iterator iterator = string.begin();
         iterator != string.end(); ++iterator) {
        if (*iterator == ' ') {
            *iterator = HARD_SPACE;
        }
    }

    return string;
}

} // namespace con
