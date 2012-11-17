// Copyright 2008 Retroactive Fiasco.

#include "Mesh.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

#include <map>

namespace delaunay {

struct CompareVertexPtr {
    bool operator()(VertexPtr v1, VertexPtr v2) {
        return &*v1 < &*v2;
    }
};

struct CompareEdgePtr {
    bool operator()(EdgePtr e1, EdgePtr e2) {
        return &*e1 < &*e2;
    }
};

struct CompareFacePtr {
    bool operator()(FacePtr f1, FacePtr f2) {
        return &*f1 < &*f2;
    }
};

Mesh::Mesh()
    : mVertexList(),
      mEdgeList(),
      mFaceList(),
      mVertexCount(0),
      mEdgeCount(0),
      mFaceCount(0)
{
}

Mesh::~Mesh()
{
}

MeshPtr
Mesh::clone()
{
    MeshPtr clonedMeshPtr(new Mesh);

    // Loop over each vertex, edge, and face, creating duplicates of each and putting each
    // in a map from the old pointers to the new pointers.

    typedef std::map<VertexPtr, VertexPtr, CompareVertexPtr> VertexMap;
    VertexMap vertexMap;

    typedef std::map<EdgePtr, EdgePtr, CompareEdgePtr> EdgeMap;
    EdgeMap edgeMap;

    typedef std::map<FacePtr, FacePtr, CompareFacePtr> FaceMap;
    FaceMap faceMap;

    for (VertexPtr vertexPtr = vertexBegin(); vertexPtr != vertexEnd(); ++vertexPtr) {
        vertexMap.insert(std::make_pair(vertexPtr, vertexPtr->clone(clonedMeshPtr)));
    }

    for (EdgePtr edgePtr = edgeBegin(); edgePtr != edgeEnd(); ++edgePtr) {
        edgeMap.insert(std::make_pair(edgePtr, edgePtr->clone(clonedMeshPtr)));
    }

    for (FacePtr facePtr = faceBegin(); facePtr != faceEnd(); ++facePtr) {
        faceMap.insert(std::make_pair(facePtr, facePtr->clone(clonedMeshPtr)));
    }

    // Loop over the old vertices, edges and faces, creating the adjacent elements for the
    // corresponding new mesh elements.

    for (VertexPtr vertexPtr = vertexBegin(); vertexPtr != vertexEnd(); ++vertexPtr) {
        VertexPtr clonedVertexPtr = vertexMap.find(vertexPtr)->second;
        for (int index = 0; index < vertexPtr->adjacentEdgeCount(); ++index) {
            clonedVertexPtr->addAdjacentEdge(
                edgeMap.find(vertexPtr->adjacentEdge(index))->second);
        }
        for (int index = 0; index < vertexPtr->adjacentFaceCount(); ++index) {
            clonedVertexPtr->addAdjacentFace(
                faceMap.find(vertexPtr->adjacentFace(index))->second);
        }
    }

    for (EdgePtr edgePtr = edgeBegin(); edgePtr != edgeEnd(); ++edgePtr) {
        EdgePtr clonedEdgePtr = edgeMap.find(edgePtr)->second;
        for (int index = 0; index < edgePtr->adjacentVertexCount(); ++index) {
            clonedEdgePtr->addAdjacentVertex(
                vertexMap.find(edgePtr->adjacentVertex(index))->second);
        }
        for (int index = 0; index < edgePtr->adjacentFaceCount(); ++index) {
            clonedEdgePtr->addAdjacentFace(
                faceMap.find(edgePtr->adjacentFace(index))->second);
        }
    }

    for (FacePtr facePtr = faceBegin(); facePtr != faceEnd(); ++facePtr) {
        FacePtr clonedFacePtr = faceMap.find(facePtr)->second;
        for (int index = 0; index < facePtr->adjacentVertexCount(); ++index) {
            clonedFacePtr->addAdjacentVertex(
                vertexMap.find(facePtr->adjacentVertex(index))->second);
        }
        for (int index = 0; index < facePtr->adjacentEdgeCount(); ++index) {
            clonedFacePtr->addAdjacentEdge(
                edgeMap.find(facePtr->adjacentEdge(index))->second);
        }
    }

    return clonedMeshPtr;
}

VertexPtr
Mesh::createVertex()
{
    mVertexList.push_back(Vertex());
    ++mVertexCount;

    // Returns an iterator that points to the last element of the list.
    return --mVertexList.end();
}

void
Mesh::destroyVertex(VertexPtr vertexPtr)
{
    mVertexList.erase(vertexPtr);
    --mVertexCount;
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

VertexList::size_type
Mesh::vertexCount() const
{
    return mVertexCount;
}

EdgePtr
Mesh::createEdge()
{
    mEdgeList.push_back(Edge());
    ++mEdgeCount;

    // Returns an iterator that points to the last element of the list.
    return --mEdgeList.end();
}

void
Mesh::destroyEdge(EdgePtr edgePtr)
{
    mEdgeList.erase(edgePtr);
    --mEdgeCount;
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

EdgeList::size_type
Mesh::edgeCount() const
{
    return mEdgeCount;
}

FacePtr
Mesh::createFace()
{
    mFaceList.push_back(Face());
    ++mFaceCount;

    // Returns an iterator that points to the last element of the list.
    return --mFaceList.end();
}

void
Mesh::destroyFace(FacePtr facePtr)
{
    mFaceList.erase(facePtr);
    --mFaceCount;
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

FaceList::size_type
Mesh::faceCount() const
{
    return mFaceCount;
}

} // namespace delaunay
