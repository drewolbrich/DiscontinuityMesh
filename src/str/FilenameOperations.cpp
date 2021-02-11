// Copyright 2008 Drew Olbrich

#include "FilenameOperations.h"

#include <cassert>

namespace str {

std::string 
GetFilenamePath(const std::string &filename)
{
    std::string::size_type pos = filename.rfind('/');
    if (pos == std::string::npos) {
        return "";
    }
    
    return filename.substr(0, pos + 1);
}

std::string 
GetFilenameExtension(const std::string &filename)
{
    std::string::size_type pos = filename.rfind('.');
    if (pos == std::string::npos
        || pos == filename.length() - 1) {
        return "";
    }
    
    return filename.substr(pos + 1);
}

std::string 
RemoveFilenameExtension(const std::string &filename)
{
    std::string::size_type pos = filename.rfind('.');
    if (pos == std::string::npos) {
        return filename;
    }
    
    return filename.substr(0, pos);
}

std::string 
RemovePathFromFilename(const std::string &filename)
{
    std::string::size_type pos = filename.rfind('/');
    if (pos == std::string::npos) {
        return filename;
    }

    return filename.substr(pos + 1);
}

std::string 
GetIncludedFilename(std::string parentFile, const std::string &includedFile)
{
    // If includedFile is an absolute path reference like
    // "/usr/tmp/foo.x", return it directly, because the result is
    // independent of parentFile in that case.
    if (includedFile.length() > 0
        && includedFile[0] == '/') {
        return includedFile;
    }
    
    // Strip the filename off parentFile's path.
    std::string::size_type pos = parentFile.rfind('/');
    if (pos != std::string::npos) {
        // Remove everything after the last slash in the string.
        parentFile.resize(pos + 1);
    } else {
        // parentFile refers to the local directory, so clear it.
        parentFile = "";
    }

    return parentFile + includedFile;
}

std::string 
LocalizeFilename(const std::string &filename, const std::string &path)
{
    // The specified path must either be the empty string (if it refers
    // to the current directory) or it must end in a slash.
    assert(path == "" || path.rfind('/') == path.length() - 1);

    std::string::size_type pos = filename.find(path);
    if (pos == 0) {
        return filename.substr(path.length());
    }

    return filename;
}

} // namespace str
