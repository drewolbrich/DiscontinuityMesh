// Copyright 2008 Drew Olbrich

#ifndef STR__STRING_OPERATIONS__INCLUDED
#define STR__STRING_OPERATIONS__INCLUDED

// This file contains miscellaneous string manipulation functions.

#include <string>

namespace str {

// Given an input string, replace all instances of a source
// string with a target string.
const std::string ReplaceAllSubstrings(const std::string &inputString,
    const std::string &source, const std::string &target);

// Remove all leading and trailing whitespace from a string.
const std::string RemoveLeadingAndTrailingWhitespace(
    const std::string &inputString);

} // namespace str

#endif // STR__STRING_OPERATIONS__INCLUDED
