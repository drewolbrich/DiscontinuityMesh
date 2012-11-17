// Copyright 2008 Retroactive Fiasco.

#ifndef DELAUNAY__MESH_CONSISTENCY_STATUS__INCLUDED
#define DELAUNAY__MESH_CONSISTENCY_STATUS__INCLUDED

#include <map>

namespace delaunay {

enum MeshInconsistencyType {
    NAN_VERTICES,
    INFINITY_VERTICES,
    DUPLICATE_VERTICES,
    DUPLICATE_EDGES,
    DUPLICATE_FACES,
    VERTICES_WITH_ZERO_EDGES,
    VERTICES_WITH_ONE_EDGE,
    VERTICES_WITH_DUPLICATE_EDGES,
    VERTICES_WITHOUT_FACES,
    VERTICES_WITH_DUPLICATE_FACES,
    EDGES_WITHOUT_TWO_VERTICES,
    EDGES_WITH_DUPLICATE_VERTICES, 
    EDGES_WITHOUT_FACES,
    EDGES_WITH_DUPLICATE_FACES,
    FACES_WITHOUT_THREE_VERTICES,
    FACES_WITH_DUPLICATE_VERTICES,
    FACES_WITHOUT_THREE_EDGES,
    FACES_WITH_DUPLICATE_EDGES,
    FACES_WITH_ZERO_AREA,
    FACES_WITH_CLOCKWISE_VERTICES,
    VERTEX_EDGES_WITHOUT_BACKPOINTER,
    VERTEX_FACES_WITHOUT_BACKPOINTER,
    EDGE_FACES_WITHOUT_BACKPOINTER,
    EDGE_VERTICES_WITHOUT_BACKPOINTER,
    FACE_EDGES_WITHOUT_BACKPOINTER,
    FACE_VERTICES_WITHOUT_BACKPOINTER,
    FACES_WITH_MISMATCHED_VERTEX_EDGE_ORDER,
    EDGES_WITH_FACES_ON_SAME_SIDE

    // When new symbols are added to this list, remember to update
    // the corresponding list in the function AssertMeshConsistency
    // in MeshConsistency.cpp.
};

// MeshConsistencyStatus
//
// Keeps track of the count of each kind of possible mesh inconsistency.

class MeshConsistencyStatus
{
public:
    MeshConsistencyStatus();
    ~MeshConsistencyStatus();

    // Increment the counter for a given MeshInconsistencyType.
    void incrementInconsistency(MeshInconsistencyType meshInconsistencyType);

    // Return the number of occurrences of a given MeshInconsistencyType.
    int inconsistencyCount(MeshInconsistencyType meshInconsistencyType) const;

    // Returns true if the mesh is consistent.
    bool isConsistent() const;

private:
    typedef std::map<MeshInconsistencyType, int> MeshInconsistencyTypeCountMap;
    MeshInconsistencyTypeCountMap mMeshInconsistencyTypeCountMap;
};

} // namespace delaunay

#endif // DELAUNAY__MESH_CONSISTENCY_STATUS__INCLUDED
