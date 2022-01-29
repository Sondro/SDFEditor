
#pragma once

#include <cstdint>

namespace ETexTarget
{
    enum Type
    {
        TEXTURE_2D,
        TEXTURE_3D
    };
};

namespace ETexFormat
{
    enum Type
    {
        R8,
        RGBA8,
        RGBA16F,
        RGBA32F,
    };
};

namespace ETexFilter
{
    enum Type
    {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR
    };
};

namespace ETexWrap
{
    enum Type
    {
        REPEAT,
        CLAMP_TO_EDGE
    };
};

struct TGPUTextureConfig
{
    ETexTarget::Type mTarget{ ETexTarget::TEXTURE_2D };
    uint32_t mExtentX{ 256 };
    uint32_t mExtentY{ 256 };
    uint32_t mSlices{ 1 };
    ETexFormat::Type mFormat{ ETexFormat::RGBA8 };
    ETexFilter::Type mMinFilter{ ETexFilter::LINEAR };
    ETexFilter::Type mMagFilter{ ETexFilter::LINEAR };
    ETexWrap::Type mWrapS{ ETexWrap::REPEAT };
    ETexWrap::Type mWrapT{ ETexWrap::REPEAT };
    ETexWrap::Type mWrapR{ ETexWrap::REPEAT };
    uint32_t mMips{ 1 };
};

class CGPUTexture
{
public:
    CGPUTexture(TGPUTextureConfig const & aConfig);
    ~CGPUTexture();
    void Bind(uint32_t aLocation);
private:
    TGPUTextureConfig mConfig;

    uint32_t mTextureHandler;
    uint32_t mTarget;
};