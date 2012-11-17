// Copyright 2008 Retroactive Fiasco.

#include "StringOperations.h"

#include <ctype.h>

namespace str {

const std::string 
ReplaceAllSubstrings(const std::string &inputString,
    const std::string &source, const std::string &target)
{
    std::string result(inputString);
    
    std::string::size_type pos = 0;
    while (true) {

        // Find the next instance of the source string.
        pos = result.find(source, pos);

        // If there are no more matches, we're done.
        if (pos == std::string::npos) {
            break;
        }

        // Replace the source string with the target string.
        result.replace(pos, source.size(), target);

        // Skip over the part of the string we just replaced,
        // so the next time we search for the source string,
        // we won't get stuck in an infinite loop if the
        // target string contains the source string.
        pos += target.size();
    }

    return result;
}

const std::string 
RemoveLeadingAndTrailingWhitespace(const std::string &inputString)
{
    std::string result(inputString);

    // Remove leading whitespace.
    std::string::size_type pos = 0;
    while (pos < result.size()
        && isspace(result[pos])) {
        ++pos;
    }
    result = result.substr(pos);
    
    // Remove trailing whitespace.
    pos = result.size() - 1;
    while (pos > 0
        && isspace(result[pos])) {
        --pos;
    }
    result.resize(pos + 1);

    return result;
}

} // namespace str
