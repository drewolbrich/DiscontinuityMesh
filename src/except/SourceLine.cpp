// Copyright 2008 Retroactive Fiasco.

#include "SourceLine.h"

namespace except {

SourceLine::SourceLine()
    : mIsDefined(false),
      mFile(),
      mLine(0)
{
}

SourceLine::SourceLine(const std::string &file, unsigned line)
    : mIsDefined(true),
      mFile(file),
      mLine(line)
{
}

SourceLine::~SourceLine()
{
}

bool
SourceLine::isDefined() const
{
    return mIsDefined;
}

const std::string &
SourceLine::file() const
{
    return mFile;
}

unsigned 
SourceLine::line() const
{
    return mLine;
}

std::ostream &
operator<<(std::ostream &ostr, const SourceLine &sourceLine)
{
    return ostr << sourceLine.file() << ", line " << sourceLine.line();
}

} // namespace except
