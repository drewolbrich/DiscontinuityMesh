// Copyright 2009 Drew Olbrich

#include "Types.h"

#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

namespace mesh {

bool 
operator<(const VertexPtr &lhs, const VertexPtr &rhs)
{
    return &*lhs < &*rhs;
}

bool 
operator<(const EdgePtr &lhs, const EdgePtr &rhs)
{
    return &*lhs < &*rhs;
}

bool 
operator<(const FacePtr &lhs, const FacePtr &rhs)
{
    return &*lhs < &*rhs;
}

bool 
operator<(const ConstVertexPtr &lhs, const ConstVertexPtr &rhs)
{
    return &*lhs < &*rhs;
}

bool 
operator<(const ConstEdgePtr &lhs, const ConstEdgePtr &rhs)
{
    return &*lhs < &*rhs;
}

bool 
operator<(const ConstFacePtr &lhs, const ConstFacePtr &rhs)
{
    return &*lhs < &*rhs;
}

std::ostream &
operator<<(std::ostream &ostr, const VertexPtr &rhs)
{
    return ostr << &*rhs;
}

std::ostream &
operator<<(std::ostream &ostr, const EdgePtr &rhs)
{
    return ostr << &*rhs;
}

std::ostream &
operator<<(std::ostream &ostr, const FacePtr &rhs)
{
    return ostr << &*rhs;
}

} // namespace mesh
