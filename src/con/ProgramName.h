// Copyright 2008 Retroactive Fiasco.

#ifndef CON__PROGRAM_NAME__INCLUDED
#define CON__PROGRAM_NAME__INCLUDED

#include <string>

namespace con {

// Sets the name of the program. When the program starts,
// argv[0] should be immediately passed to this function.
void SetProgramName(const std::string &programName);

// Returns the name of the program.
const std::string &GetProgramName();

// Functions to temporarily save and restore an existing program name.
// Only intended to be used by unit tests.
void PushProgramName();
void PopProgramName();

} // namespace con

#endif // CON__PROGRAM_NAME__INCLUDED
