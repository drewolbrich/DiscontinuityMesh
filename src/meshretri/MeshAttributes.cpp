// Copyright 2009 Drew Olbrich

#include "MeshAttributes.h"

namespace meshretri {

mesh::AttributeKey
GetIsDegreeZeroDiscontinuityAttributeKey(const mesh::Mesh &mesh)
{
    return mesh.getAttributeKey("__meshretri_DegreeZeroDiscontinuityAttributeKey",
        mesh::AttributeKey::BOOL, mesh::AttributeKey::TEMPORARY);
}

} // namespace meshretri
