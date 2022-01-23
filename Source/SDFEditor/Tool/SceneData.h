
#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "glm/glm.hpp"

/*
struct SShape
{
    vec4  attr0; //xyz: pos, w: 
    vec4  attr1; //xyz: scale, w: 
    vec4  attr2; //x: blend, y: hole, z: round, w: 
    vec4  quat;  //rotation quaternion
    ivec4 id;    //x: primitive, y: material, z: flags (SUBSTRACTIVE | MASK | MIRRORX | MIRRORY | MIRRORZ | REPEAT_CIRCLE), w: repeats
};
*/

struct stroke_t
{
    glm::vec4 param0;
    glm::vec4 param1;
};

class CScene
{
public:
    CScene();
    ~CScene();
    
    bool IsDirty() const { return mDirty; }
    void SetDirty() { mDirty = true; }
    void CleanDirtyFlag() { mDirty = false; }

    std::vector< stroke_t > mStorkesArray;

private:
    bool mDirty;
};

