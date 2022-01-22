/*
 * @file    CGPURenderTarget.cpp
 * @author  David Gallardo Moreno
 */


#include <sbx/Common.h>
#include "CGPURenderTarget.h"
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

namespace
{
    UID(RenderTarget);
}

namespace sbx
{
    CGPURenderTarget::CGPURenderTarget( TGPURenderTargetDesc::Ref const & aDesc,
                                        TGPUCommandCallback const & aCallback, 
                                        TGPUCommandCallback const & aDestroyCallback)
        : mDesc(aDesc)
        , mDestroyCallback(aDestroyCallback)
    {
        mResRef = GetGPUResourceManager()->CreateResource(0, uid(RenderTarget));
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCmd = lMgr->AllocCreateRenderTargetCmd(mResRef, mDesc);
        lCmd->mCallback = aCallback;
        lMgr->QueueCommand(lCmd);
    }

    CGPURenderTarget::~CGPURenderTarget ()
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCmd = lMgr->AllocDestroyRenderTargetCmd(mResRef);
        lCmd->mCallback = mDestroyCallback;
        lMgr->QueueCommand(lCmd);
    }


    void CGPURenderTarget::Clear (TColor32F const & aColor, TGPUCommandCallback const & aCallback)
    {
        if(mDesc)
        {
            CGPUCommandManager* lMgr = GetGPUCommandManager();
            TGPUCommand* lCmd = lMgr->AllocClearRenderTargetCmd(mResRef, uint32_t(mDesc->mColorAttachments.size()), aColor);
            lCmd->mCallback = aCallback;
            lMgr->QueueCommand(lCmd);
        }
    }
}