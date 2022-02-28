
#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

// Stroke operations
namespace EStrokeOp
{
    enum Type
    {
        OpAdd = 0,
        OpSubtract = 1,
        OpIntersect = 2,
        OpReplace = 3,

        OpsMaskAll = 0x3,
        OpsShift = 0,
    };
}

// Stroke primitives
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

// Base stroke data to be send to the gpu
struct stroke_t
{
    glm::vec4 posb{ 0, 0, 0, 0 };     // position.xyz, blend.w
    glm::vec4 quat{ 0, 0, 0, 0 };     // rotation quaternion
    glm::vec4 param0{ 0.35f, 0.35f, 0.35f, 0 };   // scale.xzy, unused.w
    glm::vec4 param1{ 0, 0, 0, 0 };   // unused.xyz
    glm::ivec4 id{ 0, 0, 0, 0 };      // primitive.x, operation_bitfield.y, unused.zw
};

// Extra stroke data to be used by the client
struct TStrokeInfo : public stroke_t
{
    enum
    {
        MAX_NAME_SIZE = 250,
    };

    TStrokeInfo() { 
        ::strcpy(mName, "Unnamed");
    }

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

    glm::vec3 GetScale() const
    {
        // TODO: compose scale based on primitive
        return glm::vec3(param0.x, param0.y, param0.z);
    }

    void SetScale(glm::vec3 const& aScale)
    {
        // TODO: Set Scale based on primitive
        param0.x = aScale.x;
        param0.y = aScale.y;
        param0.z = aScale.z;
    }

    glm::vec3 mEulerAngles{ 0,0,0 };

    char mName[MAX_NAME_SIZE];
};
