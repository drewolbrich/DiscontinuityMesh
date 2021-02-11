// Copyright 2010 Drew Olbrich

#include "Grid.h"

#include <cassert>

Grid::Grid()
    : mWidth(0),
      mHeight(0),
      mDepth(0),
      mData()
{
}

Grid::~Grid()
{
}

void
Grid::setWidth(int width)
{
    mWidth = width;
}

int
Grid::width() const
{
    return mWidth;
}

void
Grid::setHeight(int height)
{
    mHeight = height;
}

int
Grid::height() const
{
    return mHeight;
}

void
Grid::setDepth(int depth)
{
    mDepth = depth;
}

int
Grid::depth() const
{
    return mDepth;
}

void
Grid::initialize()
{
    mData.resize(mWidth*mHeight*mDepth);
}

void
Grid::set(int x, int y, int z, int value)
{
    assert(x >= 0);
    assert(x < mWidth);
    assert(y >= 0);
    assert(y < mHeight);
    assert(z >= 0);
    assert(z < mDepth);

    int index = x*mHeight*mDepth + y*mDepth + z;
    assert(index >= 0);
    assert(index < mWidth*mHeight*mDepth);

    mData[index] = value;
}

int
Grid::get(int x, int y, int z) const
{
    if (x < 0
        || x >= mWidth
        || y < 0
        || y >= mHeight
        || z < 0
        || z >= mDepth) {
        return 0;
    }

    int index = x*mHeight*mDepth + y*mDepth + z;
    assert(index >= 0);
    assert(index < mWidth*mHeight*mDepth);

    return mData[index];
}

int
Grid::neighbors(int x, int y, int z) const
{
    return (get(x - 1, y, z) > 0)
        + (get(x + 1, y, z) > 0)
        + (get(x, y - 1, z) > 0)
        + (get(x, y + 1, z) > 0)
        + (get(x, y, z - 1) > 0)
        + (get(x, y, z + 1) > 0);
}
