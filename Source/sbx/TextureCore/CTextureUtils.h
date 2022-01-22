/*
 * @file    CTextureUtils.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_TEXTURE_UTILS_H_
#define _SBX_TEXTURE_UTILS_H_

#include <sbx/TextureCore/Texture.h>
#include <sbx/GPU/Resources/CGPUTexture.h>
#include <sbx/FileSystem/File.h>
#include <sbx/FileSystem/FileTypes.h>
#include <sbx/Library/FVector.h>

namespace sbx
{
    class CTextureUtils
    {
    public:
        static bool             IsHDR           (TPath const & aPath);
        static void             LoadFromFile    (TPath const & aPath, TTexture & aTexture);
        static void             SaveToFile      (TPath const & aPath, TTexture const & aTexture, EFileType aFileType, uint32_t aSlice = 0);
        static void             LoadFromFile    (TPath const & aPath, CGPUTexture::Ref & aTexture);

        static bool             StoreTexturePack    (TPath const & aPath, FVector< TTexture > const & aTexturePack);
        static bool             LoadTexturePack     (TPath const & aPath, FVector< TTexture > & aTexturePack);
    };
};

#endif // _SBX_TEXTURE_UTILS_H_