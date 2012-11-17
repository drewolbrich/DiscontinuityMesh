// Copyright 2008 Retroactive Fiasco.

#ifndef CGMATH__AABB_TREE__INCLUDED
#define CGMATH__AABB_TREE__INCLUDED

#include <cassert>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>

#include "Vector3f.h"
#include "BoundingBox3f.h"
#include "BoundingBox3fOperations.h"
#include "AabbTreeNode.h"

namespace cgmath {

// AabbTree
//
// An axis-aligned bounding box tree.
//
// The template parameter class OBJECT should have the following
// member function defined:
//
// cgmath::BoundingBox3f boundingBox() const;
//     Returns the 3D bounding box of the object.

template<typename OBJECT>
class AabbTree
{
public:
    AabbTree();
    ~AabbTree();

    // Initialize the AABB tree.
    typedef std::vector<OBJECT> ObjectVector;
    typedef typename ObjectVector::const_iterator ObjectVectorConstIterator;
    void initialize(const ObjectVector &objectVector);

    // A string describing the AABB tree size at each level.
    std::string sizeStatistics() const;

    // A string describing AABB tree statistics after it has been used.
    std::string queryStatistics() const;
    
    // Clear out all the nodes from the tree.
    void clear();

    class BoundingBoxListener {
    public:
        virtual ~BoundingBoxListener() {}
        // If this function returns true, traversal of the AABB tree halts.
        virtual bool applyObjectToBoundingBox(OBJECT &object, 
            const BoundingBox3f &boundingBox) = 0;
    };

    // Apply a listener to all objects in the AABB tree whose bounding
    // boxes intersect the specified bounding box.
    // Returns true if a listener function call returned true.
    bool applyToBoundingBoxIntersection(const BoundingBox3f &boundingBox,
        BoundingBoxListener *boundingBoxListener) const;

    class Triangle {
    public:
        Vector3f mPointArray[3];
    };
    typedef std::vector<Triangle> TriangleVector;

    class TriangleListener {
    public:
        virtual ~TriangleListener() {}
        // If this function returns true, traversal of the AABB tree halts.
        virtual bool applyObjectToTriangleVector(OBJECT &object, 
            const TriangleVector &triangleVector) = 0;
    };

    // Calls a TriangleListener on all AABB nodes that intersect the specified 
    // array of triangles.
    // Returns true if a listener function call returned true.
    bool applyToTriangleVectorIntersection(const TriangleVector &triangleVector,
        TriangleListener *triangleListener) const;

    class TetrahedronListener {
    public:
        virtual ~TetrahedronListener() {}

        // If this function returns false, traversal of the AABB tree halts.
        virtual bool applyObjectToTetrahedron(OBJECT &object, 
            const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3) = 0;
    };

    // Calls a TetrahedronListener on all AABB nodes that intersect the specified
    // tetrahedron.
    // Returns true if a listener function call returned true.
    bool applyToTetrahedronIntersection(const Vector3f &v0, const Vector3f &v1, 
        const Vector3f &v2, const Vector3f &v3,
        TetrahedronListener *tetrahedronListener) const;

    class RaySegmentOcclusionListener {
    public:
        virtual ~RaySegmentOcclusionListener() {}

        // This function should return true if the ray segment
        // intersects the object. It should not return true if an object
        // is intersected beyond the far endpoint of the ray.
        virtual bool objectOccludesRaySegment(const OBJECT &object, 
            const Vector3f &origin, const Vector3f &endpoint) const = 0;
    };

    // Returns true if the specified ray intersects one or more
    // objects in the AABB tree. The callback function should return
    // true if the ray segment intersects the object.
    bool occludesRaySegment(const Vector3f &origin, const Vector3f &endpoint, 
        const RaySegmentOcclusionListener *raySegmentOcclusionListener) const;

    class RaySegmentIntersectionListener {
    public:
        virtual ~RaySegmentIntersectionListener() {}

        // This function should return true if the ray segment
        // intersects the object at a point with a smaller value of 't'.
        // 't' should be updated with the new, smaller value.
        // t=0 at the ray origin, and t=1 at the ray endpoint.
        // The function should not return true if an object is intersected
        // beyond the far endpoint of the ray.
        virtual bool objectIntersectsRaySegment(const OBJECT &object, 
            const Vector3f &origin, const Vector3f &endpoint,
            float *t) const = 0;
    };

    // Returns true if the specified ray segment intersects one or more
    // objects in the AABB tree. If true is returned, the point of intersection
    // closest to the ray origin is returned via intersectionPoint.
    // A pointer to the intersected object is also returned.
    // The callback function should return true if the ray segment intersects
    // the object at a point with a closer value of 't'.
    bool intersectsRaySegment(const Vector3f &origin, const Vector3f &endpoint, 
        const RaySegmentIntersectionListener *raySegmentIntersectionListener,
        cgmath::Vector3f *intersectionPoint, OBJECT **intersectedObject) const;

    // The number of intersection tests performed.
    unsigned queries() const;

    // The number of bounding box tests performed.
    unsigned averageBoundingBoxTestsPerQuery() const;
    unsigned minBoundingBoxTestsPerQuery() const;
    unsigned maxBoundingBoxTestsPerQuery() const;

    // The number of object intersection tests (via listener) performed.
    unsigned averageObjectTestsPerQuery() const;
    unsigned minObjectTestsPerQuery() const;
    unsigned maxObjectTestsPerQuery() const;
    
private:
    // These unimplemented declarations prevent objects of this class
    // from being copied. That would be dangerous, since this
    // class contains a pointer.
    AabbTree(const AabbTree &);
    void operator=(const AabbTree &);

    // Delete an AabbTreeNode and its descendents.
    void deleteAabbTreeNodeAndDescendants(AabbTreeNode<OBJECT> *aabbTreeNode);

    // This is used by initialize and createAabbSubtree to manage a
    // temporary array for sorting the objects as they're being
    // placed in the tree. The midpoint and size are precomputed,
    // rather than being computed on the fly by the functors below,
    // because that would potentially cause floating point roundoff 
    // errors that would throw off STL's sort algorithm.
    typedef struct {
        Vector3f mMidpoint;
        Vector3f mSize;
        const OBJECT *mObject;
    } ObjectPtr;
    typedef std::vector<ObjectPtr> ObjectPtrVector;
    typedef typename ObjectPtrVector::iterator ObjectPtrVectorIterator;
    
    // This functor is used to sort objects by their size
    // on a particular axis. Largest objects are positioned first.
    class SortBySizeFunctor {
    public:
        void setAxis(unsigned axis);
        bool operator()(const ObjectPtr &lhs, const ObjectPtr &rhs) const;
    private:
        unsigned mAxis;
    };

    // This functor is used to sort objects by their position
    // on a particular axis.
    class SortByMidpointFunctor {
    public:
        void setAxis(unsigned axis);
        bool operator()(const ObjectPtr &lhs, const ObjectPtr &rhs) const;
    private:
        unsigned mAxis;
    };

    // Create a subtree of the AABB tree, given a subrange
    // of a vector of objects to place in the tree.
    AabbTreeNode<OBJECT> *createAabbSubtree(ObjectPtrVectorIterator first,
        ObjectPtrVectorIterator last, unsigned level);

    // Apply a listener to all objects in an AABB subtree whose
    // bounding boxes intersect the specified bounding box. If the
    // callback returns false, the evaluation of the AABB tree halts.
    void applyToBoundingBoxIntersectionForSubtree(
        AabbTreeNode<OBJECT> *aabbTreeNode,
        bool *halted, const BoundingBox3f &boundingBox,
        BoundingBoxListener *boundingBoxListener) const;

    // Apply the triangle intersection test to an AABB subtree.
    void applyToTriangleVectorIntersectionForSubtree(
        AabbTreeNode<OBJECT> *aabbTreeNode, 
        bool *halted, const TriangleVector &triangleVector,
        TriangleListener *triangleListener) const;

    // Apply the tetrahedron intersection test to an AABB subtree.
    void applyToTetrahedronIntersectionForSubtree(
        AabbTreeNode<OBJECT> *aabbTreeNode, bool *halted, 
        const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3,
        TetrahedronListener *tetrahedronListener) const;

    // Apply the ray segment occlusion test to an AABB subtree.
    bool occludesRaySegmentForSubtree(
        AabbTreeNode<OBJECT> *aabbTreeNode, bool *halted, 
        const Vector3f &origin, const Vector3f &endpoint,
        const RaySegmentOcclusionListener *raySegmentOcclusionListener) const;

    // Apply the ray segment intersection test to an AABB subtree.
    bool intersectsRaySegmentForSubtree(
        AabbTreeNode<OBJECT> *aabbTreeNode,
        const Vector3f &origin, const Vector3f &endpoint, 
        const RaySegmentIntersectionListener *raySegmentIntersectionListener,
        float *t, OBJECT **intersectedObject) const;

    // Update the overall usage data based on the current query.
    void updateUsageDataFromCurrentQuery() const;

    AabbTreeNode<OBJECT> *mRootNode;

    unsigned mDepth;
    std::vector<unsigned> mNodesAtLevel;
    std::vector<float> mMinSizeAtLevel;
    std::vector<float> mMaxSizeAtLevel;
    std::vector<float> mAverageSizeAtLevel;

    mutable unsigned mQueries;
    mutable unsigned mBoundingBoxTests;
    mutable unsigned mCurrentQueryBoundingBoxTests;
    mutable unsigned mMinBoundingBoxTestsPerQuery;
    mutable unsigned mMaxBoundingBoxTestsPerQuery;
    mutable unsigned mObjectTests;
    mutable unsigned mCurrentQueryObjectTests;
    mutable unsigned mMinObjectTestsPerQuery;
    mutable unsigned mMaxObjectTestsPerQuery;
};

template<typename OBJECT>
AabbTree<OBJECT>::AabbTree()
    : mRootNode(NULL),
      mDepth(0),
      mNodesAtLevel(),
      mMinSizeAtLevel(),
      mMaxSizeAtLevel(),
      mAverageSizeAtLevel(),
      mQueries(0),
      mBoundingBoxTests(0),
      mCurrentQueryBoundingBoxTests(0),
      mMinBoundingBoxTestsPerQuery(0),
      mMaxBoundingBoxTestsPerQuery(0),
      mObjectTests(0),
      mCurrentQueryObjectTests(0),
      mMinObjectTestsPerQuery(0),
      mMaxObjectTestsPerQuery(0)
{
}

template<typename OBJECT>
AabbTree<OBJECT>::~AabbTree()
{
    if (mRootNode != NULL) {
        deleteAabbTreeNodeAndDescendants(mRootNode);
        mRootNode = NULL;
    }
}

template<typename OBJECT>
void 
AabbTree<OBJECT>::initialize(const ObjectVector &objectVector)
{
    // If there are already nodes in the tree, destroy them.
    if (mRootNode != NULL) {
        deleteAabbTreeNodeAndDescendants(mRootNode);
        mRootNode = NULL;
    }

    mDepth = 0;
    mNodesAtLevel.clear();
    mMinSizeAtLevel.clear();
    mMaxSizeAtLevel.clear();
    mAverageSizeAtLevel.clear();

    // Create a temporary vector of pointers and midpoints 
    // that's used in the creation of the AABB tree.
    ObjectPtrVector objectPtrVector;
    objectPtrVector.reserve(objectVector.size());
    for (ObjectVectorConstIterator iterator = objectVector.begin();
         iterator != objectVector.end(); ++iterator) {
        const OBJECT &object = *iterator;
        ObjectPtr objectPtr;
        const BoundingBox3f &boundingBox = object.boundingBox();
        objectPtr.mMidpoint = (boundingBox.min() + boundingBox.max())/2.0;
        objectPtr.mSize = boundingBox.max() - boundingBox.min();
        objectPtr.mObject = &object;
        objectPtrVector.push_back(objectPtr);
    }
    
    mRootNode = createAabbSubtree(objectPtrVector.begin(),
        objectPtrVector.end(), 0);

    for (unsigned level = 0; level < mDepth; ++level) {
        mAverageSizeAtLevel[level] /= mNodesAtLevel[level];
    }

    mQueries = 0;
    mBoundingBoxTests = 0;
    mMinBoundingBoxTestsPerQuery = std::numeric_limits<unsigned>::max();
    mMaxBoundingBoxTestsPerQuery = 0;
    mObjectTests = 0;
    mMinObjectTestsPerQuery = std::numeric_limits<unsigned>::max();
    mMaxObjectTestsPerQuery = 0;
}

template<typename OBJECT>
std::string 
AabbTree<OBJECT>::sizeStatistics() const
{
    std::ostringstream ostr;

    for (unsigned level = 0; level < mDepth; ++level) {
        ostr << "Level " << level << ": "
            << mNodesAtLevel[level] << " node" << (mNodesAtLevel[level] != 1 ? "s" : "")
            << ", average size = " << mAverageSizeAtLevel[level]
            << ", min size = " << mMinSizeAtLevel[level] 
            << ", max size = " << mMaxSizeAtLevel[level];
        if (level < mDepth - 1) {
            ostr << std::endl;
        }
    }

    return ostr.str();
}

template<typename OBJECT>
std::string 
AabbTree<OBJECT>::queryStatistics() const
{
    std::ostringstream ostr;

    ostr << "Queries: " << queries() << "\n";
    ostr << "Average bounding box tests per query: " << averageBoundingBoxTestsPerQuery() << "\n";
    ostr << "Min bounding box tests per query: " << minBoundingBoxTestsPerQuery() << "\n";
    ostr << "Max bounding box tests per query: " << maxBoundingBoxTestsPerQuery() << "\n";
    ostr << "Average object tests per query: " << averageObjectTestsPerQuery() << "\n";
    ostr << "Min object tests per query: " << minObjectTestsPerQuery() << "\n";
    ostr << "Max object tests per query: " << maxObjectTestsPerQuery();

    return ostr.str();
}

template<typename OBJECT>
void 
AabbTree<OBJECT>::clear()
{
    if (mRootNode != NULL) {
        deleteAabbTreeNodeAndDescendants(mRootNode);
        mRootNode = NULL;
    }
}

template<typename OBJECT>
bool
AabbTree<OBJECT>::applyToBoundingBoxIntersection(const BoundingBox3f &boundingBox,
    BoundingBoxListener *boundingBoxListener) const
{
    assert(boundingBoxListener != NULL);

    if (mRootNode == NULL) {
        return false;
    }

    bool halted = false;

    ++mQueries;

    mCurrentQueryBoundingBoxTests = 0;
    mCurrentQueryObjectTests = 0;
    if (mRootNode != NULL) {
        applyToBoundingBoxIntersectionForSubtree(mRootNode,
            &halted, boundingBox, boundingBoxListener);
    }

    updateUsageDataFromCurrentQuery();

    return halted;
}

template<typename OBJECT>
bool
AabbTree<OBJECT>::applyToTriangleVectorIntersection(const TriangleVector &triangleVector,
    TriangleListener *triangleListener) const
{
    assert(triangleListener != NULL);

    if (mRootNode == NULL) {
        return false;
    }

    bool halted = false;

    ++mQueries;

    mCurrentQueryBoundingBoxTests = 0;
    mCurrentQueryObjectTests = 0;
    
    if (mRootNode != NULL) {
        applyToTriangleVectorIntersectionForSubtree(mRootNode, 
            &halted, triangleVector, triangleListener);
    }

    updateUsageDataFromCurrentQuery();

    return halted;
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::applyToTetrahedronIntersection(const Vector3f &v0, const Vector3f &v1, 
    const Vector3f &v2, const Vector3f &v3,
    TetrahedronListener *tetrahedronListener) const
{
    assert(tetrahedronListener != NULL);

    if (mRootNode == NULL) {
        return false;
    }

    bool halted = false;

    ++mQueries;

    mCurrentQueryBoundingBoxTests = 0;
    mCurrentQueryObjectTests = 0;
    
    if (mRootNode != NULL) {
        applyToTetrahedronIntersectionForSubtree(mRootNode, &halted, v0, v1, v2, v3,
            tetrahedronListener);
    }

    updateUsageDataFromCurrentQuery();

    return halted;
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::occludesRaySegment(const Vector3f &origin, const Vector3f &endpoint, 
    const RaySegmentOcclusionListener *raySegmentOcclusionListener) const
{
    assert(raySegmentOcclusionListener != NULL);

    if (mRootNode == NULL) {
        return false;
    }

    ++mQueries;

    mCurrentQueryBoundingBoxTests = 0;
    mCurrentQueryObjectTests = 0;
    
    bool halted = false;
    bool result = occludesRaySegmentForSubtree(mRootNode, &halted,
        origin, endpoint, raySegmentOcclusionListener);

    updateUsageDataFromCurrentQuery();
    
    return result;
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::intersectsRaySegment(const Vector3f &origin, const Vector3f &endpoint, 
    const RaySegmentIntersectionListener *raySegmentIntersectionListener,
    cgmath::Vector3f *intersectionPoint, OBJECT **intersectedObject) const
{
    assert(raySegmentIntersectionListener != NULL);
    assert(intersectionPoint != NULL);
    assert(intersectedObject != NULL);

    if (mRootNode == NULL) {
        return false;
    }

    ++mQueries;

    mCurrentQueryBoundingBoxTests = 0;
    mCurrentQueryObjectTests = 0;

    float t = 1.0;
    *intersectedObject = NULL;
    bool result = intersectsRaySegmentForSubtree(mRootNode,
        origin, endpoint, raySegmentIntersectionListener, &t, intersectedObject);
    *intersectionPoint = origin*(1.0 - t) + endpoint*t;

    updateUsageDataFromCurrentQuery();
    
    return result;
}

template<typename OBJECT>
unsigned
AabbTree<OBJECT>::queries() const
{
    return mQueries;
}

template<typename OBJECT>
unsigned 
AabbTree<OBJECT>::averageBoundingBoxTestsPerQuery() const
{
    if (mQueries == 0) {
        return 0;
    }

    return mBoundingBoxTests/mQueries;
}

template<typename OBJECT>
unsigned 
AabbTree<OBJECT>::minBoundingBoxTestsPerQuery() const
{
    return mMinBoundingBoxTestsPerQuery;
}

template<typename OBJECT>
unsigned 
AabbTree<OBJECT>::maxBoundingBoxTestsPerQuery() const
{
    return mMaxBoundingBoxTestsPerQuery;
}

template<typename OBJECT>
unsigned 
AabbTree<OBJECT>::averageObjectTestsPerQuery() const
{
    if (mQueries == 0) {
        return 0;
    }

    return mObjectTests/mQueries;
}

template<typename OBJECT>
unsigned 
AabbTree<OBJECT>::minObjectTestsPerQuery() const
{
    return mMinObjectTestsPerQuery;
}

template<typename OBJECT>
unsigned 
AabbTree<OBJECT>::maxObjectTestsPerQuery() const
{
    return mMaxObjectTestsPerQuery;
}

template<typename OBJECT>
void
AabbTree<OBJECT>::deleteAabbTreeNodeAndDescendants(
    AabbTreeNode<OBJECT> *aabbTreeNode)
{
    if (aabbTreeNode->leftNode() != NULL) {
        deleteAabbTreeNodeAndDescendants(aabbTreeNode->leftNode());
    }

    if (aabbTreeNode->rightNode() != NULL) {
        deleteAabbTreeNodeAndDescendants(aabbTreeNode->rightNode());
    }

    delete aabbTreeNode;
}

template<typename OBJECT>
AabbTreeNode<OBJECT> *
AabbTree<OBJECT>::createAabbSubtree(ObjectPtrVectorIterator first,
    ObjectPtrVectorIterator last, unsigned level)
{
    // Create the new node at the top of the subtree.
    AabbTreeNode<OBJECT> *aabbTreeNode = new AabbTreeNode<OBJECT>;

    if (level + 1 > mDepth) {
        mDepth = level + 1;
        mNodesAtLevel.resize(mDepth, 0);
        mMinSizeAtLevel.resize(mDepth, std::numeric_limits<float>::max());
        mMaxSizeAtLevel.resize(mDepth, 0.0);
        mAverageSizeAtLevel.resize(mDepth, 0.0);
    }

    assert(level < mNodesAtLevel.size());

    ++mNodesAtLevel[level];
    
    // Compute the bounding box of all of the objects in the range.
    BoundingBox3f boundingBox;
    boundingBox.reset();
    for (ObjectPtrVectorIterator iterator = first; iterator != last; ++iterator) {
        const ObjectPtr &objectPtr = *iterator;
        const OBJECT &object = *objectPtr.mObject;
        const BoundingBox3f &objectBoundingBox = object.boundingBox();
        boundingBox.extendByVector3f(objectBoundingBox.min());
        boundingBox.extendByVector3f(objectBoundingBox.max());
    }

    assert(!boundingBox.empty());

    aabbTreeNode->setBoundingBox(boundingBox);

    // Determine the longest axis of the objects in the range.
    unsigned longestAxis = 0;
    const Vector3f &size = boundingBox.max() - boundingBox.min();
    if (size[0] >= size[1] && size[0] >= size[2]) {
        longestAxis = 0;
    } else if (size[1] >= size[0] && size[1] >= size[2]) {
        longestAxis = 1;
    } else {
        longestAxis = 2;
    }

    assert(level < mMinSizeAtLevel.size());
    assert(level < mMaxSizeAtLevel.size());
    assert(level < mAverageSizeAtLevel.size());

    if (size[longestAxis] < mMinSizeAtLevel[level]) {
        mMinSizeAtLevel[level] = size[longestAxis];
    }
    if (size[longestAxis] > mMaxSizeAtLevel[level]) {
        mMaxSizeAtLevel[level] = size[longestAxis];
    }
    mAverageSizeAtLevel[level] += size[longestAxis];

    // Sort the objects by size (largest objects first).
    SortBySizeFunctor sortBySizeFunctor;
    sortBySizeFunctor.setAxis(longestAxis);
    std::sort(first, last, sortBySizeFunctor);

    // Find all the objects that are as large as the
    // bounding box along the chosen axis and add them
    // to the node. This places large objects higher up
    // in the tree, so that they're encountered first
    // during traversal.
    while (first != last
        && ((*first).mObject->boundingBox().maxAxis(longestAxis)
            - (*first).mObject->boundingBox().minAxis(longestAxis))
        /(boundingBox.maxAxis(longestAxis) 
            - boundingBox.minAxis(longestAxis))
        > 0.9999) {
        aabbTreeNode->addObject(*(first->mObject));
        ++first;
    }

    // If all the objects have been placed in the node,
    // return it.
    if (first == last) {
        return aabbTreeNode;
    }

    // If there's only a small number of objects left in the range, add them to the
    // node and return.
    // Experimentally, this seems to work best with one object per leaf node.
    if (last - first <= 1) {
        for (ObjectPtrVectorIterator iterator = first; iterator != last; ++iterator) {
            aabbTreeNode->addObject(*((*iterator).mObject));
        }
        return aabbTreeNode;
    }

    // Sort the objects along the longest axis by their midpoint.
    SortByMidpointFunctor sortByMidpointFunctor;
    sortByMidpointFunctor.setAxis(longestAxis);
    std::sort(first, last, sortByMidpointFunctor);

    // Find the median object in the range, and create two
    // subtrees, using that object as the partition.
    ObjectPtrVectorIterator median = first + (last - first)/2;

    // We must have broken up the range into two non-empty ranges.
    assert(median != first);
    assert(median != last);
    
    // Create the two subtrees hanging off this node.
    aabbTreeNode->setLeftNode(createAabbSubtree(first, median, level + 1));
    aabbTreeNode->setRightNode(createAabbSubtree(median, last, level + 1));
    
    return aabbTreeNode;
}

template<typename OBJECT>
void 
AabbTree<OBJECT>::SortBySizeFunctor::setAxis(unsigned axis)
{
    mAxis = axis;
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::SortBySizeFunctor::operator()(const ObjectPtr &lhs, 
    const ObjectPtr &rhs) const
{
    return lhs.mSize[mAxis] > rhs.mSize[mAxis];
}

template<typename OBJECT>
void 
AabbTree<OBJECT>::SortByMidpointFunctor::setAxis(unsigned axis)
{
    mAxis = axis;
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::SortByMidpointFunctor::operator()(const ObjectPtr &lhs, 
    const ObjectPtr &rhs) const
{
    return lhs.mMidpoint[mAxis] < rhs.mMidpoint[mAxis];
}

template<typename OBJECT> 
void
AabbTree<OBJECT>::applyToBoundingBoxIntersectionForSubtree(
    AabbTreeNode<OBJECT> *aabbTreeNode,
    bool *halted, const BoundingBox3f &boundingBox,
    BoundingBoxListener *boundingBoxListener) const
{
    if (*halted) {
        return;
    }

    while (true) {
        ++mBoundingBoxTests;
        ++mCurrentQueryBoundingBoxTests;

        // If the specified bounding box doesn't intersect
        // the node's bounding box, skip this subtree.
        if (!BoundingBox3fIntersectsBoundingBox3f(boundingBox, aabbTreeNode->boundingBox())) {
            return;
        }

        // Evaluate the callback on all of the objects in this node.
        for (typename AabbTreeNode<OBJECT>::ObjectVectorIterator iterator
                 = aabbTreeNode->objectBegin();
             iterator != aabbTreeNode->objectEnd(); ++iterator) {
            OBJECT &object = *iterator;

            ++mObjectTests;
            ++mCurrentQueryObjectTests;

            // If the callback returns true, skip all further processing.
            if (boundingBoxListener->applyObjectToBoundingBox(object, boundingBox)) {
                *halted = true;
                return;
            }
        }

        // Evaluate the left subtree.
        if (aabbTreeNode->leftNode() != NULL) {
            applyToBoundingBoxIntersectionForSubtree(aabbTreeNode->leftNode(),
                halted, boundingBox, boundingBoxListener);
            if (*halted) {
                return;
            }
        }

        // To avoid function call overhead, loop on the right subtree.
        // rather than using recursion.
        if (aabbTreeNode->rightNode() != NULL) {
            aabbTreeNode = aabbTreeNode->rightNode();
        } else {
            break;
        }
    }
}

template<typename OBJECT>
void
AabbTree<OBJECT>::applyToTriangleVectorIntersectionForSubtree(
    AabbTreeNode<OBJECT> *aabbTreeNode, 
    bool *halted, const TriangleVector &triangleVector,
    TriangleListener *triangleListener) const
{
    if (*halted) {
        return;
    }

    while (true) {
        ++mBoundingBoxTests;
        ++mCurrentQueryBoundingBoxTests;

        // If none of the specified triangles intersect
        // the node's bounding box, skip this subtree.
        bool foundIntersection = false;
        for (unsigned index = 0; index < triangleVector.size(); ++index) {
            if (BoundingBox3fIntersectsTriangle(aabbTreeNode->boundingBox(), 
                    triangleVector[index].mPointArray[0],
                    triangleVector[index].mPointArray[1],
                    triangleVector[index].mPointArray[2])) {
                foundIntersection = true;
                break;
            }
        }
        if (!foundIntersection) {
            return;
        }

        // Evaluate the callback on all of the objects in this node.
        for (typename AabbTreeNode<OBJECT>::ObjectVectorIterator iterator
                 = aabbTreeNode->objectBegin();
             iterator != aabbTreeNode->objectEnd(); ++iterator) {
            OBJECT &object = *iterator;

            ++mObjectTests;
            ++mCurrentQueryObjectTests;

            // If the callback returns true, skip all further processing.
            if (triangleListener->applyObjectToTriangleVector(object, triangleVector)) {
                *halted = true;
                return;
            }
        }

        // Evaluate the left subtree.
        if (aabbTreeNode->leftNode() != NULL) {
            applyToTriangleVectorIntersectionForSubtree(aabbTreeNode->leftNode(),
                halted, triangleVector, triangleListener);
            if (*halted) {
                return;
            }
        }

        // To avoid function call overhead, loop on the right subtree.
        // rather than using recursion.
        if (aabbTreeNode->rightNode() != NULL) {
            aabbTreeNode = aabbTreeNode->rightNode();
        } else {
            break;
        }
    }
}

template<typename OBJECT>
void
AabbTree<OBJECT>::applyToTetrahedronIntersectionForSubtree(
    AabbTreeNode<OBJECT> *aabbTreeNode, bool *halted, 
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Vector3f &v3,
    TetrahedronListener *tetrahedronListener) const
{
    if (*halted) {
        return;
    }

    while (true) {
        ++mBoundingBoxTests;
        ++mCurrentQueryBoundingBoxTests;

        // If the tetrahedron doesn't intersect
        // the node's bounding box, skip this subtree.
        if (!BoundingBox3fIntersectsTetrahedron(aabbTreeNode->boundingBox(), 
                v0, v1, v2, v2)) {
            return;
        }

        // Evaluate the callback on all of the objects in this node.
        for (typename AabbTreeNode<OBJECT>::ObjectVectorIterator iterator
                 = aabbTreeNode->objectBegin();
             iterator != aabbTreeNode->objectEnd(); ++iterator) {
            OBJECT &object = *iterator;

            ++mObjectTests;
            ++mCurrentQueryObjectTests;

            // If the callback returns true, skip all further processing.
            if (tetrahedronListener->applyObjectToTetrahedron(object, v0, v1, v2, v3)) {
                *halted = true;
                return;
            }
        }

        // Evaluate the left subtree.
        if (aabbTreeNode->leftNode() != NULL) {
            applyToTetrahedronIntersectionForSubtree(aabbTreeNode->leftNode(),
                halted, v0, v1, v2, v3, tetrahedronListener);
            if (*halted) {
                return;
            }
        }

        // To avoid function call overhead, loop on the right subtree.
        // rather than using recursion.
        if (aabbTreeNode->rightNode() != NULL) {
            aabbTreeNode = aabbTreeNode->rightNode();
        } else {
            break;
        }
    }
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::occludesRaySegmentForSubtree(
    AabbTreeNode<OBJECT> *aabbTreeNode, bool *halted, 
    const Vector3f &origin, const Vector3f &endpoint,
    const RaySegmentOcclusionListener *raySegmentOcclusionListener) const
{
    if (*halted) {
        return true;
    }

    while (true) {
        ++mBoundingBoxTests;
        ++mCurrentQueryBoundingBoxTests;

        // If the ray segment does not intersect the bounding
        // box of this node, don't bother testing the subtree
        // any further.
        if (!BoundingBox3fIntersectsRaySegment(aabbTreeNode->boundingBox(),
                origin, endpoint)) {
            return false;
        }

        // Evaluate the callback on all of the objects in this node.
        for (typename AabbTreeNode<OBJECT>::ObjectVectorIterator iterator
                 = aabbTreeNode->objectBegin();
             iterator != aabbTreeNode->objectEnd(); ++iterator) {
            OBJECT &object = *iterator;

            ++mObjectTests;
            ++mCurrentQueryObjectTests;
            
            // If the callback returns true, we've hit something,
            // and there's no need to perform further tests.
            if (raySegmentOcclusionListener->objectOccludesRaySegment(object, 
                    origin, endpoint)) {
                *halted = true;
                return true;
            }
        }
        
        // Evaluate the left subtree.
        if (aabbTreeNode->leftNode() != NULL) {
            if (occludesRaySegmentForSubtree(
                    aabbTreeNode->leftNode(), halted, origin, endpoint, 
                    raySegmentOcclusionListener)) {
                return true;
            }
            if (*halted) {
                return true;
            }
        }

        // To avoid function call overhead, loop on the right subtree.
        // rather than using recursion.
        if (aabbTreeNode->rightNode() != NULL) {
            aabbTreeNode = aabbTreeNode->rightNode();
        } else {
            break;
        }
    }
    
    return false;
}

template<typename OBJECT>
bool 
AabbTree<OBJECT>::intersectsRaySegmentForSubtree(
    AabbTreeNode<OBJECT> *aabbTreeNode,
    const Vector3f &origin, const Vector3f &endpoint, 
    const RaySegmentIntersectionListener *raySegmentIntersectionListener,
    float *t, OBJECT **intersectedObject) const
{
    bool result = false;

    while (true) {
        ++mBoundingBoxTests;
        ++mCurrentQueryBoundingBoxTests;

        // If the ray segment does not intersect the bounding
        // box of this node, don't bother testing the subtree
        // any further.
        float bboxT = 0.0;
        if (!BoundingBox3fIntersectsRaySegment(aabbTreeNode->boundingBox(),
                origin, endpoint, &bboxT)) {
            return result;
        }

        // If the value of 't' where the ray intersects the bounding box
        // is farther away than smallest value of 't' we've encountered
        // so far, so it's not possible that any of the objects
        // within it are closer. Consequently, there's no reason to test
        // against the objects within the bounding box.
        if (bboxT >= *t) {
            return result;
        }

        // Evaluate the callback on all of the objects in this node.
        for (typename AabbTreeNode<OBJECT>::ObjectVectorIterator iterator
                 = aabbTreeNode->objectBegin();
             iterator != aabbTreeNode->objectEnd(); ++iterator) {
            OBJECT &object = *iterator;

            ++mObjectTests;
            ++mCurrentQueryObjectTests;

            if (raySegmentIntersectionListener->objectIntersectsRaySegment(object, 
                    origin, endpoint, t)) {
                result = true;
                *intersectedObject = &object;
            }
        }
        
        // Evaluate the left subtree.
        if (aabbTreeNode->leftNode() != NULL) {
            if (intersectsRaySegmentForSubtree(
                    aabbTreeNode->leftNode(), origin, endpoint, 
                    raySegmentIntersectionListener, t, intersectedObject)) {
                result = true;
            }
        }

        // To avoid function call overhead, loop on the right subtree.
        // rather than using recursion.
        if (aabbTreeNode->rightNode() != NULL) {
            aabbTreeNode = aabbTreeNode->rightNode();
        } else {
            break;
        }
    }
    
    return result;
}

template<typename OBJECT>
void
AabbTree<OBJECT>::updateUsageDataFromCurrentQuery() const
{
    if (mCurrentQueryBoundingBoxTests < mMinBoundingBoxTestsPerQuery) {
        mMinBoundingBoxTestsPerQuery = mCurrentQueryBoundingBoxTests;
    }

    if (mCurrentQueryBoundingBoxTests > mMaxBoundingBoxTestsPerQuery) {
        mMaxBoundingBoxTestsPerQuery = mCurrentQueryBoundingBoxTests;
    }
    
    if (mCurrentQueryObjectTests < mMinObjectTestsPerQuery) {
        mMinObjectTestsPerQuery = mCurrentQueryObjectTests;
    }

    if (mCurrentQueryObjectTests > mMaxObjectTestsPerQuery) {
        mMaxObjectTestsPerQuery = mCurrentQueryObjectTests;
    }
}

} // namespace cgmath

#endif // CGMATH__AABB_TREE__INCLUDED
