/*
 * @file    GPUResourceManager.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CGPUResourceManager.h"

namespace
{
    sbx::CGPUResourceManager* sGPUResourceManager { nullptr };
}

namespace sbx
{
    void InitGPUResourceManager ()
    {
        if(!sGPUResourceManager)
        {
            sGPUResourceManager = SBX_NEW(sbx::CGPUResourceManager);
            sGPUResourceManager->Init();
        }
    }

    void EndGPUResourceManager ()
    {
        if(sGPUResourceManager)
        {
            SBX_DELETE(sGPUResourceManager);
        }
    }

    CGPUResourceManager* GetGPUResourceManager ()
    {
        return sGPUResourceManager;
    }

    struct TGPUResData
    {
        TGPUHandle   mHandle;
        TUniqueId    mTag;
        uintptr_t    mIndex;
        int32_t      mInstances;

        TGPUResData(TGPUHandle          aHandle,
                    TUniqueId const &   aTag,
                    uintptr_t           aIndex)
            : mHandle(aHandle)
            , mTag(aTag)
            , mIndex(aIndex)
            , mInstances(0)
        {;}
    };

    struct CGPUResourceManager::TImpl
    {
        FVector< TGPUResData* > mHandles;
    };

    CGPUResourceManager::CGPUResourceManager()
        : mImpl(SBX_NEW(TImpl))
    {

    }

    CGPUResourceManager::~CGPUResourceManager()
    {

    }

    bool CGPUResourceManager::Init()
    {
        return true;
    }

    void CGPUResourceManager::End()
    {

    }

    TGPUResourceRef CGPUResourceManager::CreateResource (TGPUHandle aHandle, TUniqueId const & aTag)
    {
        TGPUResData* lData = SBX_NEW(TGPUResData, aHandle, aTag, mImpl->mHandles.size());
        mImpl->mHandles.push_back(lData);
        return TGPUResourceRef(lData);
    }

    void CGPUResourceManager::SetResHandle(void* aInnerData, TGPUHandle aHandle)
    {
        TGPUResData* lData = reinterpret_cast< TGPUResData* >(aInnerData);
        lData->mHandle = aHandle;
    }

    TGPUHandle CGPUResourceManager::GetResHandle(void* aInnerData) const
    {
        TGPUResData* lData = reinterpret_cast< TGPUResData* >(aInnerData);
        return lData->mHandle;
    }

    void CGPUResourceManager::Retain(void* aInnerData)
    {
        TGPUResData* lData = reinterpret_cast< TGPUResData* >(aInnerData);
        lData->mInstances++;
    }

    void CGPUResourceManager::Release(void* aInnerData)
    {
        TGPUResData* lData = reinterpret_cast< TGPUResData* >(aInnerData);
        lData->mInstances--;
        if(lData->mInstances <= 0)
        {
            RemoveResource(aInnerData);
        }
    }

    void CGPUResourceManager::RemoveResource (void* aInnerData)
    {
        TGPUResData* lData = reinterpret_cast< TGPUResData* >(aInnerData);
        if(mImpl->mHandles.size() > 1)
        {
            mImpl->mHandles[lData->mIndex] = mImpl->mHandles.back();
            mImpl->mHandles[lData->mIndex]->mIndex = lData->mIndex;
        }
        mImpl->mHandles.pop_back();
        SBX_DELETE(lData);
    }
}