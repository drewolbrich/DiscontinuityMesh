// Copyright 2008 Drew Olbrich

#include "CreateEdgesFromFace.h"

using mesh::VertexPtr;
using mesh::EdgePtr;
using mesh::AdjacentVertexIterator;
using mesh::AdjacentEdgeIterator;

namespace meshobj {

void 
CreateEdgesFromFace(mesh::Mesh &mesh, mesh::FacePtr facePtr)
{
    for (AdjacentVertexIterator adjVertex = facePtr->adjacentVertexBegin();
         adjVertex != facePtr->adjacentVertexEnd(); ++adjVertex) {
        VertexPtr vertexPtr(*adjVertex);

        // Get a pointer to the next adjacent vertex
        // of the face.
        AdjacentVertexIterator nextAdjVertex(adjVertex);
        ++nextAdjVertex;
        if (nextAdjVertex == facePtr->adjacentVertexEnd()) {
            nextAdjVertex = facePtr->adjacentVertexBegin();
        }
        VertexPtr nextVertexPtr(*nextAdjVertex);

        // Check to see if there is already an edge between
        // the two vertices.
        AdjacentEdgeIterator adjEdge 
            = vertexPtr->findAdjacentEdgeByVertex(nextVertexPtr);

        // If the edge does not exist, create it, and attach it
        // to the vertices and vice versa.
        EdgePtr edgePtr;
        if (adjEdge == vertexPtr->adjacentEdgeEnd()) {

            edgePtr = mesh.createEdge();

            edgePtr->addAdjacentVertex(vertexPtr);
            edgePtr->addAdjacentVertex(nextVertexPtr);
            vertexPtr->addAdjacentEdge(edgePtr);
            nextVertexPtr->addAdjacentEdge(edgePtr);

        } else {

            // An edge with these two vertices already exists.
            edgePtr = *adjEdge;
        }

        // Attach the edge to the face and vice versa.
        edgePtr->addAdjacentFace(facePtr);
        facePtr->addAdjacentEdge(edgePtr);
    }
}

} // namespace meshobj
