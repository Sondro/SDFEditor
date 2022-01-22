
#pragma once

#include <cstdint>
#include <vector>
#include <memory>

template <typename T>
struct TVec4
{
    union
    {
        struct
        {
            T x, y, z, w;
        };
        T mBuffer[4];
    };
};

using vec4 = TVec4<float>;
using ivec4 = TVec4<int32_t>; 

struct SShape
{
    vec4  attr0; //xyz: pos, w: 
    vec4  attr1; //xyz: scale, w: 
    vec4  attr2; //x: blend, y: hole, z: round, w: 
    vec4  quat;  //rotation quaternion
    ivec4 id;    //x: primitive, y: material, z: flags (SUBSTRACTIVE | MASK | MIRRORX | MIRRORY | MIRRORZ | REPEAT_CIRCLE), w: repeats
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
    std::vector< std::shared_ptr<SShape> > mShapes;
};

