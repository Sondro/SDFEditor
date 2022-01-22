
#pragma once

#include <cstdint>
#include <list>
#include <vector>
#include <memory>

struct TShape
{
    float px, py, pz, pw;
    float sx, sy, sz, sw;
};

class CScene
{
public:
    auto begin() const
    {
        return mShapes.begin();
    }
    auto end() const
    {
        return mShapes.end();
    }
private:
    std::vector< std::shared_ptr<TShape> > mShapes;
};

