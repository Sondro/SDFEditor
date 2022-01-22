/*
 * @file    Texture.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "Texture.h"

namespace sbx
{
    namespace
    {    
        static const uint8_t sComponents[] =
        {
            1,
            4,
            1,
            4
        };

        static const int32_t sFormatSizes[] =
        {
            sizeof(uint8_t),
            sizeof(TColor8U),
            sizeof(float),
            sizeof(TColor32F)
        };
    }

    TTexture::TTexture()
        : mWidth (0)
        , mHeight(0)
        , mSlices(0)
        , mFormat(ETextureFormat::R8)
        , mGammaSpace(EGammaSpace::Linear)
    {

    }

    TTexture::TTexture(int32_t aWidth, int32_t aHeight, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace)
    {
        Init(aWidth, aHeight, 1, aFormat, aGammaSpace);
    }

    TTexture::TTexture(int32_t aWidth, int32_t aHeight, int32_t aSlices, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace)
    {
        Init(aWidth, aHeight, aSlices, aFormat, aGammaSpace);
    }

    TTexture::TTexture(TTexture& aTexture) 
        : mBuffer(aTexture.mBuffer)
        , mWidth(aTexture.mWidth)
        , mHeight(aTexture.mHeight)
        , mSlices(aTexture.mSlices)
        , mFormat(aTexture.mFormat)
        , mGammaSpace(aTexture.mGammaSpace)

    {
    }

    TTexture::TTexture(TTexture&& aTexture) 
        : mBuffer(std::move(aTexture.mBuffer))
        , mWidth(aTexture.mWidth)
        , mHeight(aTexture.mHeight)
        , mSlices(aTexture.mSlices)
        , mFormat(aTexture.mFormat)
        , mGammaSpace(aTexture.mGammaSpace)

    {
        aTexture.mWidth = 0;
        aTexture.mHeight = 0;
        aTexture.mSlices = 0;
        aTexture.mFormat = ETextureFormat::R8;
        aTexture.mGammaSpace = EGammaSpace::Linear;
    }

    TTexture::~TTexture()
    {
    }

    void TTexture::Init (int32_t aWidth, int32_t aHeight, int32_t aSlices, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace)
    {
        mWidth = aWidth;
        mHeight = aHeight;
        mSlices = aSlices;
        mFormat = aFormat;
        mGammaSpace = aGammaSpace;
        mBuffer.Init(mWidth * mHeight * mSlices * sFormatSizes[mFormat]);
    }



    /*void TTexture::CopyTo(TTexture& aTexture, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace) const
    {
        //TODO: copy it properly
        //aTexture.Init(mWidth, mHeight, mSlices, aFormat, aGammaSpace);

        aTexture = *this;
    }*/

    size_t TTexture::GetSliceSize() const
    {
        return mWidth * mHeight * sFormatSizes[mFormat];
    }

    int32_t TTexture::GetTexelSize() const
    {
        return sFormatSizes[mFormat];
    }

    uint8_t TTexture::GetComponents() const
    {
        return sComponents[mFormat];
    }
} // sbx