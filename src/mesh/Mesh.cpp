// Copyright 2008 Retroactive Fiasco.

#include "Mesh.h"
#include "Concatenate.h"

namespace mesh {

Mesh::Mesh()
    : AttributePossessor(),
      mFilename(),
      mVertexList(),
      mEdgeList(),
      mFaceList(),
      mAttributeKeyMap()
{
}

Mesh::~Mesh()
{
}

Mesh::Mesh(const Mesh &rhs)
    : AttributePossessor()
{
    *this = rhs;
}

Mesh &
Mesh::operator=(const Mesh &rhs)
{
    if (this == &rhs) {
        return *this;
    }

    // Empty the mesh.
    clear();

    // Copy the mesh elements via concatenation with the now empty mesh.
    Concatenate(this, rhs);

    // The mesh's filename is not affected by the concatenation operation,
    // so we must copy it explicitly.
    this->mFilename = rhs.mFilename;

    return *this;
}

void 
Mesh::swap(Mesh &rhs)
{
    mFilename.swap(rhs.mFilename);

    // This should be fast because a constant number of pointers
    // are swapped, rather than the complete data structures.
    mVertexList.swap(rhs.mVertexList);
    mEdgeList.swap(rhs.mEdgeList);
    mFaceList.swap(rhs.mFaceList);
    mAttributeKeyMap.swap(rhs.mAttributeKeyMap);

    // Swap the base class's member variables.
    this->AttributePossessor::swap(rhs);
}

void
Mesh::clear()
{
    // Clear out the mesh by swapping it with an empty one.
    Mesh emptyMesh;
    this->swap(emptyMesh);
}

void 
Mesh::setFilename(const std::string &filename)
{
    mFilename = filename;
}

const std::string &
Mesh::filename() const
{
    return mFilename;
}

VertexPtr 
Mesh::createVertex()
{
    mVertexList.push_back(Vertex());

    // Return an iterator pointing to the last element of the list.
    return --mVertexList.end();
}

void 
Mesh::destroyVertex(VertexPtr vertexPtr)
{
    mVertexList.erase(vertexPtr);
}

VertexPtr
Mesh::vertexBegin()
{
    return mVertexList.begin();
}

VertexPtr
Mesh::vertexEnd()
{
    return mVertexList.end();
}

ConstVertexPtr
Mesh::vertexBegin() const
{
    return mVertexList.begin();
}

ConstVertexPtr
Mesh::vertexEnd() const
{
    return mVertexList.end();
}

VertexList::size_type 
Mesh::vertexCount() const
{
    return mVertexList.size();
}

EdgePtr 
Mesh::createEdge()
{
    mEdgeList.push_back(Edge());

    // Return an iterator pointing to the last element of the list.
    return --mEdgeList.end();
}

void 
Mesh::destroyEdge(EdgePtr edgePtr)
{
    mEdgeList.erase(edgePtr);
}

EdgePtr
Mesh::edgeBegin()
{
    return mEdgeList.begin();
}

EdgePtr
Mesh::edgeEnd()
{
    return mEdgeList.end();
}

ConstEdgePtr
Mesh::edgeBegin() const
{
    return mEdgeList.begin();
}

ConstEdgePtr
Mesh::edgeEnd() const
{
    return mEdgeList.end();
}

EdgeList::size_type 
Mesh::edgeCount() const
{
    return mEdgeList.size();
}

FacePtr 
Mesh::createFace()
{
    mFaceList.push_back(Face());

    // Return an iterator pointing to the last element of the list.
    return --mFaceList.end();
}

void 
Mesh::destroyFace(FacePtr facePtr)
{
    mFaceList.erase(facePtr);
}

FacePtr
Mesh::faceBegin()
{
    return mFaceList.begin();
}

FacePtr
Mesh::faceEnd()
{
    return mFaceList.end();
}

ConstFacePtr
Mesh::faceBegin() const
{
    return mFaceList.begin();
}

ConstFacePtr
Mesh::faceEnd() const
{
    return mFaceList.end();
}

FaceList::size_type 
Mesh::faceCount() const
{
    return mFaceList.size();
}

AttributeKey
Mesh::getAttributeKey(const std::string &name, AttributeKey::Type type,
    unsigned flags) const
{
    return mAttributeKeyMap.getAttributeKey(name, type, flags);
}

bool 
Mesh::hasAttributeKey(const std::string &name) const
{
    return mAttributeKeyMap.hasAttributeKey(name);
}

void 
Mesh::eraseAttributeKey(const std::string &name)
{
    return mAttributeKeyMap.eraseAttributeKey(name);
}

bool
Mesh::findAttributeNameAndKeyFromHandle(AttributeKey::Handle handle,
    const std::string **name, const AttributeKey **attributeKey) const
{
    return mAttributeKeyMap.findAttributeNameAndKeyFromHandle(handle, name, attributeKey);
}

AttributeKeyMap::size_type 
Mesh::attributeKeyMapSize() const
{
    return mAttributeKeyMap.size();
}

AttributeKeyMap::const_iterator 
Mesh::attributeKeyMapBegin() const
{
    return mAttributeKeyMap.begin();
}

AttributeKeyMap::const_iterator 
Mesh::attributeKeyMapEnd() const
{
    return mAttributeKeyMap.end();
}

} // namespace mesh
