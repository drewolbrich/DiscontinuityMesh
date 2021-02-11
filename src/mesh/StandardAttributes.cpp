// Copyright 2008 Drew Olbrich

#include "StandardAttributes.h"

#include <cassert>
#include <string>
#include <sstream>
#include <set>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "Mesh.h"

namespace mesh {

// These names are used both as a prefix of the Mesh's material and
// texture attribute tables (e.g. "material.5.diffuse4f") and as the
// name of the index attribute that is applied to faces (e.g. 
// "material") as a reference back to the table. Applications can rely
// on this consistency in the naming scheme when Mesh attributes of
// the form "<prefix>.<index>.<attribute>" are encountered.
// Changing these values will break the file format.
const std::string MATERIAL_ATTRIBUTE_BASE_NAME = "material";
const std::string TEXTURE_ATTRIBUTE_BASE_NAME = "texture";

// Return a standard mesh attribute name for an indexed
// material definition.
static std::string GetIndexedMaterialAttributeName(int materialIndex,
    const std::string &attributeName);
    
// Return a standard mesh attribute name for an indexed
// texture definition.
static std::string GetIndexedTextureAttributeName(int textureIndex,
    const std::string &attributeName);

// Return a vector of indices of all attributes that begin with the
// specified, followed by a period, a unique number, another period,
// and another string. (e.g. "material.3.diffuse4f")
static std::vector<int> GetAttributeTableIndexVector(const Mesh &mesh, const std::string &prefix);
    
AttributeKey 
GetTexCoord2fAttributeKey(const Mesh &mesh)
{
    return mesh.getAttributeKey("texCoord2f", AttributeKey::VECTOR2F,
        AttributeKey::STANDARD);
}

AttributeKey 
GetNormal3fAttributeKey(const Mesh &mesh)
{
    return mesh.getAttributeKey("normal3f", AttributeKey::UNIT_VECTOR3F,
        AttributeKey::STANDARD);
}

AttributeKey 
GetColor3fAttributeKey(const Mesh &mesh)
{
    return mesh.getAttributeKey("color3f", AttributeKey::VECTOR3F,
        AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialIndexAttributeKey(const Mesh &mesh)
{
    return mesh.getAttributeKey(MATERIAL_ATTRIBUTE_BASE_NAME, 
        AttributeKey::INT, AttributeKey::STANDARD);
}

AttributeKey 
GetTextureIndexAttributeKey(const Mesh &mesh)
{
    return mesh.getAttributeKey(TEXTURE_ATTRIBUTE_BASE_NAME, 
        AttributeKey::INT, AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialNameAttributeKey(const Mesh &mesh, int materialIndex)
{
    return mesh.getAttributeKey(
        GetIndexedMaterialAttributeName(materialIndex, "name"),
        AttributeKey::STRING, AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialAmbient4fAttributeKey(const Mesh &mesh, int materialIndex)
{
    return mesh.getAttributeKey(
        GetIndexedMaterialAttributeName(materialIndex, "ambient4f"),
        AttributeKey::VECTOR4F, AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialDiffuse4fAttributeKey(const Mesh &mesh, int materialIndex)
{
    return mesh.getAttributeKey(
        GetIndexedMaterialAttributeName(materialIndex, "diffuse4f"),
        AttributeKey::VECTOR4F, AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialSpecular4fAttributeKey(const Mesh &mesh, int materialIndex)
{
    return mesh.getAttributeKey(
        GetIndexedMaterialAttributeName(materialIndex, "specular4f"),
        AttributeKey::VECTOR4F, AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialEmission4fAttributeKey(const Mesh &mesh, int materialIndex)
{
    return mesh.getAttributeKey(
        GetIndexedMaterialAttributeName(materialIndex, "emission4f"),
        AttributeKey::VECTOR4F, AttributeKey::STANDARD);
}

AttributeKey 
GetMaterialShininessAttributeKey(const Mesh &mesh, int materialIndex)
{
    return mesh.getAttributeKey(
        GetIndexedMaterialAttributeName(materialIndex, "shininess"),
        AttributeKey::FLOAT, AttributeKey::STANDARD);
}

AttributeKey 
GetTextureFilenameAttributeKey(const Mesh &mesh, int textureIndex)
{
    return mesh.getAttributeKey(
        GetIndexedTextureAttributeName(textureIndex, "filename"),
        AttributeKey::STRING, AttributeKey::STANDARD);
}

bool 
HasMaterialDefinition(const Mesh &mesh, int materialIndex)
{
    return mesh.hasAttribute(GetMaterialNameAttributeKey(mesh, materialIndex)) 
        || mesh.hasAttribute(GetMaterialAmbient4fAttributeKey(mesh, materialIndex)) 
        || mesh.hasAttribute(GetMaterialDiffuse4fAttributeKey(mesh, materialIndex)) 
        || mesh.hasAttribute(GetMaterialSpecular4fAttributeKey(mesh, materialIndex)) 
        || mesh.hasAttribute(GetMaterialEmission4fAttributeKey(mesh, materialIndex)) 
        || mesh.hasAttribute(GetMaterialShininessAttributeKey(mesh, materialIndex));
}

bool 
HasTextureDefinition(const Mesh &mesh, int textureIndex)
{
    return mesh.hasAttribute(GetTextureFilenameAttributeKey(mesh, textureIndex));
}

MaterialIndexVector
GetMaterialIndexVector(const Mesh &mesh)
{
    return GetAttributeTableIndexVector(mesh, MATERIAL_ATTRIBUTE_BASE_NAME);
}

TextureIndexVector
GetTextureIndexVector(const Mesh &mesh)
{
    return GetAttributeTableIndexVector(mesh, TEXTURE_ATTRIBUTE_BASE_NAME);
}

static std::string 
GetIndexedMaterialAttributeName(int materialIndex,
    const std::string &attributeName)
{
    std::ostringstream ostr;
    ostr << MATERIAL_ATTRIBUTE_BASE_NAME 
        << "." << materialIndex << "." << attributeName;

    return ostr.str();
}

static std::string 
GetIndexedTextureAttributeName(int textureIndex,
    const std::string &attributeName)
{
    std::ostringstream ostr;
    ostr << TEXTURE_ATTRIBUTE_BASE_NAME
        << "." << textureIndex << "." << attributeName;

    return ostr.str();
}

static std::vector<int> 
GetAttributeTableIndexVector(const Mesh &mesh, const std::string &prefix)
{
    std::set<int> indexSet;
    for (AttributeKeyMap::const_iterator iterator = mesh.attributeKeyMapBegin();
         iterator != mesh.attributeKeyMapEnd(); ++iterator) {
        const std::string &attributeName = (*iterator).first;
        if (attributeName.find(prefix + ".") == 0) {
            std::string indexString = attributeName.substr(prefix.length() + 1);
            size_t pos = indexString.find('.');
            if (pos != std::string::npos) {
                indexString.resize(pos);
            }
            try {
                int materialIndex = boost::lexical_cast<int>(indexString);
                indexSet.insert(materialIndex);
            } catch (...) {
                // The lexical_cast above should never fail.
                assert(0);
            }
        }
    }

    std::vector<int> indexVector(indexSet.begin(), indexSet.end());

    return indexVector;
}

} // namespace mesh
