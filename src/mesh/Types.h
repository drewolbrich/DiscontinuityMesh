// Copyright 2008 Drew Olbrich

#ifndef MESH__TYPES__INCLUDED
#define MESH__TYPES__INCLUDED

#include <list>
#include <vector>
#include <ostream>

namespace mesh {

// Forward declarations of the mesh element types.
class Vertex;
class Edge;
class Face;

// List of elements owned by the mesh.
typedef std::list<Vertex> VertexList;
typedef std::list<Edge> EdgeList;
typedef std::list<Face> FaceList;

// Iterators for the mesh's element lists.
typedef VertexList::iterator VertexPtr;
typedef EdgeList::iterator EdgePtr;
typedef FaceList::iterator FacePtr;
typedef VertexList::const_iterator ConstVertexPtr;
typedef EdgeList::const_iterator ConstEdgePtr;
typedef FaceList::const_iterator ConstFacePtr;

// Vectors of element pointers that the mesh elements use to 
// keep track of adjacent elements.
typedef std::vector<VertexPtr> AdjacentVertexVector;
typedef std::vector<EdgePtr> AdjacentEdgeVector;
typedef std::vector<FacePtr> AdjacentFaceVector;

// Iterators for the vectors of pointers to adjacent elements.
// Dereference these to get a VertexPtr, EdgePtr, or FacePtr.
typedef AdjacentVertexVector::iterator AdjacentVertexIterator;
typedef AdjacentEdgeVector::iterator AdjacentEdgeIterator;
typedef AdjacentFaceVector::iterator AdjacentFaceIterator;
typedef AdjacentVertexVector::const_iterator AdjacentVertexConstIterator;
typedef AdjacentEdgeVector::const_iterator AdjacentEdgeConstIterator;
typedef AdjacentFaceVector::const_iterator AdjacentFaceConstIterator;
typedef AdjacentVertexVector::reverse_iterator AdjacentVertexReverseIterator;
typedef AdjacentEdgeVector::reverse_iterator AdjacentEdgeReverseIterator;
typedef AdjacentFaceVector::reverse_iterator AdjacentFaceReverseIterator;
typedef AdjacentVertexVector::const_reverse_iterator 
    AdjacentVertexConstReverseIterator;
typedef AdjacentEdgeVector::const_reverse_iterator 
    AdjacentEdgeConstReverseIterator;
typedef AdjacentFaceVector::const_reverse_iterator 
    AdjacentFaceConstReverseIterator;

bool operator<(const VertexPtr &lhs, const VertexPtr &rhs);
bool operator<(const EdgePtr &lhs, const EdgePtr &rhs);
bool operator<(const FacePtr &lhs, const FacePtr &rhs);
bool operator<(const ConstVertexPtr &lhs, const ConstVertexPtr &rhs);
bool operator<(const ConstEdgePtr &lhs, const ConstEdgePtr &rhs);
bool operator<(const ConstFacePtr &lhs, const ConstFacePtr &rhs);

std::ostream &operator<<(std::ostream &ostr, const VertexPtr &rhs);
std::ostream &operator<<(std::ostream &ostr, const EdgePtr &rhs);
std::ostream &operator<<(std::ostream &ostr, const FacePtr &rhs);

} // namespace mesh

#endif // MESH__TYPES__INCLUDED
