// Copyright 2010 Retroactive Fiasco.

#ifndef TEMPLATE_BASED_CGMATH__VECTOR2__INCLUDED
#define TEMPLATE_BASED_CGMATH__VECTOR2__INCLUDED

#include "VectorBase.h"

// Vector2
//
// Vector of length 2.

template<typename TYPE>
class Vector2 : public VectorBase<class Vector2, 2, TYPE>
{
public:
    Vector2();
    ~Vector2();

    Vector2(TYPE c0, TYPE c1);
};

template<typename TYPE> 
Vector2<TYPE>::Vector2()
{
}

template<typename TYPE> 
Vector2<TYPE>::~Vector2()
{
}

template<typename TYPE> 
Vector2<TYPE>::Vector2(TYPE c0, TYPE c1)
{
    this->mData[0] = c0;
    this->mData[1] = c1;
}

#endif // TEMPLATE_BASED_CGMATH__VECTOR2__INCLUDED
