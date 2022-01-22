/*
 * @file    CMemoryProfiler.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CMemoryProfiler.h"

#include <algorithm>

namespace sbx
{
    CMemoryProfiler::CMemoryProfiler()
        : mSortCriteria(EMemAllocSortCriteria::eCurrentInstancesAllocated)
    {
    };

    CMemoryProfiler::~CMemoryProfiler()
    {
        for(size_t i = 0; i < mAllocationsList.size(); ++i)
        {
            delete mAllocationsList[i];
            mAllocationsList[i] = nullptr;
        }

        mAllocationsList.clear();
    }

    void CMemoryProfiler::AddAllocation (uint32_t aTypeId, const char* aTypeIdName, size_t aSize)
    {
        uint32_t lLen;
        uint32_t lCrc32 = sbx::CRC32::str2crc32(aTypeIdName, lLen, false);
        auto lIt = mAllocationsMap.find(lCrc32);

        TAllocInfo* lInfo = nullptr;
        if(lIt != mAllocationsMap.end())
        {
            lInfo = lIt->second;
        }

        if(nullptr == lInfo)
        {
            mAllocationsList.push_back(new TAllocInfo{ aTypeId, aTypeIdName, aSize, 0, 0 });
            lInfo = mAllocationsList.back();
            mAllocationsMap.insert( lIt, std::make_pair(lCrc32, lInfo) );
            
        }
       
        lInfo->mTotalAllocated += 1;
        lInfo->mCurrentAllocated += 1;

        Sort();
    }

    void CMemoryProfiler::RemoveAllocation (uint32_t aTypeId, const char* aTypeIdName)
    {
        uint32_t lLen;
        uint32_t lCrc32 = sbx::CRC32::str2crc32(aTypeIdName, lLen, false);
        auto lIt = mAllocationsMap.find(lCrc32);

        TAllocInfo* lInfo = nullptr;
        if(lIt != mAllocationsMap.end())
        {
            lInfo = lIt->second;
        }

        SBX_ASSERT(lInfo != nullptr, "Deallocation unknown object");
        if(nullptr != lInfo)
        {
            SBX_ASSERT(lInfo->mCurrentAllocated > 0, "Deallocating an object with 0 instances");
            lInfo->mCurrentAllocated -= 1;

            /*if(lInfo->mCurrentAllocated == 0)
            {
                mAllocationsList.erase(std::find(mAllocationsList.begin(), mAllocationsList.end(), lInfo));
                mAllocationsMap.erase(lCrc32);
            }*/
        }

        

        Sort();
    }

    uint32_t CMemoryProfiler::GetAllocInfoCount () const
    {
        return static_cast< uint32_t >(mAllocationsList.size());
    }

    TAllocInfo const * CMemoryProfiler::GetAllocInfoAt (uint32_t aIndex) const
    {
        return aIndex < mAllocationsList.size() ? mAllocationsList[aIndex] : nullptr;
    }

    void CMemoryProfiler::SetSortCriteria(EMemAllocSortCriteria aSortCriteria)
    {
        if(aSortCriteria < EMemAllocSortCriteria::eCount)
        {
            mSortCriteria = aSortCriteria;
            Sort();
        }
    }

    void CMemoryProfiler::Sort()
    {
        struct greater_current_allocated
        {
            inline bool operator() (const TAllocInfo* struct1, const TAllocInfo* struct2)
            {
                if(struct1->mCurrentAllocated == struct2->mCurrentAllocated)
                {
                    return struct1->mTypeName < struct2->mTypeName;
                }

                return (struct1->mCurrentAllocated > struct2->mCurrentAllocated);
            }
        };

        struct greater_size_allocated
        {
            inline bool operator() (const TAllocInfo* struct1, const TAllocInfo* struct2)
            {
                if((struct1->mCurrentAllocated * struct1->mSize) == (struct2->mCurrentAllocated * struct2->mSize))
                {
                    return struct1->mTypeName < struct2->mTypeName;
                }

                return ((struct1->mCurrentAllocated * struct1->mSize) > (struct2->mCurrentAllocated * struct2->mSize));
            }
        };

        struct greater_total_allocated
        {
            inline bool operator() (const TAllocInfo* struct1, const TAllocInfo* struct2)
            {
                if(struct1->mTotalAllocated == struct2->mTotalAllocated)
                {
                    return struct1->mTypeName < struct2->mTypeName;
                }

                return (struct1->mTotalAllocated > struct2->mTotalAllocated);
            }
        };

        switch(mSortCriteria)
        {
        case EMemAllocSortCriteria::eCurrentInstancesAllocated: std::sort(mAllocationsList.begin(), mAllocationsList.end(), greater_current_allocated()); break;
        case EMemAllocSortCriteria::eCurrentSizeAllocated: std::sort(mAllocationsList.begin(), mAllocationsList.end(), greater_size_allocated()); break;
        case EMemAllocSortCriteria::eTotalInstancesAllocated: std::sort(mAllocationsList.begin(), mAllocationsList.end(), greater_total_allocated()); break;
        };
    }
}