/*
 * @file    CGPUSharedBlock.cpp
 * @author  David Gallardo Moreno
 */


#include <sbx/Common.h>
#include "CGPUSharedBlock.h"
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

namespace
{
    UID(UniformBuffer)
}

namespace sbx
{
    CGPUSharedBlock::CGPUSharedBlock( CRawBuffer::Ref const & aBuffer,
                                      TGPUCommandCallback const & aCallback, 
                                      TGPUCommandCallback const & aDestroyCallback)
        : mDestroyCallback(aDestroyCallback)
        , mSize(0)
    {
        mBlockRes = GetGPUResourceManager()->CreateResource(0, uid(UniformBuffer));
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        mSize = aBuffer->GetSize();
        TGPUCommand* lCmd = lMgr->AllocCreateSharedBlockCmd(mBlockRes, aBuffer);
        lCmd->mCallback = aCallback;
        lMgr->QueueCommand(lCmd);
    }

    CGPUSharedBlock::~CGPUSharedBlock()
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCmd = lMgr->AllocDestroySharedBlockCmd(mBlockRes);
        lCmd->mCallback = mDestroyCallback;
        lMgr->QueueCommand(lCmd);
    }

    void CGPUSharedBlock::Update (CRawBuffer::Ref const & aBuffer, uint32_t aOffset, uint32_t aSize, TGPUCommandCallback const & aCallback)
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCmd = lMgr->AllocUpdateSharedBlockCmd(mBlockRes, aBuffer, aOffset, aSize);
        lCmd->mCallback = aCallback;
        lMgr->QueueCommand(lCmd);
    }
} // sbx
