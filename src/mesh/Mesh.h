// Copyright 2008 Drew Olbrich

#ifndef MESH__MESH__INCLUDED
#define MESH__MESH__INCLUDED

#include <string>

#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "Types.h"
#include "AttributePossessor.h"
#include "AttributeKey.h"
#include "AttributeKeyMap.h"

namespace mesh {

// Mesh
//
// A fully connected mesh of polygons. The mesh is capable of
// representing a nonmanifold surface.
//
// Holes in polygons are not supported.

class Mesh : public AttributePossessor
{
public:
    Mesh();
    ~Mesh();

    // Copy constructor.
    Mesh(const Mesh &rhs);

    // Assignment operator.
    Mesh &operator=(const Mesh &rhs);

    // Efficiently swap the contents of the mesh with another mesh.
    void swap(Mesh &rhs);

    // Empty the mesh. The mesh is restored to the state of a newly created Mesh object.
    void clear();

    // The file that the mesh was loaded from.
    void setFilename(const std::string &filename);
    const std::string &filename() const;
    
    // Vertices.
    VertexPtr createVertex();
    void destroyVertex(VertexPtr vertexPtr);
    VertexPtr vertexBegin();
    VertexPtr vertexEnd();
    ConstVertexPtr vertexBegin() const;
    ConstVertexPtr vertexEnd() const;
    VertexList::size_type vertexCount() const;

    // Edges.
    EdgePtr createEdge();
    void destroyEdge(EdgePtr edgePtr);
    EdgePtr edgeBegin();
    EdgePtr edgeEnd();
    ConstEdgePtr edgeBegin() const;
    ConstEdgePtr edgeEnd() const;
    EdgeList::size_type edgeCount() const;

    // Faces.
    FacePtr createFace();
    void destroyFace(FacePtr facePtr);
    FacePtr faceBegin();
    FacePtr faceEnd();
    ConstFacePtr faceBegin() const;
    ConstFacePtr faceEnd() const;
    FaceList::size_type faceCount() const;

    // Return an AttributeKey for the Mesh or any of its components.
    // Pass AttributeKey::TEMPORARY for the optional flags argument
    // to ensure that the attribute is never written to disk.
    AttributeKey getAttributeKey(const std::string &name, 
        AttributeKey::Type type, unsigned flags = 0) const;

    // Returns true if the named attribute key is already defined
    // for the mesh.
    bool hasAttributeKey(const std::string &name) const; 
    
    // Erase a named AttributeKey.
    void eraseAttributeKey(const std::string &name);

    // Returns an attribute name and AttributeKey from a handle.
    bool findAttributeNameAndKeyFromHandle(AttributeKey::Handle handle,
        const std::string **name, const AttributeKey **attributeKey) const;

    // Read access to the Mesh's AttributeKeyMap.
    AttributeKeyMap::size_type attributeKeyMapSize() const;
    AttributeKeyMap::const_iterator attributeKeyMapBegin() const;
    AttributeKeyMap::const_iterator attributeKeyMapEnd() const;

private:
    std::string mFilename;
    VertexList mVertexList;
    EdgeList mEdgeList;
    FaceList mFaceList;

    AttributeKeyMap mAttributeKeyMap;

    // NOTE: When new member variables are added, the function
    // Mesh::swap must be updated accordingly.
};

} // namespace mesh

#endif // MESH__MESH__INCLUDED
