/*
 * @file    Texture.h
 * @author  David Gallardo Moreno
 */


#ifndef __SBX_TEXTURE_H__
#define __SBX_TEXTURE_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/TextureCore/Color.h>
#include <sbx/DataStructures/CRawBuffer.h>

namespace sbx
{
    namespace ETextureFormat
    {
        enum Type
	    {
		    R8,
		    RGBA8,
            R32F,
		    RGBA32F
	    };
    } // EImageFormat

    enum class EGammaSpace
    {
	    Linear,
	    Pow22, //simplified sRGB conversion
	    sRGB,
    };

    struct TTexture : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TTexture)
    public:
        CRawBuffer              mBuffer;
        int32_t                 mWidth;
        int32_t                 mHeight;
        int32_t                 mSlices;
        ETextureFormat::Type    mFormat;
        EGammaSpace             mGammaSpace;
                
                TTexture    ();
                TTexture    (int32_t aWidth, int32_t aHeight, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace = EGammaSpace::Linear);
                TTexture    (int32_t aWidth, int32_t aHeight, int32_t aSlices, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace = EGammaSpace::Linear);
                TTexture    (TTexture& aTexture);
                TTexture    (TTexture&& aTexture);
               ~TTexture    ();

        //TTexture const & operator=(TTexture& aOther);


        void    Init        (int32_t aWidth, int32_t aHeight, int32_t aSlices, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace = EGammaSpace::Linear);

        //TODO: implement full copy to
        //void    CopyTo      (TTexture& aTexture, ETextureFormat::Type aFormat, EGammaSpace aGammaSpace) const;

        size_t  GetSliceSize    () const;
        int32_t GetTexelSize    () const;
        uint8_t GetComponents   () const;

        uint8_t*    AsR8Buffer      () const { return mBuffer.GetByteArray(); };
        TColor8U*   AsRGBA8Buffer   () const { return (TColor8U*)mBuffer.GetByteArray(); };
        float*      AsR32FBuffer    () const { return (float*)mBuffer.GetByteArray(); };
		TColor32F*  AsRGBA32FBuffer () const { return (TColor32F*)mBuffer.GetByteArray(); };
    };
};

#endif //__SBX_TEXTURE_H__