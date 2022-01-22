/*
 * @file    TGPUResourceRef.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

#define GPUResMgr() sbx::GetGPUResourceManager()


namespace sbx
{
    TGPUResourceRef::TGPUResourceRef()
        : mInnerData(nullptr)
    {
    }

    TGPUResourceRef::TGPUResourceRef(void* aInnerData)
        : mInnerData(aInnerData)
    {
        if(mInnerData)
        {
            GPUResMgr()->Retain(mInnerData);
        }
    }

    TGPUResourceRef::TGPUResourceRef(TGPUResourceRef const & aOther)
        : mInnerData(nullptr)
    {
        mInnerData = aOther.mInnerData;   

        if(mInnerData)
        {
            GPUResMgr()->Retain(mInnerData);
        }
    }

    TGPUResourceRef::~TGPUResourceRef()
    {
        if(mInnerData)
        {
            GPUResMgr()->Release(mInnerData);
            mInnerData = 0;
        }
    }

    TGPUResourceRef & TGPUResourceRef::operator=(TGPUResourceRef const & aOther)
    {
        if(mInnerData)
        {
            GPUResMgr()->Release(mInnerData);
        }

        mInnerData = aOther.mInnerData;   

        if(mInnerData)
        {
            GPUResMgr()->Retain(mInnerData);
        }

        return *this;
    }

    void TGPUResourceRef::SetHandle(TGPUHandle aHandle)
    {
        if(mInnerData)
        {
            GPUResMgr()->SetResHandle(mInnerData, aHandle);
        }
    }

    TGPUHandle TGPUResourceRef::GetHandle()
    {
        return (mInnerData) ? GPUResMgr()->GetResHandle(mInnerData) : TGPUHandle(0);
    }
}
