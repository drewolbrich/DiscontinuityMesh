// Copyright 2008 Drew Olbrich

#ifndef MESH__ATTRIBUTE_KEY_MAP__INCLUDED
#define MESH__ATTRIBUTE_KEY_MAP__INCLUDED

#include <string>
#include <map>

#include "AttributeKey.h"

namespace mesh {

// AttributeKeyMap
//
// This class defines an std::map that maps key string names
// into AttributeKeys.

class AttributeKeyMap 
{
public:
    AttributeKeyMap();
    ~AttributeKeyMap();

    // Efficiently swap the AttributeKeyMap with another.
    void swap(AttributeKeyMap &rhs);
    
    // Returns an AttributeKey by name and type, or creates
    // a new one if it does not exist.
    AttributeKey getAttributeKey(const std::string &name, 
        AttributeKey::Type type, unsigned flags = 0) const;

    // Returns true if the map contains the specified AttributeKey.
    bool hasAttributeKey(const std::string &name) const;

    // Erase a named AttributeKey from the map.
    void eraseAttributeKey(const std::string &name);

    // Returns an attribute name and AttributeKey from a handle.
    // The name is returned a pointer to a string.
    bool findAttributeNameAndKeyFromHandle(AttributeKey::Handle handle,
        const std::string **name, const AttributeKey **attributeKey) const;

    // Exposed std::map member functions for iterating over
    // the map elements. Given an iterator, (*iterator).first
    // is the const std::string name of the AttributeKey, 
    // and (*iterator).second is the AttributeKey.
    typedef std::map<const std::string, AttributeKey> Map;
    typedef Map::size_type size_type;
    typedef Map::iterator iterator;
    typedef Map::const_iterator const_iterator;
    size_type size() const { return mMap.size(); }
    iterator begin() { return mMap.begin(); } 
    iterator end() { return mMap.end(); } 
    const_iterator begin() const { return mMap.begin(); } 
    const_iterator end() const { return mMap.end(); } 

private:
    // These member variables are mutable, because the intention is
    // that a class that contains an AttributeKeyMap, like Mesh, can
    // use getAttributeKey to query the AttributeKeyMap even when the
    // Mesh is const. It's possible that the call to getAttributeKey
    // may wind up creating a new key if it doesn't already exist, but
    // this isn't considered a modification to the AttributeKeyMap,
    // per se, since getAttributeKey must always return a valid key.
    mutable Map mMap;
    mutable AttributeKey::Handle mNextHandle;
};

} // namespace mesh

#endif // MESH__ATTRIBUTE_KEY_MAP__INCLUDED
