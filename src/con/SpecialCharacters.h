// Copyright 2009 Drew Olbrich

#ifndef CON__SPECIAL_CHARACTERS__INCLUDED
#define CON__SPECIAL_CHARACTERS__INCLUDED

namespace con {

// If the following ISO 8859-1 non-breaking space character is
// placed in the output stream, line breaks during word wrapping
// will never appear where it is specified. During output
// it will be converted to a regular ASCII space character.
extern const char HARD_SPACE;

} // namespace con

#endif // CON__SPECIAL_CHARACTERS__INCLUDED
