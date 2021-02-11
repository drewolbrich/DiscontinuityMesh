// Copyright 2010 Drew Olbrich

#ifndef TEMPLATE_BASED_CGMATH__VECTOR_BASE__INCLUDED
#define TEMPLATE_BASED_CGMATH__VECTOR_BASE__INCLUDED

#include <cassert>
#include <algorithm>

// VectorBase
//
// Template class for fixed-length vectors for computer graphics math.

template<typename DERIVED, unsigned SIZE, typename TYPE>
class VectorBase
{
public:
    // For efficiency, the constructor does not initialize the components of the vector.
    VectorBase();
    ~VectorBase();

    // Constructors.
    VectorBase(const DERIVED &rhs);

    // Constructor from an array.
    explicit VectorBase(const TYPE *data);

    // Return the vector as a pointer to the internal data.
    const TYPE *data() const;

    // Assignment.
    DERIVED &operator=(const DERIVED &rhs);
    void swap(DERIVED &rhs);

    // Component access (zero-indexed).
    TYPE operator[](unsigned index) const;
    TYPE &operator[](unsigned index);

    // Equality.
    bool operator==(const DERIVED &rhs) const;
    bool operator!=(const DERIVED &rhs) const;

    // Returns true if the two vectors are equivalent to within the specified tolerance.
    bool equivalent(const DERIVED &rhs, TYPE epsilon) const;

    // Comparison. Required for STL containers.
    bool operator<(const DERIVED &rhs) const;

    // Vector addition.
    DERIVED operator+(const DERIVED &rhs) const;

protected:
    // The components of the vector.
    TYPE mData[SIZE];
};

template<typename DERIVED, unsigned SIZE, typename TYPE> 
VectorBase<DERIVED, SIZE, TYPE>::VectorBase() 
{
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
VectorBase<DERIVED, SIZE, TYPE>::~VectorBase() 
{
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
VectorBase<DERIVED, SIZE, TYPE>::VectorBase(const TYPE *data)
{
    for (unsigned index = 0; index < SIZE; ++index) {
        mData[index] = data[index];
    }
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
const TYPE *VectorBase<DERIVED, SIZE, TYPE>::data() const
{
    return &mData[0];
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
DERIVED &VectorBase<DERIVED, SIZE, TYPE>::operator=(const DERIVED &rhs)
{
    for (unsigned index = 0; index < SIZE; ++index) {
        mData[index] = rhs.mData[index];
    }

    return *this;
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
void VectorBase<DERIVED, SIZE, TYPE>::swap(DERIVED &rhs)
{
    for (unsigned index = 0; index < SIZE; ++index) {
        std::swap(mData[index], rhs.mData[index]);
    }
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
TYPE VectorBase<DERIVED, SIZE, TYPE>::operator[](unsigned index) const
{
    assert(index < SIZE);

    return mData[index];
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
TYPE &VectorBase<DERIVED, SIZE, TYPE>::operator[](unsigned index)
{
    assert(index < SIZE);

    return mData[index];
}

template<typename DERIVED, unsigned SIZE, typename TYPE> 
DERIVED VectorBase<DERIVED, SIZE, TYPE>::operator+(const DERIVED &rhs) const
{
    DERIVED result;
    for (unsigned index = 0; index < SIZE; ++index) {
        result.mData[index] = mData[index] + rhs.mData[index];
    }

    return result;
}

#endif // TEMPLATE_BASED_CGMATH__VECTOR_BASE__INCLUDED
