// Copyright 2008 Drew Olbrich

#ifndef MESHOBJ__OBJ_MATERIAL_MAP__INCLUDED
#define MESHOBJ__OBJ_MATERIAL_MAP__INCLUDED

#include <string>

#include "ObjMaterial.h"

namespace meshobj {

// ObjMaterialMap
//
// A map of material names to ObjMaterial objects.

typedef std::map<std::string, ObjMaterial> ObjMaterialMap;

} // namespace meshobj

#endif // MESHOBJ__OBJ_MATERIAL_MAP__INCLUDED
