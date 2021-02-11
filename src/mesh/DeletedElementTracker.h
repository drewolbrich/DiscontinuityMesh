// Copyright 2009 Drew Olbrich

#ifndef MESH__DELETED_ELEMENT_TRACKER__INCLUDED
#define MESH__DELETED_ELEMENT_TRACKER__INCLUDED

#include "Types.h"

#include <set>

namespace mesh {

// DeletedElementTracker
//
// Class for keeping track of sets of vertices, edges, and faces that have
// been deleted, and should not be referenced again. Useful for avoiding
// following dangling pointers.
//
// Instances of this class may be passed to functions like CollapseEdge,
// which may have the side effect of deleting vertices, edges, or faces.
// CollapseEdge will use DeletedElementTracker to keep track of the deleted elements.
// The caller may then test elements against the DeletedElementTracker to avoid
// referencing dangling pointers.
//
// Elements tracked by DeletedElementTracker are only represented as pointers,
// so it's entirely possible that newly created elements will wind up with pointers
// that are already tracked as deleted.

class DeletedElementTracker
{
public:
    DeletedElementTracker();
    ~DeletedElementTracker();

    // Clear out all the elements referenced by the DeletedElementTracker.
    void clear();

    // Mask to control which element types can be added to the DeletedElementTracker.
    enum {
        VERTICES = 1 << 0,
        EDGES = 1 << 1,
        FACES = 1 << 2,
        NONE = 0,
        ALL = VERTICES | EDGES | FACES
    };
    void setElementMask(unsigned elementMask);
    unsigned elementMask() const;

    // Add elements to the DeletedElementTracker.
    // Element types not represented by the element mask will not be added.
    void addVertex(VertexPtr vertexPtr);
    void addEdge(EdgePtr edgePtr);
    void addFace(FacePtr facePtr);

    // Returns true if the DeletedElementTracker contains the specified element.
    bool hasVertex(VertexPtr vertexPtr);
    bool hasEdge(EdgePtr edgePtr);
    bool hasFace(FacePtr facePtr);

    // Iterators for looping over the tracked elements.
    typedef std::set<VertexPtr> VertexPtrSet;
    typedef std::set<EdgePtr> EdgePtrSet;
    typedef std::set<FacePtr> FacePtrSet;
    typedef VertexPtrSet::const_iterator VertexIterator;
    typedef EdgePtrSet::const_iterator EdgeIterator;
    typedef FacePtrSet::const_iterator FaceIterator;
    VertexIterator vertexBegin() const;
    VertexIterator vertexEnd() const;
    EdgeIterator edgeBegin() const;
    EdgeIterator edgeEnd() const;
    FaceIterator faceBegin() const;
    FaceIterator faceEnd() const;

    // Add the elements tracked by another DeletedElementTracker.
    // The other DeletedElementTracker's mask must be a superset of the
    // current DeletedElementTracker's mask.
    void addElementsFromElementTracker(const DeletedElementTracker &rhs);

private:
    unsigned mElementMask;

    VertexPtrSet mVertexPtrSet;
    EdgePtrSet mEdgePtrSet;
    FacePtrSet mFacePtrSet;
};

} // namespace mesh

#endif // MESH__DELETED_ELEMENT_TRACKER__INCLUDED
