
#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

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

namespace EStrokeOp
{
    enum Type
    {
        OpAdd = 0,
        OpSubstract = 1,
        OpIntersect = 2,
        OpReplace = 3,
        
        OpsMaskAll = 0x3,
        OpsShift = 0,
    };
}

namespace EPrimitive
{
    enum Type
    {
        PrEllipsoid,
        PrBox,
        PrTorus,
        PrCapsule,

        PrCount,
    };
}

// Base stroke to be added to the gpu
struct stroke_t
{
    glm::vec4 posb;
    glm::vec4 quat;
    glm::vec4 param0;
    glm::vec4 param1;
    glm::ivec4 id;
};

// Extra stroke data to be used by the client
struct TStrokeInfo : public stroke_t
{
    enum
    {
        MAX_NAME_SIZE = 250,
    };

    TStrokeInfo(const TStrokeInfo& aOther)
        : stroke_t(aOther)
        , mEulerAngles(aOther.mEulerAngles)
    {
        ::memcpy(mName, aOther.mName, MAX_NAME_SIZE);
        UpdateRotation();
    }

    TStrokeInfo(const stroke_t& aBaseStroke, glm::vec3 aEulerAngles, const char* aName)
        : stroke_t(aBaseStroke)
        , mEulerAngles(aEulerAngles)
    {
        ::snprintf(mName, MAX_NAME_SIZE, "%s", aName);
        mName[MAX_NAME_SIZE - 1] = 0;
        UpdateRotation();
    }

    void UpdateRotation();

    glm::vec3 mEulerAngles {0,0,0};

    char mName[MAX_NAME_SIZE];
};

class CScene
{
public:
    CScene();
    ~CScene();
    
    bool IsDirty() const { return mDirty; }
    void SetDirty() { mDirty = true; }
    void CleanDirtyFlag() { mDirty = false; }

    uint32_t AddNewStorke(uint32_t aBaseStrokeIndex = UINT32_MAX);

    std::vector< TStrokeInfo > mStorkesArray;
    CCamera mCamera;

private:
    bool mDirty;
    uint32_t mNextStrokeId;

};

