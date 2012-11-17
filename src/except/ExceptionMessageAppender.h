// Copyright 2010 Retroactive Fiasco.

#ifndef EXCEPT__EXCEPTION_MESSAGE_APPENDER__INCLUDED
#define EXCEPT__EXCEPTION_MESSAGE_APPENDER__INCLUDED

#include "Exception.h"

namespace except {

// ExceptionMessageAppender
//
// Template class that all instantiatable Exception-derived classes should
// derive from. Before an exception is thrown, this allows a descriptive message
// to be assigned to the exception using the stream operator.
//
// This is a template class so that the return type of the stream operator
// can match the exception being thrown. If we implemented this without a template,
// we'd be forced to return a reference to some other type, like the Exception base class,
// which would cause slicing. When the exception was caught, we'd lose the ability
// to tell what the original thrown type was.
// 
// Example:
//     throw except::FailedOperationException(SOURCE_LINE)
//         << "Syntax error in file \"" << filename << "\".";

template<class DERIVED_CLASS>
class ExceptionMessageAppender
{
public:
    ExceptionMessageAppender() {
    }

    virtual ~ExceptionMessageAppender() {
    }

    template<typename TYPE> DERIVED_CLASS &operator<<(TYPE value) {
        dynamic_cast<Exception *>(this)->appendToMessage(value);
        return dynamic_cast<DERIVED_CLASS &>(*this);
    }
};

} // namespace except

#endif // EXCEPT__EXCEPTION_MESSAGE_APPENDER__INCLUDED
