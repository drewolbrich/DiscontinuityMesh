// Copyright 2008 Drew Olbrich

#include "MeshConsistencyStatus.h"

namespace delaunay {

MeshConsistencyStatus::MeshConsistencyStatus()
    : mMeshInconsistencyTypeCountMap()
{
}

MeshConsistencyStatus::~MeshConsistencyStatus()
{
}

void
MeshConsistencyStatus::incrementInconsistency(MeshInconsistencyType meshInconsistencyType)
{
    MeshInconsistencyTypeCountMap::const_iterator iterator
        = mMeshInconsistencyTypeCountMap.find(meshInconsistencyType);
    if (iterator == mMeshInconsistencyTypeCountMap.end()) {
        mMeshInconsistencyTypeCountMap.insert(
            std::make_pair(meshInconsistencyType, 1));
        return;
    }

    int count = (*iterator).second;
    ++count;
    mMeshInconsistencyTypeCountMap.insert(
        std::make_pair(meshInconsistencyType, count));
}

int
MeshConsistencyStatus::inconsistencyCount(MeshInconsistencyType meshInconsistencyType) const
{
    MeshInconsistencyTypeCountMap::const_iterator iterator
        = mMeshInconsistencyTypeCountMap.find(meshInconsistencyType);
    if (iterator == mMeshInconsistencyTypeCountMap.end()) {
        return 0;
    }

    return (*iterator).second;
}

bool
MeshConsistencyStatus::isConsistent() const
{
    return mMeshInconsistencyTypeCountMap.size() == 0;
}

} // namespace delaunay
