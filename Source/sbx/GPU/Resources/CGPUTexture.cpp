/*
 * @file    CGPUTexture.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CGPUTexture.h"
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

namespace
{
    UID(Texture);
}


namespace sbx
{ 
    CGPUTexture::CGPUTexture(TTextureDesc const & aDesc,
                             TGPUCommandCallback const & aCallback, 
                             TGPUCommandCallback const & aDestroyCallback)
        : mDescRef(aDesc)
        , mResRef(0)
        , mDestroyCallback(aDestroyCallback) 
    {
        mResRef = GetGPUResourceManager()->CreateResource(0, uid(Texture));
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCreateTextureCmd = lMgr->AllocCreateTextureCmd(mResRef, mDescRef);
        lCreateTextureCmd->mCallback = aCallback;
        lMgr->QueueCommand(lCreateTextureCmd);
    }

    CGPUTexture::~CGPUTexture()
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lDestroyTextureCmd = lMgr->AllocDestroyTextureCmd(mResRef, mDescRef);
        lDestroyTextureCmd->mCallback = mDestroyCallback;
        lMgr->QueueCommand(lDestroyTextureCmd);
    }

    void CGPUTexture::CopyToBuffer(CRawBuffer::Ref aRawBuffer, TGPUCommandCallback const & aCallback)
    {
        CGPUCommandManager* lCmdMgr = GetGPUCommandManager();
        TGPUCommand* lCopyTextureToBufferCmd = lCmdMgr->AllocCopyTextureToBufferCmd(mResRef, mDescRef, aRawBuffer);
        lCopyTextureToBufferCmd->mCallback = aCallback;
        lCmdMgr->QueueCommand(lCopyTextureToBufferCmd);
    }

    void CGPUTexture::CopyFromBuffer(CRawBuffer::Ref const aRawBuffer, TGPUCommandCallback const & aCallback)
    {
        CGPUCommandManager* lCmdMgr = GetGPUCommandManager();
        TGPUCommand* lCopyTextureFromBufferCmd = lCmdMgr->AllocCopyTextureFromBufferCmd(mResRef, mDescRef, aRawBuffer);    
        lCopyTextureFromBufferCmd->mCallback = aCallback;
        lCmdMgr->QueueCommand(lCopyTextureFromBufferCmd);
    }
}
