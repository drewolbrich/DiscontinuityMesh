// Copyright 2008 Retroactive Fiasco.

#ifndef MESH__ATTRIBUTE_DATA__INCLUDED
#define MESH__ATTRIBUTE_DATA__INCLUDED

#include "AttributeKey.h"

namespace mesh {

// AttributeData
//
// This class holds a pointer to an arbitrary data type. Even though
// the data itself is pointed to separately, it should work properly
// when used as a value, so you can stick them in STL containers.
// 
// If the extra memory usage ever becomes a problem, perhaps this
// could be reimplemented in terms of boost::pool.

class AttributeData {
public:
    AttributeData(AttributeKey attributeKey);
    ~AttributeData();

    AttributeData(const AttributeData &rhs);
    AttributeData &operator=(const AttributeData &rhs);
    
    AttributeKey::Handle handle() const { return mHandle; }
    AttributeKey::Type type() const { return mType; }
    void *data() const { return mData; }

    // Copy only the data pointed to, not the handle or type.
    void copyData(const AttributeData &rhs);

private:
    // This should never be called. An AttributeData object must
    // always be created from an AttributeKey, or by copying another
    // AttributeData object.
    AttributeData();

    void allocateData();
    void deallocateData();

    AttributeKey::Handle mHandle;
    AttributeKey::Type mType;
    void *mData;
};

} // namespace mesh

#endif // MESH__ATTRIBUTE_DATA__INCLUDED
