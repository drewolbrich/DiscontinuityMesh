// Copyright 2008 Drew Olbrich

#ifndef CGMATH__AABB_TREE_NODE__INCLUDED
#define CGMATH__AABB_TREE_NODE__INCLUDED

#include <vector>

#include "BoundingBox3f.h"

namespace cgmath {

// AabbTreeNode
//
// A node in an axis-aligned bounding box tree.

template<typename OBJECT>
class AabbTreeNode
{
public:
    AabbTreeNode();
    ~AabbTreeNode();

    // Set the bounding box of the node and all of it's children.
    void setBoundingBox(const BoundingBox3f &boundingBox);
    const BoundingBox3f &boundingBox() const;

    // Add an object to the node.
    void addObject(const OBJECT &object);

    // Iterators for the vector of objects in the node.
    typedef std::vector<OBJECT> ObjectVector;
    typedef typename ObjectVector::iterator ObjectVectorIterator;
    ObjectVectorIterator objectBegin();
    ObjectVectorIterator objectEnd();

    // Set the left child node.
    void setLeftNode(AabbTreeNode<OBJECT> *leftNode);
    AabbTreeNode<OBJECT> *leftNode() const;

    // Set the right child node.
    void setRightNode(AabbTreeNode<OBJECT> *rightNode);
    AabbTreeNode<OBJECT> *rightNode() const;

    // Returns true if the node is a leaf (it has no children).
    bool isLeaf() const;
    
private:
    BoundingBox3f mBoundingBox;
    ObjectVector mObjectVector;
    AabbTreeNode<OBJECT> *mLeftNode;
    AabbTreeNode<OBJECT> *mRightNode;
};

template<typename OBJECT>
AabbTreeNode<OBJECT>::AabbTreeNode()
    : mBoundingBox(),
      mObjectVector(),
      mLeftNode(NULL),
      mRightNode(NULL)
{
}

template<typename OBJECT>
AabbTreeNode<OBJECT>::~AabbTreeNode()
{
#ifdef DEBUG
    mLeftNode = NULL;
    mRightNode = NULL;
#endif
}

template<typename OBJECT>
void 
AabbTreeNode<OBJECT>::setBoundingBox(const BoundingBox3f &boundingBox)
{
    mBoundingBox = boundingBox;
}

template<typename OBJECT>
const BoundingBox3f &
AabbTreeNode<OBJECT>::boundingBox() const
{
    return mBoundingBox;
}

template<typename OBJECT>
void 
AabbTreeNode<OBJECT>::addObject(const OBJECT &object)
{
    mObjectVector.push_back(object);
}

template<typename OBJECT>
typename AabbTreeNode<OBJECT>::ObjectVectorIterator 
AabbTreeNode<OBJECT>::objectBegin()
{
    return mObjectVector.begin();
}

template<typename OBJECT>
typename AabbTreeNode<OBJECT>::ObjectVectorIterator 
AabbTreeNode<OBJECT>::objectEnd()
{
    return mObjectVector.end();
}

template<typename OBJECT>
void 
AabbTreeNode<OBJECT>::setLeftNode(AabbTreeNode *leftNode)
{
    mLeftNode = leftNode;
}

template<typename OBJECT>
AabbTreeNode<OBJECT> *
AabbTreeNode<OBJECT>::leftNode() const
{
    return mLeftNode;
}

template<typename OBJECT>
void 
AabbTreeNode<OBJECT>::setRightNode(AabbTreeNode *rightNode)
{
    mRightNode = rightNode;
}

template<typename OBJECT>
AabbTreeNode<OBJECT> *
AabbTreeNode<OBJECT>::rightNode() const
{
    return mRightNode;
}

template<typename OBJECT>
bool 
AabbTreeNode<OBJECT>::isLeaf() const
{
    return mLeftNode == NULL && mRightNode == NULL;
}

} // namespace cgmath

#endif // CGMATH__AABB_TREE_NODE__INCLUDED
