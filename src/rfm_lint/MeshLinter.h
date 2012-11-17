// Copyright 2009 Retroactive Fiasco.

#ifndef RFM_LINT__MESH_LINTER__INCLUDED
#define RFM_LINT__MESH_LINTER__INCLUDED

#include <mesh/Types.h>

namespace mesh {
class Mesh;
}

// MeshLinter
//
// Scans for errors or potential problems in a mesh.

class MeshLinter
{
public:
    MeshLinter();
    ~MeshLinter();

    // The mesh to test.
    void setMesh(mesh::Mesh *mesh);
    mesh::Mesh *mesh() const;

    // Absolute tolerance for geometry tests.
    void setAbsoluteTolerance(float absoluteTolerance);
    float absoluteTolerance() const;

    // Absolute tolerance for geometry tests.
    void setRelativeTolerance(float relativeTolerance);
    float relativeTolerance() const;

    // If true, errors in the mesh should be marked with geometry.
    void setShouldMarkErrors(bool shouldMarkErrors);
    bool shouldMarkErrors() const;

    // Run the test. Returns true if there are no errors in the mesh.
    bool testMesh();

private:
    // Returns true if the mesh has one or more degenerate faces.
    bool meshHasDegenerateFaces(mesh::Mesh &mesh);

    // Mark errors in the mesh with additional geometry.
    void markErrors(mesh::Mesh *mesh);

    mesh::Mesh *mMesh;
    float mAbsoluteTolerance;
    float mRelativeTolerance;
    bool mShouldMarkErrors;

    typedef std::vector<mesh::FacePtr> FaceVector;
    FaceVector mMarkedFaceVector;
};

#endif // RFM_LINT__MESH_LINTER__INCLUDED
