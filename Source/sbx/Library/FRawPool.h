/*
 * @file    FRawPool.h
 * @author  David Gallardo Moreno
 * @brief   Raw fixed size memory blocks allocator
 */

#ifndef __SBX_RAW_POOL_H__
#define __SBX_RAW_POOL_H__

namespace sbx
{
    class FRawPool
    {
        SBX_DISALLOW_COPY(FRawPool)
    public:
        FRawPool();
        ~FRawPool();

        bool Init(size_t aBlockSize, size_t aPageBlocks);
        void End();

        bool IsOk() const;

        void* Alloc();
        void  Free(void* aPtr);
    
    private:
        struct TBlock;
        struct THeader
        {
            THeader*    mNextPage {0};
            THeader*    mPrevPage {0};
            TBlock*     mNextFreeBlock {0};
            size_t      __padding[1];
        };

        struct TBlock
        {
            THeader*    mPage {0};
            TBlock*     mNextFreeBlock {0};
        };

    private:
        void CreatePage();
        void MoveToFirst(THeader* aPage);

    private:

        size_t      mBlockSize;
        size_t      mPageBlocks;
        size_t      mPageSize;
        THeader*    mFirstPage;
        THeader*    mLastPage;
    };
}

#endif //__SBX_RAW_POOL_H__
