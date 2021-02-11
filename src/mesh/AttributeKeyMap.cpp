// Copyright 2008 Drew Olbrich

#include "AttributeKeyMap.h"

#include <cassert>

namespace mesh {

AttributeKeyMap::AttributeKeyMap()
    : mMap(),
      // The first handle assigned is 1, because 0 is an illegal 
      // handle number.
      mNextHandle(1)
{
}

AttributeKeyMap::~AttributeKeyMap()
{
}

void 
AttributeKeyMap::swap(AttributeKeyMap &rhs)
{
    // This should be fast because a constant number of pointers
    // are swapped, rather than the complete data structure.
    mMap.swap(rhs.mMap);

    std::swap(mNextHandle, rhs.mNextHandle);
}

AttributeKey
AttributeKeyMap::getAttributeKey(const std::string &name, 
    AttributeKey::Type type, unsigned flags) const
{
    Map::const_iterator iterator = mMap.find(name);

    // If the AttributeKey already exists, return it.
    if (iterator != mMap.end()) {

        // If the AttributeKey already exists, its type must match
        // the specified type. If this assertion fails, code
        // external to the mesh library has probably called
        // getAttributeKey twice on a key with the same name,
        // but with differing types.
        assert((*iterator).second.type() == type);

        return (*iterator).second;
    }

    // Create the new AttributeKey.
    AttributeKey attributeKey(mNextHandle, type, flags);

    // Ensure that each created AttributeKey is unique.
    ++mNextHandle;
    
    // Insert a name-AttributeKey pair into the map.
    mMap.insert(std::make_pair(name, attributeKey));

    return attributeKey;
}

bool 
AttributeKeyMap::hasAttributeKey(const std::string &name) const
{
    return mMap.find(name) != mMap.end();
}

void 
AttributeKeyMap::eraseAttributeKey(const std::string &name)
{
    mMap.erase(name);
}

bool 
AttributeKeyMap::findAttributeNameAndKeyFromHandle(AttributeKey::Handle handle,
    const std::string **name, const AttributeKey **attributeKey) const
{
    // The current implementation is a O(n). It's probably fine if the number 
    // of attributes is small, but could be improved by maintaining a map
    // of handles to attribute names.

    for (Map::const_iterator iterator = mMap.begin();
         iterator != mMap.end(); ++iterator) {
        if ((*iterator).second.handle() == handle) {
            *name = &(*iterator).first;
            *attributeKey = &(*iterator).second;
            return true;
        }
    }

    // This should never happen if we're getting our handles from elements 
    // of the mesh that owns the AttributeKeyMap.
    return false;
}

} // namespace mesh
