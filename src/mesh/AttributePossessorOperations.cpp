// Copyright 2009 Retroactive Fiasco.

#include "AttributePossessorOperations.h"

#include <cassert>

#include "AttributePossessor.h"
#include "Mesh.h"

namespace mesh {

void 
MergeAttributePossessorAttributes(const Mesh &mesh, 
    const AttributePossessor &sourceAttributePossessor,
    AttributePossessor *targetAttributePossessor)
{
    for (AttributePossessor::const_iterator iterator 
             = sourceAttributePossessor.attributeDataBegin();
         iterator != sourceAttributePossessor.attributeDataEnd(); ++iterator) {
        const AttributeData &sourceAttributeData = *iterator;

        const std::string *name = NULL;
        const AttributeKey *attributeKey = NULL;
#ifdef DEBUG
        bool result = 
#endif
            mesh.findAttributeNameAndKeyFromHandle(sourceAttributeData.handle(),
                &name, &attributeKey);
        // The attribute must exist in the mesh's AttributeKeyMap.
        assert(result);

        if (targetAttributePossessor->hasAttribute(*attributeKey)) {
            // The target attribute already possesses the attribute,
            // so don't overwrite it.
            continue;
        }

        // Copy the attribute from the source attribute possessor 
        // to the target attribute possessor.
        AttributeData *targetAttributeData = targetAttributePossessor->findOrCreateAttributeData(
            *attributeKey);
        targetAttributeData->copyData(sourceAttributeData);
    }
}

} // namespace mesh
