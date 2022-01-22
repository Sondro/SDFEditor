/*
 * @file    FRawPool.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "FRawPool.h"

namespace
{
    /*
    ** http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
    */
#   define __LOG2A(s) ((s &0xffffffff00000000) ? (32 +__LOG2B(s >>32)): (__LOG2B(s)))
#   define __LOG2B(s) ((s &0xffff0000)         ? (16 +__LOG2C(s >>16)): (__LOG2C(s)))
#   define __LOG2C(s) ((s &0xff00)             ? (8  +__LOG2D(s >>8)) : (__LOG2D(s)))
#   define __LOG2D(s) ((s &0xf0)               ? (4  +__LOG2E(s >>4)) : (__LOG2E(s)))
#   define __LOG2E(s) ((s &0xc)                ? (2  +__LOG2F(s >>2)) : (__LOG2F(s)))
#   define __LOG2F(s) ((s &0x2)                ? (1)                  : (0))

#   define LOG2_UINT64 __LOG2A
#   define LOG2_UINT32 __LOG2B
#   define LOG2_UINT16 __LOG2C
#   define LOG2_UINT8  __LOG2D

    static inline bool sbxIsPowerOf2(uint64_t n){ return (n & (n - 1)) == 0; }

    static inline uint64_t
    sbxNextPowerOf2(uint64_t i)
    {
#   if SBX_COMPILER_GCC
        return 1ULL <<(1 +(63 -__builtin_clzl(i -1)));
#   else
        i =i -1;
        i =LOG2_UINT64(i);
        return 1ULL <<(1 +i);
#   endif
    }

    const static size_t kMinBlockSize = sizeof(uintptr_t);
}

namespace sbx
{
    FRawPool::FRawPool()
        : mBlockSize(0)
        , mPageBlocks(0)
        , mFirstPage(nullptr)
    {
    }

    FRawPool::~FRawPool()
    {
        End();
    }

    bool FRawPool::Init(size_t aBlockSize, size_t aPageBlocks)
    {
        SBX_ASSERT(mFirstPage == nullptr, "Trying to init an already initialized pool %u %u.", aBlockSize, aPageBlocks);
        SBX_ASSERT(aBlockSize >= kMinBlockSize);
        SBX_ASSERT(aPageBlocks > 0);

        if( (mFirstPage == nullptr) && 
            (aBlockSize >= kMinBlockSize) && 
            (aPageBlocks > 0))
        {
            //Add extra space to store a page pointer
            aBlockSize += sizeof(THeader*);

            //Aligned blocks
            //TODO: properly align / size this
            mBlockSize = (sbxIsPowerOf2(aBlockSize)) ? aBlockSize : sbxNextPowerOf2(aBlockSize);
            mPageBlocks = aPageBlocks;
            mPageSize = sizeof(THeader) + (mBlockSize * mPageBlocks);
            //mPageSize = (sbxIsPowerOf2(mPageSize)) ? mPageSize : sbxNextPowerOf2(mPageSize);

            //Create default page
            CreatePage();

            return (mFirstPage != nullptr);
        }

        return false;
    }

    void FRawPool::End()
    {
        while(mFirstPage != nullptr)
        {
            //Delete all pages
            THeader* lNext = mFirstPage->mNextPage;
            SBX_FREE(mFirstPage);
            mFirstPage = lNext;
        }
    }

    bool FRawPool::IsOk() const
    {
        return (mFirstPage != nullptr);
    }

    void* FRawPool::Alloc()
    {
        SBX_ASSERT(IsOk(), "Trying to alloc from an uninitialized pool.");

        if(IsOk())
        {
            THeader* lPage = mFirstPage;
            //Iterate pages until we find a page with a free block
            while(lPage->mNextFreeBlock == nullptr)
            {
                //If there are no more pages to iterate, create one
                if(!lPage->mNextPage)
                {
                    CreatePage();
                    break;
                }
                else
                {
                    lPage = lPage->mNextPage;
                    //If a page with a free block is found, move it to the first position
                    if(lPage->mNextFreeBlock != nullptr)
                    {
                        MoveToFirst(lPage);
                    }
                }
            }

            //Return a pointer to the next free block after the page pointer
            TBlock* lBlock = mFirstPage->mNextFreeBlock;
            mFirstPage->mNextFreeBlock = lBlock->mNextFreeBlock;
            return &lBlock->mNextFreeBlock;
        }
        return nullptr;
    }

    void  FRawPool::Free(void* aPtr)
    {
        SBX_ASSERT(IsOk(), "Trying to alloc in an uninitialized pool.");

        if(IsOk())
        {
            //Block starts a pointer to THeader before the returned pointer in Alloc
            TBlock* lBlock = (TBlock*)(uintptr_t(aPtr) - sizeof(THeader*));
            THeader* lPage = lBlock->mPage;

            //Replaces the next free block in the linked list of free blocks
            lBlock->mNextFreeBlock = lPage->mNextFreeBlock;
            lPage->mNextFreeBlock = lBlock;

            //Move this page to first position
            MoveToFirst(lPage);
        }
    }
    
    void FRawPool::CreatePage()
    {
        //Allocate Page and replace the first page
        THeader* lNewPage = (THeader*)SBX_MALLOC(mPageSize);
        lNewPage->mNextPage = mFirstPage;
        lNewPage->mPrevPage = 0;
        if(lNewPage->mNextPage)
        {
            lNewPage->mNextPage->mPrevPage = lNewPage;
        }
        mFirstPage = lNewPage;

        //Set the next free block the position after the header
        lNewPage->mNextFreeBlock = (TBlock*)(uintptr_t(lNewPage) + sizeof(THeader));

        //Iterate the page to initialize all blocks
        for(uintptr_t lBlockUPtr = uintptr_t(lNewPage->mNextFreeBlock), 
                      lLimit = uintptr_t(lNewPage->mNextFreeBlock) + ((mPageBlocks - 1) * mBlockSize); 
            lBlockUPtr < lLimit; 
            lBlockUPtr += mBlockSize)
        {
            TBlock* lBlock = (TBlock*)(lBlockUPtr);

            //Store the pointer to the page
            lBlock->mPage = lNewPage;

            //Store the pointer to the next free block, creating a linked list of free blocks
            lBlock->mNextFreeBlock = (TBlock*)(lBlockUPtr + mBlockSize);
        }

        //Last element points to nullptr as next free block
        TBlock* lLastBlock = (TBlock*)(uintptr_t(lNewPage->mNextFreeBlock) + ((mPageBlocks - 1) * mBlockSize));
        lLastBlock->mPage = lNewPage;
        lLastBlock->mNextFreeBlock = nullptr;
    }

    void FRawPool::MoveToFirst(THeader* aPage)
    {
        SBX_ASSERT(aPage != nullptr, "Trying to move a NULL page.");

        //If is not already the first page, move the page "node" to first in the linked list
        if((aPage != nullptr) && (mFirstPage != aPage))
        {
            if(aPage->mPrevPage)
            {
                aPage->mPrevPage->mNextPage = aPage->mNextPage;
            }

            if(aPage->mNextPage)
            {
                aPage->mNextPage->mPrevPage = aPage->mPrevPage;
            }

            mFirstPage->mPrevPage = aPage;
            aPage->mNextPage = mFirstPage;
            mFirstPage = aPage;
            mFirstPage->mPrevPage = 0;
        }
    }
}