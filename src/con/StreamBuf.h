// Copyright 2008 Drew Olbrich

#ifndef CON__STREAM_BUF__INCLUDED
#define CON__STREAM_BUF__INCLUDED

#include <iostream>
#include <vector>
#include <string>

#include <boost/thread/tss.hpp>

// Forward declaration for the friend declaration that comes later.
class StreamBufTest;

namespace con {

// StreamBuf
//
// The class is a base class for stream buffers based
// on std::streambuf, providing a layer of support for
// thread-safe operation.

class StreamBuf : public std::streambuf
{
    // This allows StreamBuf's unit test to access its private members.
    friend class ::StreamBufTest;

public:
    // Return a string where the space characters of the string
    // have been converted to unbroken spaces.
    static std::string unbrokenString(std::string string);
    
protected:
    StreamBuf();
    virtual ~StreamBuf();

    // The type definition of the vector of characters used as
    // thread-specific buffer for output.
    typedef std::vector<char> Buffer;

    // Converts the special hard space characters back into
    // regular spaces.
    void decodeHardSpaces(unsigned int base, unsigned int length);

    // Returns a reference to the thread-specific character buffer.
    Buffer &threadSpecificBuffer();

private:
    // A reference to the character buffer uniquely associated with
    // each thread.
    boost::thread_specific_ptr<Buffer> mThreadSpecificBuffer;
};

} // namespace con

#endif // CON__STREAM_BUF__INCLUDED
