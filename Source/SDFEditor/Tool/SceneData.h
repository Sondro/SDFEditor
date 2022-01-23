
#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "glm/glm.hpp"

#include "SDFEditor/Tool/Camera.h"

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
    glm::ivec4 id;
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
    CCamera mCamera;

private:
    bool mDirty;

};

