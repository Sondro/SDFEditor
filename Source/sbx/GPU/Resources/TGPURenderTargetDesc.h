/*
 * @file    TGPURenderTargetDesc.H
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_RENDER_TARGET_DESC_H__
#define __SBX_GPU_RENDER_TARGET_DESC_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/Resources/CGPUTexture.h>

namespace sbx
{
    struct TGPURenderTargetDesc : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TGPURenderTargetDesc)
        SBX_DELETE_DEFAULT_CTORS(TGPURenderTargetDesc)
    public: 
        enum{ MAX_COLOR_ATTACHMENTS = 4 };
        uint32_t mViewportWidth = 0;
        uint32_t mViewportHeight = 0;
        FVector< CGPUTexture::Ref > mColorAttachments; //[MAX_COLOR_ATTACHMENTS];
        CGPUTexture::Ref mDepthAttachment;

        TGPURenderTargetDesc(uint32_t aViewportWidth, uint32_t aViewportHeight)
            : mViewportWidth(aViewportWidth)
            , mViewportHeight(aViewportHeight)
        {;}
    };
} // sbx

#endif //__SBX_GPU_RENDER_TARGET_DESC_H__