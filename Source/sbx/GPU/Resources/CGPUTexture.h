/*
 * @file    CGPUTexture.h
 * @author  David Gallardo Moreno
 */

#ifndef _GPU_TEXTURE_H
#define _GPU_TEXTURE_H

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>
#include <sbx/TextureCore/TTextureDesc.h>

namespace sbx
{    
    class CGPUTexture : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CGPUTexture)
    public:
                                CGPUTexture     (TTextureDesc const & aDesc, 
                                                 TGPUCommandCallback const & aCallback = nullptr, 
                                                 TGPUCommandCallback const & aDestroyCallback = nullptr);

                               ~CGPUTexture     ();
        void                    CopyToBuffer    (CRawBuffer::Ref aRawBuffer, TGPUCommandCallback const & aCallback = nullptr);
        void                    CopyFromBuffer  (CRawBuffer::Ref const aRawBuffer, TGPUCommandCallback const & aCallback = nullptr);
        TTextureDesc const &    GetDesc         () const { return mDescRef; }
        TGPUResourceRef         GetRes          () const { return mResRef; }

                                
    private:
        TTextureDesc const      mDescRef;
        TGPUResourceRef         mResRef;
        TGPUCommandCallback     mDestroyCallback;
    };
    
} // sbx::gfx

#endif // _GPU_TEXTURE_H
