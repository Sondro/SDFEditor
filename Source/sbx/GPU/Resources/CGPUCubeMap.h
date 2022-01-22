/*
 * @file    CGPUCubeMap.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_TEXTURE_H__
#define __SBX_GPU_TEXTURE_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/TextureCore/Texture.h>

namespace sbx
{    
    class CGPUCubeMap : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CGPUCubeMap)
    public:
                                CGPUCubeMap     (TGPUCommandCallback const & aCallback = nullptr, 
                                                 TGPUCommandCallback const & aDestroyCallback = nullptr);
                               ~CGPUCubeMap     ();
        void                    SetupMip        (TTexture::Ref aTexture, uint32_t aMipLevel, TGPUCommandCallback const & aCallback = nullptr);
        TGPUResourceRef         GetRes          () const { return mResRef; }

    private:
        TGPUResourceRef         mResRef;
        TGPUCommandCallback     mDestroyCallback;
    };

} // sbx::gfx

#endif // __SBX_GPU_TEXTURE_H__
