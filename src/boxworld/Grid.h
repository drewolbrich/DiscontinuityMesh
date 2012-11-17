// Copyright 2010 Retroactive Fiasco.

#ifndef BOXWORLD__GRID__INCLUDED
#define BOXWORLD__GRID__INCLUDED

#include <vector>

// Grid
//
// 

class Grid
{
public:
    Grid();
    ~Grid();

    void setWidth(int width);
    int width() const;
    void setHeight(int height);
    int height() const;
    void setDepth(int depth);
    int depth() const;

    void initialize();

    void set(int x, int y, int z, int value);
    int get(int x, int y, int z) const;

    int neighbors(int x, int y, int z) const;

private:
    int mWidth;
    int mHeight;
    int mDepth;

    std::vector<int> mData;
};

#endif // BOXWORLD__GRID__INCLUDED
