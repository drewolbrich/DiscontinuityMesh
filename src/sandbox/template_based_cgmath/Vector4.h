// Copyright 2010 Retroactive Fiasco.

#ifndef TEMPLATE_BASED_CGMATH__VECTOR4__INCLUDED
#define TEMPLATE_BASED_CGMATH__VECTOR4__INCLUDED

#include "VectorBase.h"

// Vector4
//
// Vector of length 4.

template<typename TYPE>
class Vector4 : public VectorBase<class Vector4, 4, TYPE>
{
public:
    Vector4();
    ~Vector4();

    Vector4(TYPE c0, TYPE c1, TYPE c2, TYPE c3);
};

template<typename TYPE> 
Vector4<TYPE>::Vector4()
{
}

template<typename TYPE> 
Vector4<TYPE>::~Vector4()
{
}

template<typename TYPE> 
Vector4<TYPE>::Vector4(TYPE c0, TYPE c1, TYPE c2, TYPE c3)
{
    this->mData[0] = c0;
    this->mData[1] = c1;
    this->mData[2] = c2;
    this->mData[3] = c3;
}

#endif // TEMPLATE_BASED_CGMATH__VECTOR4__INCLUDED
