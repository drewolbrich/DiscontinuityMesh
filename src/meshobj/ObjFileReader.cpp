// Copyright 2008 Drew Olbrich

#include "ObjFileReader.h"

#include <cassert>

#include <except/OpenFileException.h>
#include <except/FailedOperationException.h>
#include <mesh/StandardAttributes.h>
#include <mesh/IsConsistent.h>
#include <str/FilenameOperations.h>

#include "CreateEdgesFromFace.h"

using cgmath::Vector3f;
using cgmath::Vector4f;
using except::OpenFileException;
using except::FailedOperationException;
using os::Error;
using mesh::AttributeKey;

namespace meshobj {

ObjFileReader::ObjFileReader()
    : mFile(NULL),
      mMesh(),
      mObjFilename(),
      mErrorLineNumber(0),
      mObjVertexTable(),
      mObjNormalVertexTable(),
      mObjTextureVertexTable(),
      mObjMaterialMap(),
      mMeshMaterialIndexMap(),
      mMeshTextureIndexMap(),
      mMeshMaterialIndex(0),
      mMeshTextureIndex(0),
      mMaterialCount(0),
      mTextureCount(0)
{
}

ObjFileReader::~ObjFileReader()
{
}

void 
ObjFileReader::readObjFile(mesh::Mesh *mesh, const std::string &objFilename)
{
    reset();

    mMesh.setFilename(objFilename);

    mObjFilename = objFilename;
    
    // Try to open the OBJ file. By declaring the std::ifstream in
    // local storage, and then referencing it throughout the class via
    // a pointer, we ensure that the file will be closed if an
    // exception is thrown.
    std::ifstream file(objFilename.c_str());
    mFile = &file;

    if (!*mFile) {
        throw OpenFileException(SOURCE_LINE, Error::fromSystemError()) 
            << "Could not open file \"" << objFilename << "\".";
    }

    // The current line number.
    unsigned int currentLineNumber = 1;

    // The material index to apply to each face.
    mMeshMaterialIndex = mesh::UNDEFINED_MATERIAL_INDEX;

    // The texture index to apply to each face.
    mMeshTextureIndex = mesh::UNDEFINED_TEXTURE_INDEX;

    // The number of unique materials encountered so far.
    // This variable is incremented when a new material index
    // is required.
    mMaterialCount = 0;

    // The number of unique textures encountered so far.
    mTextureCount = 0;

    // There are two ways that textures can be defined,
    // either with the "usemap" token in a .obj file (the old way),
    // or with the "map_Kd" token in a material definition
    // in the .mtl file. We must keep track of which style
    // we encounter first, in order to treat them properly.
    typedef enum {
        UNDEFINED,
        OLD,
        NEW
    } MapStyle;
    MapStyle mapStyle = UNDEFINED;
    
    while (!mFile->eof()) {

        // Store the line number used for syntax error reporting.
        mErrorLineNumber = currentLineNumber;
        
        // Read the next line from the file.
        std::string line = readLine(*mFile, currentLineNumber);

        // Create a string stream for parsing the line.
        std::istringstream istr(line.c_str());
        
        // Parse the first token on the line.
        std::string firstToken;
        istr >> firstToken;

        if (firstToken == "v") {

            // Vertex.
            readVertex(istr);

        } else if (firstToken == "vt") {

            // Texture vertex.
            readTextureVertex(istr);

        } else if (firstToken == "vn") {

            // Vertex normal.
            readNormalVertex(istr);

        } else if (firstToken == "f") {

            // Face.
            readFace(istr);

        } else if (firstToken == "mtllib") {

            // Material library file.

            // Multiple material library files may be specified
            // as arguments to the "mtllib" command.
            while (!istr.eof()) {
                std::string mtlFilename;
                istr >> mtlFilename;
                readMaterialLibrary(mtlFilename);
            }

        } else if (firstToken == "usemtl") {

            // Material name.
            std::string objMaterialName;
            readMaterial(istr, objMaterialName);
            mMeshMaterialIndex = findOrCreateMaterialIndex(objMaterialName);

            ObjMaterialMap::iterator objMaterialMapIterator
                = mObjMaterialMap.find(objMaterialName);

            // The ObjMaterial must be in the ObjMaterialMap.
            assert(objMaterialMapIterator != mObjMaterialMap.end());

            // Check for a new style texture map definition.
            const ObjMaterial &objMaterial = (*objMaterialMapIterator).second;
            if (objMaterial.diffuseMapIsSet()) {

                if (mapStyle == OLD) {
                    throw FailedOperationException(SOURCE_LINE)
                        << "New style texture definition encountered "
                        << "after old style texture definition "
                        << "on line " << mErrorLineNumber
                        << " of file \"" << mObjFilename << "\".";
                }

                mapStyle = NEW;

                const std::string &objTextureName = objMaterial.diffuseMap();
                mMeshTextureIndex = findOrCreateTextureIndex(objTextureName);
                
            } else {

                // Only reset the texture index if we know that
                // we've encountered new style texture definitions
                // already. Otherwise, this assignment would
                // disrupt old style texture definitions, which
                // require an explicit "usemap off" line to 
                // clear the definition.
                if (mapStyle == NEW) {
                    mMeshTextureIndex = mesh::UNDEFINED_TEXTURE_INDEX;
                }
            }
            
        } else if (firstToken == "usemap") {

            // Old-style map name.

            if (mapStyle == NEW) {
                throw FailedOperationException(SOURCE_LINE)
                    << "Old style texture definition encountered "
                    << "after new style texture definition "
                    << "on line " << mErrorLineNumber
                    << " of file \"" << mObjFilename << "\".";
            }
            
            mapStyle = OLD;

            std::string objTextureName;
            readString(istr, mObjFilename, mErrorLineNumber, objTextureName);

            // The Alias documentation says that if the map name
            // is "off", this means that no map should be assigned
            // to the faces. However, Blender's OBJ file exporter
            // writes out the string "(null)" in that case.
            if (objTextureName == "off"
                || objTextureName == "(null)") {
                mMeshTextureIndex = mesh::UNDEFINED_TEXTURE_INDEX;
            } else {
                mMeshTextureIndex = findOrCreateTextureIndex(objTextureName);
            }
        }
    }

    file.close();

    // Create all the edges, and wire them up to the
    // faces and vertices.
    for (mesh::FacePtr facePtr = mMesh.faceBegin(); 
         facePtr != mMesh.faceEnd(); ++facePtr) {
        CreateEdgesFromFace(mMesh, facePtr);
    }

    // TODO: Check for vertices that never had any faces
    // reference them, and delete them.

    // Make sure the references between the mesh components
    // are consistent. (Only has an effect in debug builds.)
    assert(mesh::IsConsistent(mMesh));

    // Efficiently swap the mesh specified as an argument with the
    // mesh we read off disk. This helps make this function
    // exception safe.
    mesh->swap(mMesh);

    reset();
}

void 
ObjFileReader::reset()
{
    mFile = NULL;

    mMesh.clear();

    mObjFilename = "";
    mErrorLineNumber = 0;
    mObjVertexTable.clear();
    mObjNormalVertexTable.clear();
    mObjTextureVertexTable.clear();
    mObjMaterialMap.clear();
    mMeshMaterialIndexMap.clear();
    mMeshTextureIndexMap.clear();
    mMeshMaterialIndex = 0;
    mMeshTextureIndex = 0;
    mMaterialCount = 0;
    mTextureCount = 0;
}

void 
ObjFileReader::readFace(std::istringstream &istr)
{
    // Create a new face in the mesh.
    mesh::FacePtr facePtr = mMesh.createFace();

    // If a material was previously assigned with 
    // the "usemtl" token, assign it to the face.
    if (mMeshMaterialIndex != mesh::UNDEFINED_MATERIAL_INDEX) {
        facePtr->setInt(mesh::GetMaterialIndexAttributeKey(mMesh), mMeshMaterialIndex);
    }
    
    // If a texture was previously assigned with 
    // the "usemap" token, assign it to the face.
    if (mMeshTextureIndex != mesh::UNDEFINED_TEXTURE_INDEX) {
        facePtr->setInt(mesh::GetTextureIndexAttributeKey(mMesh), mMeshTextureIndex);
    }
    
    // Count the vertices as we encounter them so if there
    // aren't any vertices at all, we can report an error.
    int vertexCount = 0;

    while (true) {

        int geometricVertexIndex = 0;
        int textureVertexIndex = 0;
        int normalVertexIndex = 0;
        if (!readFaceVertex(istr, geometricVertexIndex, textureVertexIndex, 
                normalVertexIndex)) {
            // If there are no more tokens, stop reading vertices.
            break;
        }

        // Get the VertexPtr corresponding to the geometric
        // vertex index.
        mesh::VertexPtr vertexPtr = getVertexPtrFromGeometricVertexIndex(geometricVertexIndex);
        
        // Add the vertex to the face.
        facePtr->addAdjacentVertex(vertexPtr);

        // Add the face to the vertex;
        vertexPtr->addAdjacentFace(facePtr);

        // Assign the texture vertex to the face vertex
        // if it's defined.
        if (textureVertexIndex != 0) {

            // The resulting texture coordinate for this vertex.
            cgmath::Vector2f texture = getVector2fFromTextureVertexIndex(textureVertexIndex);

            // Assign the texture vertex to the face vertex.
            facePtr->setVertexVector2f(vertexPtr, 
                mesh::GetTexCoord2fAttributeKey(mMesh), texture);
        }
        
        // Assign the normal vertex to the face vertex
        // if it's defined.
        if (normalVertexIndex != 0) {

            // The resulting normal for this vertex.
            Vector3f normal = getVector3fFromNormalVertexIndex(normalVertexIndex);

            // Assign the vertex normal to the face vertex.
            facePtr->setVertexUnitVector3f(vertexPtr, 
                mesh::GetNormal3fAttributeKey(mMesh), normal);
        }
        
        // Count the number of vertices we've read so later we
        // can check if there were no vertices at all.
        vertexCount++;
    }

    if (vertexCount == 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Missing vertex indices on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }
}

bool 
ObjFileReader::readFaceVertex(std::istringstream &istr,
    int &geometricVertexIndex, int &textureVertexIndex, int &normalVertexIndex)
{
    std::string token;
    istr >> token;
    
    // If there are no more tokens, stop reading vertices.
    if (istr.fail()) {
        return false;
    }
    
    geometricVertexIndex = 0;
    textureVertexIndex = 0;
    normalVertexIndex = 0;
    
    // Read the geometric vertex index.
    std::string::size_type pos = token.find('/');
    if (pos == std::string::npos) {
        geometricVertexIndex = atoi(token.c_str());
    } else {
        geometricVertexIndex = atoi(token.substr(0, pos).c_str());
    }
    
    // Read the texture vertex index, if present.
    if (pos != std::string::npos) {
        
        std::string::size_type startPos = pos + 1;
        pos = token.find('/', startPos);
        if (pos == std::string::npos) {
            textureVertexIndex = atoi(token.substr(startPos).c_str());
        } else {
            textureVertexIndex = atoi(token.substr(startPos, pos).c_str());
        }
        
        // Read the normal vertex index, if present.
        if (pos != std::string::npos) {
            
            std::string::size_type startPos = pos + 1;
            pos = token.find('/', startPos);
            if (pos == std::string::npos) {
                normalVertexIndex = atoi(token.substr(startPos).c_str());
            } else {
                normalVertexIndex = atoi(token.substr(startPos, pos).c_str());
            }
        }
    }
    
    // If the geometric vertex index is zero, or was not
    // specified, this is an error.
    if (geometricVertexIndex == 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Missing or 0 geometric vertex index on line " 
                << mErrorLineNumber << " of file \"" << mObjFilename << "\".";
    }

    return true;
}

int 
ObjFileReader::findOrCreateMaterialIndex(const std::string &objMaterialName)
{
    // If the material already exists in the map, return its index.
    MeshMaterialIndexMap::iterator indexMapIterator
        = mMeshMaterialIndexMap.find(objMaterialName);
    if (indexMapIterator != mMeshMaterialIndexMap.end()) {
        return (*indexMapIterator).second;
    }

    // Otherwise, create a new index and add it to the map.
    // The first material index is 0.
    int meshMaterialIndex = mMaterialCount;
    ++mMaterialCount;
    mMeshMaterialIndexMap.insert(std::make_pair(objMaterialName, meshMaterialIndex));
    
    // Get the OBJ material definition.

    ObjMaterial objMaterial;

    // If a material name was specified, look it up
    // in the OBJ material map.
    if (objMaterialName != "") {
        // Retrieve the OBJ material definition corresponding to the
        // OBJ material name.
        ObjMaterialMap::const_iterator materialMapIterator 
            = mObjMaterialMap.find(objMaterialName);
        
        // There must be a matching material.
        assert(materialMapIterator != mObjMaterialMap.end());

        objMaterial = (*materialMapIterator).second;
    }

    // Assign the OBJ material definition to the mesh's attributes.

    // Material name.
    AttributeKey materialNameKey = mesh::GetMaterialNameAttributeKey(mMesh, 
        meshMaterialIndex);
    mMesh.setString(materialNameKey, objMaterialName);

    // Opacity. This value gets folded into the Mesh material's
    // ambient and diffuse components, because that's how OpenGL
    // handles opacity.
    float opacity = 1.0;
    if (objMaterial.opacityIsSet()) {
        opacity = objMaterial.opacity();
    }
    
    // Ambient.
    if (objMaterial.ambientIsSet()
        || objMaterial.opacityIsSet()) {
        AttributeKey ambientKey = mesh::GetMaterialAmbient4fAttributeKey(mMesh,
            meshMaterialIndex);
        Vector3f ambient = objMaterial.ambient();
        mMesh.setVector4f(ambientKey, 
            Vector4f(ambient[0], ambient[1], ambient[2], opacity));
    }
    
    // Diffuse.
    if (objMaterial.diffuseIsSet()
        || objMaterial.opacityIsSet()) {
        AttributeKey diffuseKey = mesh::GetMaterialDiffuse4fAttributeKey(mMesh,
            meshMaterialIndex);
        Vector3f diffuse = objMaterial.diffuse();
        mMesh.setVector4f(diffuseKey, Vector4f(diffuse[0], diffuse[1], diffuse[2], opacity));
    }
    
    // Specular.
    if (objMaterial.specularIsSet()) {
        AttributeKey specularKey = mesh::GetMaterialSpecular4fAttributeKey(mMesh,
            meshMaterialIndex);
        Vector3f specular = objMaterial.specular();
        mMesh.setVector4f(specularKey, 
            Vector4f(specular[0], specular[1], specular[2], 1.0));
    }

    // Emission.
    if (objMaterial.emissionIsSet()) {
        AttributeKey emissionKey = mesh::GetMaterialEmission4fAttributeKey(mMesh,
            meshMaterialIndex);
        Vector3f emission = objMaterial.emission();
        mMesh.setVector4f(emissionKey, Vector4f(emission[0], emission[1], emission[2], 1.0));
    }

    // Shininess.
    if (objMaterial.shininessIsSet()) {
        AttributeKey shininessKey = mesh::GetMaterialShininessAttributeKey(mMesh,
            meshMaterialIndex);
        float shininess = objMaterial.shininess();
        mMesh.setFloat(shininessKey, shininess);
    }
    
    return meshMaterialIndex;
}

int 
ObjFileReader::findOrCreateTextureIndex(const std::string &objTextureName)
{
    // If the texture already exists in the map, return its index.
    MeshTextureIndexMap::iterator indexMapIterator = mMeshTextureIndexMap.find(objTextureName);
    if (indexMapIterator != mMeshTextureIndexMap.end()) {
        return (*indexMapIterator).second;
    }

    // Otherwise, create a new index and add it to the map.
    // The first texture index is 0.
    int meshTextureIndex = mTextureCount;
    ++mTextureCount;
    mMeshTextureIndexMap.insert(std::make_pair(objTextureName, meshTextureIndex));

    // Assign the texture filename to the mesh's attributes.
   AttributeKey textureFilenameKey = mesh::GetTextureFilenameAttributeKey(mMesh,
        meshTextureIndex);
    mMesh.setString(textureFilenameKey, objTextureName);
    
    return meshTextureIndex;
}

void 
ObjFileReader::readMaterialLibrary(std::string mtlFilename)
{
    // Modify the included filename so that it can be reached
    // from the current directory, even if mObjFilename
    // references another directory. So, if mObjFilename
    // is "foo/bar/baz.obj", and the included filename is "xxx.mtl",
    // the returned filename will be "foo/baz/xxx.mtl".
    mtlFilename = str::GetIncludedFilename(mObjFilename, mtlFilename);

    // Try to open the OBJ file.
    std::ifstream file(mtlFilename.c_str());

    if (!file) {
        throw FailedOperationException(SOURCE_LINE, Error::fromSystemError())
            << "Could not open file \"" << mtlFilename << "\" "
                << "referenced on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }

    // The current line number.
    unsigned int currentLineNumber = 1;

    // The line number that is used to report syntax errors. This may
    // be different from the current line number, if a line is
    // encountered that ends in a backslash.
    unsigned int errorLineNumber = 1;

    // The current Wavefront material definition being read in.
    ObjMaterial objMaterial;

    // The name of the material being read.
    std::string materialName("");

    // If false, a "newmtl" token has not yet been encountered,
    // and it is illegal to define material attributes.
    bool materialFound = false;

    while (!file.eof()) {

        // Store the line number used for syntax error reporting.
        errorLineNumber = currentLineNumber;
        
        // Read the next line from the file.
        std::string line(readLine(file, currentLineNumber));

        // Create a string stream for parsing the line.
        std::istringstream iStr(line.c_str());

        // Parse the first token on the line.
        std::string firstToken;
        iStr >> firstToken;

        if (firstToken == "newmtl") {

            // If a previous material had been read,
            // insert it into the map before reading the 
            // new material.
            if (materialFound) {
                // A material name must have already been assigned.
                assert(materialName != "");
                mObjMaterialMap.insert(std::make_pair(materialName, objMaterial));
            }

            materialFound = true;
            readString(iStr, mtlFilename, errorLineNumber, materialName);

            // Start with the default material definition before
            // reading any attributes.
            objMaterial = ObjMaterial();

        } else if (firstToken == "Ka") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            Vector3f ambient;
            readVector3f(iStr, mtlFilename, errorLineNumber, ambient);
            objMaterial.setAmbient(ambient);
                
        } else if (firstToken == "Kd") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            Vector3f diffuse;
            readVector3f(iStr, mtlFilename, errorLineNumber, diffuse);
            objMaterial.setDiffuse(diffuse);

        } else if (firstToken == "Ks") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            Vector3f specular;
            readVector3f(iStr, mtlFilename, errorLineNumber, specular);
            objMaterial.setSpecular(specular);

        } else if (firstToken == "Ke") {

            // "Ke" (emission) is not a standard attribute of
            // Wavefront MTL files. We've defined it here as a way of
            // specifying the emissive color for OpenGL materials, and
            // an initial emission value for radiosity computation.
            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            Vector3f emission;
            readVector3f(iStr, mtlFilename, errorLineNumber, emission);
            objMaterial.setEmission(emission);

        } else if (firstToken == "d" || firstToken == "Tr") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            float opacity;
            readFloat(iStr, mtlFilename, errorLineNumber, opacity);
            objMaterial.setOpacity(opacity);

        } else if (firstToken == "Ns") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            float shininess;
            readFloat(iStr, mtlFilename, errorLineNumber, shininess);
            objMaterial.setShininess(shininess);

        } else if (firstToken == "illum") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            float illuminationModel;
            readFloat(iStr, mtlFilename, errorLineNumber, illuminationModel);

            // The illumination model number is currently ignored. If
            // the material is completely diffuse, it is assumed that
            // no specular value will be defined.

        } else if (firstToken == "map_Kd") {

            verifyMaterialFound(materialFound, mtlFilename, errorLineNumber);
            std::string diffuseMap;
            readString(iStr, mtlFilename, errorLineNumber, diffuseMap);
            objMaterial.setDiffuseMap(diffuseMap);
        }
    }

    if (materialFound) {
        // A material name must have already been assigned.
        assert(materialName != "");
        mObjMaterialMap.insert(std::make_pair(materialName, objMaterial));
    }
    
    file.close();
}

void 
ObjFileReader::readMaterial(std::istringstream &istr, std::string &objMaterialName)
{
    readString(istr, mObjFilename, mErrorLineNumber, objMaterialName);

    // If no material is specified, do nothing.
    if (objMaterialName == "") {
        return;
    }
    
    // Look up the material name in the map;
    ObjMaterialMap::const_iterator iterator = mObjMaterialMap.find(objMaterialName);
    if (iterator == mObjMaterialMap.end()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Undefined material name \"" << objMaterialName
                << "\" on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }
}

void 
ObjFileReader::verifyMaterialFound(bool materialFound, 
    const std::string &mtlFilename, unsigned int errorLineNumber)
{
    if (!materialFound) {
        throw FailedOperationException(SOURCE_LINE)
            << "Material attribute encountered before \"newmtl\" token "
                << "on line " << errorLineNumber
                << " of file \"" << mtlFilename << "\".";
    }
}

std::string 
ObjFileReader::readLine(std::ifstream &file, unsigned int &currentLineNumber)
{
    std::string logicalLine;
    std::string physicalLine;
    std::size_t index = 0;

    while (true) {

        // Read the next physical line from the file.
        std::getline(file, physicalLine);
        ++currentLineNumber;

        // Concatenate the line read onto the logical line read so
        // far, which can consist of multiple physical lines if lines
        // ending in backslashes are encountered.
        logicalLine += physicalLine;

        // Set the index to the last character in the line that
        // was just read in.
        if (physicalLine.length() > 0) {
            index += physicalLine.length() - 1;
        }

        // Files that originated on Windows machines end in
        // carriage return line feed pairs. So, if the last character
        // in the file is a carriage return, chop it off.
        if (logicalLine[index] == '\r') {
            logicalLine.resize(index);
            index--;
        }
        
        if (logicalLine[index] == '\\') {
            // Replace the backslash with a space, because some
            // applications that parse OBJ files supposedly treat
            // it as whitespace.
            logicalLine[index] = ' ';
            index++;
        } else {
            // The line does not end with a backslash, so stop
            // reading from the file.
            break;
        }
    }

    return logicalLine;
}

void 
ObjFileReader::readString(std::istringstream &istr, 
    const std::string &filename, unsigned int errorLineNumber, std::string &string)
{
    istr >> string;
    if (istr.fail()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Syntax error on line " << errorLineNumber
                << " of file \"" << filename << "\".";
    }
}

void 
ObjFileReader::readFloat(std::istringstream &istr, 
    const std::string &filename, unsigned int errorLineNumber, float &value)
{
    istr >> value;
    if (istr.fail()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Syntax error on line " << errorLineNumber
                << " of file \"" << filename << "\".";
    }
}

void 
ObjFileReader::readVector3f(std::istringstream &istr, 
    const std::string &filename, unsigned int errorLineNumber,
    cgmath::Vector3f &vector3f)
{
    istr >> vector3f[0] >> vector3f[1] >> vector3f[2];
    if (istr.fail()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Syntax error on line " << errorLineNumber
                << " of file \"" << filename << "\".";
    }
}

void 
ObjFileReader::readVertex(std::istringstream &istr)
{
    // Read the OBJ vertex from the current line.
    float x, y, z;
    istr >> x >> y >> z;
    if (istr.fail()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Syntax error on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }

    // Create a mesh vertex from these values.
    mesh::VertexPtr vertexPtr(mMesh.createVertex());
    vertexPtr->setPosition(cgmath::Vector3f(x, y, z));
    
    // Add the vertex pointer to the table so we can reference it
    // later by index when we want to create faces.
    mObjVertexTable.push_back(vertexPtr);
}

void 
ObjFileReader::readTextureVertex(std::istringstream &istr)
{
    // Read the OBJ texture vertex from the current line.
    float u, v;
    istr >> u >> v;
    if (istr.fail()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Syntax error on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }

    // Add the vertex texture to the table so we can reference it
    // later by index when we want to create faces.
    mObjTextureVertexTable.push_back(cgmath::Vector2f(u, v));
}

void 
ObjFileReader::readNormalVertex(std::istringstream &istr)
{
    // Read the OBJ normal vertex from the current line.
    float x, y, z;
    istr >> x >> y >> z;
    if (istr.fail()) {
        throw FailedOperationException(SOURCE_LINE)
            << "Syntax error on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }

    // Add the vertex normal to the table so we can reference it
    // later by index when we want to create faces.
    mObjNormalVertexTable.push_back(Vector3f(x, y, z));
}

mesh::VertexPtr 
ObjFileReader::getVertexPtrFromGeometricVertexIndex(
    int geometricVertexIndex)
{
    // The geometric vertex index should never be zero.
    if (geometricVertexIndex == 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Geometric vertex index "
                << geometricVertexIndex << " is zero "
                << "on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }
    
    if (geometricVertexIndex > 0) {
        // If the vertex index is positive, it's a direct index into
        // the table.
        if (geometricVertexIndex - 1
            >= static_cast<int>(mObjVertexTable.size())) {
            throw FailedOperationException(SOURCE_LINE)
                << "Geometric vertex index "
                    << geometricVertexIndex << " out of range "
                    << "on line " << mErrorLineNumber
                    << " of file \"" << mObjFilename << "\".";
        }
        
        // The value 1 references the first vertex in the table, so we
        // have to subract 1, because STL vectors are zero-indexed.
        return mObjVertexTable[geometricVertexIndex - 1];
    }

    // If the vertex index is negative, it's an index relative to
    // the end of the table.
    if (static_cast<int>(mObjVertexTable.size()) 
        + geometricVertexIndex < 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Geometric vertex index "
                << geometricVertexIndex << " out of range "
                << "on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }
    
    return mObjVertexTable[mObjVertexTable.size() 
        + geometricVertexIndex];
}

cgmath::Vector2f 
ObjFileReader::getVector2fFromTextureVertexIndex(
    int textureVertexIndex)
{
    // The texture vertex index should never be zero.
    assert(textureVertexIndex != 0);
    
    if (textureVertexIndex > 0) {
        
        // If the texture vertex index is positive, it's a
        // direct index into the table.
        if (textureVertexIndex - 1 >= static_cast<int>(mObjTextureVertexTable.size())) {
            throw FailedOperationException(SOURCE_LINE)
                << "Texture vertex index "
                    << textureVertexIndex << " out of range "
                    << "on line " << mErrorLineNumber
                    << " of file \"" << mObjFilename << "\".";
        }
        
        // The value 1 references the first vertex in the
        // table, so we have to subract 1, because STL vectors
        // are zero-indexed.
        return mObjTextureVertexTable[textureVertexIndex - 1];
    }
    
    // If the texture vertex index is negative, it's an
    // index relative to the end of the table.
    if (static_cast<int>(mObjTextureVertexTable.size()) + textureVertexIndex < 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Texture vertex index "
                << textureVertexIndex << " out of range "
                << "on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }
    
    return mObjTextureVertexTable[mObjTextureVertexTable.size() 
        + textureVertexIndex];
}

cgmath::Vector3f 
ObjFileReader::getVector3fFromNormalVertexIndex(
    int normalVertexIndex)
{
    // The normal vertex index should never be zero.
    assert(normalVertexIndex != 0);
    
    if (normalVertexIndex > 0) {
        
        // If the normal vertex index is positive, it's a
        // direct index into the table.
        if (normalVertexIndex - 1 >= static_cast<int>(mObjNormalVertexTable.size())) {
            throw FailedOperationException(SOURCE_LINE)
                << "Normal vertex index "
                    << normalVertexIndex << " out of range "
                    << "on line " << mErrorLineNumber
                    << " of file \"" << mObjFilename << "\".";
        }
        
        // The value 1 references the first vertex in the
        // table, so we have to subract 1, because STL vectors
        // are zero-indexed.
        return mObjNormalVertexTable[normalVertexIndex - 1];
    }
    
    // If the normal vertex index is negative, it's an
    // index relative to the end of the table.
    if (static_cast<int>(mObjNormalVertexTable.size()) 
        + normalVertexIndex < 0) {
        throw FailedOperationException(SOURCE_LINE)
            << "Normal vertex index "
                << normalVertexIndex << " out of range "
                << "on line " << mErrorLineNumber
                << " of file \"" << mObjFilename << "\".";
    }
    
    return mObjNormalVertexTable[mObjNormalVertexTable.size() + normalVertexIndex];
}

} // namespace meshobj
