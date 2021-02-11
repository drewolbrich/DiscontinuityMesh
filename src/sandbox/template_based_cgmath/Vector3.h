// Copyright 2010 Drew Olbrich

#ifndef TEMPLATE_BASED_CGMATH__VECTOR3__INCLUDED
#define TEMPLATE_BASED_CGMATH__VECTOR3__INCLUDED

#include "VectorBase.h"

// Vector3
//
// Vector of length 3.

template<typename TYPE>
class Vector3 : public VectorBase<class Vector3, 3, TYPE>
{
public:
    Vector3();
    ~Vector3();

    Vector3(TYPE c0, TYPE c1, TYPE c2);
};

template<typename TYPE> 
Vector3<TYPE>::Vector3()
{
}

template<typename TYPE> 
Vector3<TYPE>::~Vector3()
{
}

template<typename TYPE> 
Vector3<TYPE>::Vector3(TYPE c0, TYPE c1, TYPE c2)
{
    this->mData[0] = c0;
    this->mData[1] = c1;
    this->mData[2] = c2;
}

#endif // TEMPLATE_BASED_CGMATH__VECTOR3__INCLUDED
