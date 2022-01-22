/*
 * @file    CGPUProgram.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CGPUProgram.h"
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

namespace
{
    UID(ShaderProgram);
}

namespace sbx
{ 
    CGPUProgram::CGPUProgram(TGPUProgramDesc::Ref const & aDesc, 
                             TGPUCommandCallback const & aCallback, 
                             TGPUCommandCallback const & aDestroyCallback)
        : mDesc(aDesc)
        , mDestroyCallback(aDestroyCallback)
    {
        mResRef = GetGPUResourceManager()->CreateResource(0, uid(ShaderProgram));
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCreateTextureCmd = lMgr->AllocCreateProgramCmd(mResRef, mDesc);
        lCreateTextureCmd->mCallback = [=]()
        {
            if(aCallback)
            {
                aCallback();
            }

            mDesc = nullptr;
        };
        lMgr->QueueCommand(lCreateTextureCmd);
    }

    CGPUProgram::~CGPUProgram()
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lDestroyTextureCmd = lMgr->AllocDestroyProgramCmd(mResRef);
        lDestroyTextureCmd->mCallback = mDestroyCallback;
        lMgr->QueueCommand(lDestroyTextureCmd);
    }

    void CGPUProgram::Swap(CGPUProgram & aOther)
    {
        sbx::Swap(mDesc,            aOther.mDesc);
        sbx::Swap(mDestroyCallback, aOther.mDestroyCallback);
        sbx::Swap(mResRef,          aOther.mResRef);
    }

    /*
    void CGPUProgram::Execute(TGPUProgramData const & aData)
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCreateTextureCmd = lMgr->AllocExecuteComputeShaderCmd(mResRef, aData);
        lMgr->QueueCommand(lCreateTextureCmd);
    }*/
}
