// Copyright 2008 Drew Olbrich

#ifndef STR__FILENAME_OPERATIONS__INCLUDED
#define STR__FILENAME_OPERATIONS__INCLUDED

#include <string>

namespace str {

// Returns the path component of a filename, including the
// trailing slash. If the filename has no path component,
// the empty string is returned.
std::string GetFilenamePath(const std::string &filename);

// Returns the extension of a filename, not including the period.
// If the filename has no extension, the empty string is returned.
std::string GetFilenameExtension(const std::string &filename);

// Remove the extension from the filename. If the the filename
// has no extension, it is returned unmodified.
std::string RemoveFilenameExtension(const std::string &filename);

// Remove the path specification from a filename. For example,
// "foo/bar/baz.xxx" becomes "baz.xxx". If the filename has no
// path specification, it is returned unmodified.
std::string RemovePathFromFilename(const std::string &filename);

// Given a filename (parentFile) in which another file
// (includedFile) was referenced for reading, return a path name to
// includedFile that can be reached from the current directory, even
// if parentFile's filename path references another directory.
//
// Examples:
//
//     parentFile          includedFile        result
//     ------------------  ------------------  ------------------
//     a.x                 b.x                 b.x
//     foo/bar/a.x         b.x                 foo/bar/b.x
//     foo/bar/a.x         baz/b.x             foo/bar/baz/b.x
//     foo/bar/a.x         /usr/tmp/b.x        /usr/tmp/b.x
//     foo/bar/a.x         ../b.x              foo/bar/../b.x
//
std::string GetIncludedFilename(std::string parentFile,
    const std::string &includedFile);

// Localize a filename's path relative to a specified path. For
// example, if filename is "foo/bar/baz.jpg" and path is "foo/", then
// the resulting filename will be "bar/baz.jpg". The specified path
// must either end with a slash or be the empty string if it refers to
// the current directory.
std::string LocalizeFilename(const std::string &filename, 
    const std::string &path);

} // namespace str

#endif // STR__FILENAME_OPERATIONS__INCLUDED
