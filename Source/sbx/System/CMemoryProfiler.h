/*
 * @file    CMemoryProfiler.h
 * @author  David Gallardo Moreno
 */

#include <sbx/Utils/LangUtils.h>
#include <sbx/Library/FVector.h>
#include <map>
#include <string>

namespace sbx
{
    enum class EMemAllocSortCriteria
    {
        eCurrentInstancesAllocated,
        eCurrentSizeAllocated,
        eTotalInstancesAllocated,
        eCount,
        eInvalid = -1
    };

    struct TAllocInfo
    {
        uint32_t    mTypeId;
        std::string mTypeName;
        size_t      mSize;
        int32_t     mTotalAllocated;
        int32_t     mCurrentAllocated;
    };

    class CMemoryProfiler : public CSingleton< CMemoryProfiler >
    {
    public:
        CMemoryProfiler();
        ~CMemoryProfiler();

        void                AddAllocation       (uint32_t aTypeId, const char* aTypeIdName, size_t aSize);
        void                RemoveAllocation    (uint32_t aTypeId, const char* aTypeIdName);

        uint32_t            GetAllocInfoCount   () const;
        TAllocInfo const *  GetAllocInfoAt      (uint32_t aIndex) const;
        void                SetSortCriteria     (EMemAllocSortCriteria aSortCriteria);
        void                Sort                ();

    private:
        std::map< uint32_t, TAllocInfo* > mAllocationsMap;
        FVector< TAllocInfo* > mAllocationsList;
        EMemAllocSortCriteria mSortCriteria;
    };

} // sbx
