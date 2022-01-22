/*
 * @file    CGPUCubeMap.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CGPUCubeMap.h"
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

namespace
{
    UID(CubeMap);
}

namespace sbx
{ 
    CGPUCubeMap::CGPUCubeMap(TGPUCommandCallback const & aCallback, 
                             TGPUCommandCallback const & aDestroyCallback)
        : mResRef(0)
        , mDestroyCallback(aDestroyCallback) 
    {
        SBX_LOG("[CGPUCubeMap::CGPUCubeMap] CTOR");
        mResRef = GetGPUResourceManager()->CreateResource(0, uid(CubeMap));
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCreateTextureCmd = lMgr->AllocCreateCubeMapTextureCmd(mResRef);
        lCreateTextureCmd->mCallback = aCallback;
        lMgr->QueueCommand(lCreateTextureCmd);
    }

    CGPUCubeMap::~CGPUCubeMap()
    {
        SBX_LOG("[CGPUCubeMap::CGPUCubeMap] DESTRUCTOR");
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lDestroyTextureCmd = lMgr->AllocDestroyTextureCmd(mResRef, TTextureDesc());
        lDestroyTextureCmd->mCallback = mDestroyCallback;
        lMgr->QueueCommand(lDestroyTextureCmd);
    }

    void CGPUCubeMap::SetupMip(TTexture::Ref aTexture, uint32_t aMipLevel, TGPUCommandCallback const & aCallback)
    {
        CGPUCommandManager* lCmdMgr = GetGPUCommandManager();
        TGPUCommand* lSetupCubemapMipCmd = lCmdMgr->AllocSetupCubeMapTextureCmd(mResRef, aTexture, aMipLevel);
        lSetupCubemapMipCmd->mCallback = aCallback;
        lCmdMgr->QueueCommand(lSetupCubemapMipCmd);
    }
}
