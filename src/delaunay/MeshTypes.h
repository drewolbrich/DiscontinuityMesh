// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH_TYPES__INCLUDED
#define DELAUNAY__MESH_TYPES__INCLUDED

#include <list>

namespace delaunay {

class Vertex;
class Edge;
class Face;

typedef std::list<Vertex> VertexList;
typedef std::list<Edge> EdgeList;
typedef std::list<Face> FaceList;

typedef VertexList::iterator VertexPtr;
typedef EdgeList::iterator EdgePtr;
typedef FaceList::iterator FacePtr;

} // namespace delaunay

#endif // DELAUNAY__MESH_TYPES__INCLUDED
