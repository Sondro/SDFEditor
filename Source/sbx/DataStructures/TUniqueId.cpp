/*
 * @file    TUniqueId.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "TUniqueId.h"
#include <sbx/Library/FObjectPool.h>
#include <sbx/Library/FVector.h>
#include <mutex>
#include <string>

namespace
{
    const size_t kUIDPoolPageElements = 256;
}

namespace sbx
{
    

    TUniqueIdData::TUniqueIdData(const char* aStr, uint32_t aLength, uint32_t aCRC32) : mString(0), mLength(aLength), mCRC32(aCRC32), mRefCount(0)
    {
        mString = static_cast<char*>(SBX_MALLOC(mLength + 1));
        ::memcpy(mString, aStr, mLength);
        mString[mLength] = 0;
    };

    TUniqueIdData::~TUniqueIdData()
    {
        SBX_LOG("Deleted TUniqueIdData %s %u", mString, mCRC32);
        SBX_FREE(mString);
        mString = 0;
    }

    struct CUniqueIdRegistry::TData
    {
        FObjectPool< TUniqueIdData >  mPool;
        FVector< TUniqueIdData* > mRegistry;
    };

    CUniqueIdRegistry::CUniqueIdRegistry()
    {
        mData = SBX_NEW(TData);
        mData->mPool.Init(kUIDPoolPageElements);
    }

    CUniqueIdRegistry::~CUniqueIdRegistry()
    {
        for(uint32_t i = 0; i < mData->mRegistry.size(); ++i)
        {
            mData->mPool.FreeObject(mData->mRegistry[i]);
        }

        mData->mRegistry.clear();
        SBX_DELETE(mData);
    }

    TUniqueIdData* CUniqueIdRegistry::RegisterUniqueId(const char * aNameStr, uint32_t aLength, uint32_t aCRC32)
    {
        static std::mutex sUniqueRegistryMutex; 
        std::lock_guard< std::mutex > lScopedMutex(sUniqueRegistryMutex);

        TUniqueIdData* lRegisteredId = 0;
        for(size_t i = 0; i < mData->mRegistry.size() && !lRegisteredId; ++i)
        {
            if( (mData->mRegistry.at(i)->mCRC32 == aCRC32) &&
                (mData->mRegistry.at(i)->mLength == aLength) )
            {
                lRegisteredId = mData->mRegistry.at(i);
            }
        }

        if(0 == lRegisteredId)
        {
            lRegisteredId = mData->mPool.AllocOjbect(aNameStr, aLength, aCRC32);
            mData->mRegistry.push_back( lRegisteredId );
        }

        lRegisteredId->mRefCount++;

        return lRegisteredId;
    }

    //----------------------------------------------------------------
    // AddReference
    //----------------------------------------------------------------
    void CUniqueIdRegistry::AddReference(TUniqueIdData* aRegisteredId)
    {
        aRegisteredId->mRefCount++;
    }

    //----------------------------------------------------------------
    // RemoveReference
    //----------------------------------------------------------------
    bool CUniqueIdRegistry::RemoveReference(TUniqueIdData* aRegisteredId)
    {
        if(nullptr != aRegisteredId)
        {
            aRegisteredId->mRefCount--;
            if(0 == aRegisteredId->mRefCount)
            {
                auto lIterator = std::find(mData->mRegistry.begin(), mData->mRegistry.end(), aRegisteredId);
                std::swap(*lIterator, mData->mRegistry.back());
                mData->mPool.FreeObject(mData->mRegistry.back());
                mData->mRegistry.pop_back();
                return (mData->mRegistry.size32() == 0);
            }
        }

        return false;
    }
} // sbx