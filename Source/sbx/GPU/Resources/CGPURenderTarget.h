/*
 * @file    CGPURenderTarget.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_RENDER_TARGET_H__
#define __SBX_GPU_RENDER_TARGET_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>
#include <sbx/GPU/Resources/TGPURenderTargetDesc.h>
#include <sbx/TextureCore/Color.h>

namespace sbx
{

    class CGPURenderTarget : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CGPURenderTarget)
    public: 
                                    CGPURenderTarget    (TGPURenderTargetDesc::Ref const & aDesc,
                                                         TGPUCommandCallback const & aCallback = nullptr, 
                                                         TGPUCommandCallback const & aDestroyCallback = nullptr);
                                   ~CGPURenderTarget    ();
        TGPURenderTargetDesc::Ref   GetDesc             () const { return mDesc; }
        TGPUResourceRef             GetRes              () const { return mResRef; }
        void                        Clear               (TColor32F const & aColor, TGPUCommandCallback const & aCallback = nullptr);

    private:
        TGPURenderTargetDesc::Ref       mDesc;
        TGPUResourceRef                 mResRef;
        TGPUCommandCallback             mDestroyCallback;
    };

} // sbx::gfx

#endif // __SBX_GPU_RENDER_TARGET_H__
