///$(BONE_CODE_HEADER_NOTICE)

#include <sbx/Common.h>
#include "CHeapAllocator.h"
#include "Alignment.h"

//TODO: REVIEW MACROS - SBX_NEXT_MULTIPLE_OF_8 started to fail when converted from a macro to constexpr, why?

namespace sbx
{
#   define HALLOC_GET_PAGEINFO(_page)                  ((TPageInfo*)(uintptr_t(_page) + uintptr_t(HALLOC_PAGE_SIZE) - uintptr_t(sizeof(TPageInfo))))
#   define HALLOC_GET_LIST_INDEX_FROM_SIZE(_size)      ((SBX_NEXT_MULTIPLE_OF_8(_size) >> 3) - 1)
#   define HALLOC_GET_BLOCK_PTR(_pageInfo, _index)     ((uintptr_t*)((uintptr_t(_pageInfo) & SBX_64K_MASK) + uintptr_t(_index * _pageInfo->mBlockSize)))
#   define HALLOC_GET_INFO_FROM_BLOCK_PTR(_ptr)        ((TPageInfo*)((uintptr_t(_ptr) & SBX_64K_MASK) + HALLOC_PAGE_SIZE - sizeof(TPageInfo)))
#   define HALLOC_GET_PAGE(_ptr)                       ((uintptr_t*)(uintptr_t(_ptr) & SBX_64K_MASK))

    //#define HALLOC_EXTREME_VERBOSE 1

    struct CHeapAllocator::TPageInfo
    {
        //pointers to next and previous page infos (duble linked list)
        TPageInfo*  mPreviousPageInfo    {nullptr};
        TPageInfo*  mNextPageInfo        {nullptr};
        
        //number of blocks that can be allocated here (small allocs only)
        uintptr_t   mNumberOfBlocks {0UL};
        
        //size of the blocks allocated in this page (small allocs only)
        size_t      mBlockSize {0UL};
        
        //index in the pages array of the first page
        uintptr_t   mFreeListIndex {0UL};
        
        //in small allocs represents the number of blocks allocated
        //in big allocs represents the number fo bytes allocated
        uint32_t   mAllocations {0UL};
        
        //where the next free block is (small allocs only)
        uintptr_t*  mMarker {nullptr};
        
        #if SBX_ARCH_32BITS
        uint8_t __offset[228];
        #elif SBX_ARCH_64BITS
        uint8_t __offset[200];
        #endif
    };

    CHeapAllocator::CHeapAllocator()
        : mTotalAllocated(0)
        , mTotalRequested(0)
    {
        ::memset(mBook, 0, sizeof(uintptr_t*) * HALLOC_MAX_FREELIST_TYPES);
        Startup();
    }

    CHeapAllocator::~CHeapAllocator()
    {
        Shutdown();
    }

    void CHeapAllocator::Startup()
    {
        mTotalAllocated = 0;
        mTotalRequested = 0;
        for(uint32_t i = 0; i < HALLOC_MAX_FREELIST_TYPES; ++i)
        {
            if(nullptr == mBook[i])
            {
                mBook[i] = CreatePage(nullptr, (i + 1) * 8, i); //bytes: 8, 16, 24, 32, 40 ... 256 and big allocs
            }
        }
    }
    
    void CHeapAllocator::Shutdown()
    {
        //destroy all pages
        for(uint32_t i = 0; i < HALLOC_MAX_FREELIST_TYPES; ++i)
        {
            TPageInfo* lInfo = HALLOC_GET_PAGEINFO(mBook[i]);
        
            while(lInfo != nullptr)
            {
                uintptr_t* lPage = (uintptr_t*)(uintptr_t(lInfo) & SBX_64K_MASK);
                lInfo = lInfo->mNextPageInfo;
                SBX_PLATFORM_ALIGNED_FREE(lPage);
            }
        }
    }
    
    uintptr_t* CHeapAllocator::CreatePage(TPageInfo * aParent, size_t aBlockDataSize, uintptr_t aFreeListIndex)
    {
        //create the page
        uintptr_t* lPage = (uintptr_t*)SBX_PLATFORM_ALIGNED_MALLOC(aligned_uintptr_t(HALLOC_PAGE_SIZE), HALLOC_PAGE_SIZE);
        SBX_ASSERT((uintptr_t(lPage) & SBX_64K_MASK) == uintptr_t(lPage), "[CHeapAllocator::CreatePage] Page allocated in an unaligned location.");
        ::memset(lPage, 0, HALLOC_PAGE_SIZE);
        mTotalAllocated += HALLOC_PAGE_SIZE;

        //get the page info location
        TPageInfo* lInfo            = HALLOC_GET_PAGEINFO(lPage);
        lInfo->mMarker              = lPage;
        lInfo->mPreviousPageInfo    = aParent;
        lInfo->mNextPageInfo        = nullptr;
        lInfo->mFreeListIndex       = aFreeListIndex;
        lInfo->mBlockSize           = aBlockDataSize;
        lInfo->mAllocations         = 0UL;
    
        //if there is a parent store this page as the next one
        if(aParent)
        {
            aParent->mNextPageInfo = lInfo;
        }

        //calculate how many space we can use substracting the page size
        uintptr_t lProfitSpace = uintptr_t(HALLOC_PAGE_SIZE) - uintptr_t(sizeof(TPageInfo));
        
        //calculate the number of blocks allocated in this page
        lInfo->mNumberOfBlocks = uintptr_t(lProfitSpace / aBlockDataSize);

        //make each block point the next one and the last point to null
        for(uintptr_t i = 0; i < lInfo->mNumberOfBlocks - 1; ++i)
        {
            //PTRUINT* currentBlock = (PTRUINT*)(PTRUINT(page) + (i * info->blockSize));
            uintptr_t* lCurrentBlock = HALLOC_GET_BLOCK_PTR(lInfo, i);
            uintptr_t lNextBlock = (uintptr_t(lPage) + ((i + 1) * lInfo->mBlockSize));
            (*lCurrentBlock) = lNextBlock;
        }
        
        uintptr_t* lLastBlock = HALLOC_GET_BLOCK_PTR(lInfo, (lInfo->mNumberOfBlocks - 1));
        (*lLastBlock) = 0UL;
        
        return lPage;
    }

    void* CHeapAllocator::AllocSmallBlock(size_t aBlockSize)
    {
        size_t lTotalBlockSize = aBlockSize + sizeof(size_t);
        uintptr_t lListIndex = HALLOC_GET_LIST_INDEX_FROM_SIZE(lTotalBlockSize);
        SBX_ASSERT(lListIndex < HALLOC_MAX_FREELIST_TYPES, "[CHeapAllocator::AllocSmallBlock] Free list index out of bounds");
        
        //mMutex[lListIndex].lock();

        //get the appropiate freelist first page
        uintptr_t* lPage = mBook[lListIndex];
    
        //get the page info location
        TPageInfo* lInfo = HALLOC_GET_PAGEINFO(lPage);
    
        //if marker points to null means that there are no free blocks in this page, so move next
        while((lInfo->mMarker == nullptr) && lInfo->mNextPageInfo)
        {
            lInfo = lInfo->mNextPageInfo;
        }
    
        //if page without blocks and without next page has been achieved, create a new page
        if((lInfo->mMarker == nullptr) && (lInfo->mNextPageInfo == nullptr))
        {
            lPage = CreatePage(lInfo, lInfo->mBlockSize, lInfo->mFreeListIndex);
            lInfo = lInfo->mNextPageInfo;
        }
    
        uintptr_t* lNewBlock = lInfo->mMarker;
        lInfo->mMarker = (uintptr_t*)(*lNewBlock);
        *((size_t*)lNewBlock) = lTotalBlockSize;
        lInfo->mAllocations++;
        
        mTotalRequested += aBlockSize;

        //mMutex[lListIndex].unlock();

        return lNewBlock + 1;
    
    }
    
    void* CHeapAllocator::AllocBlock(size_t aBlockSize)
    {
        if((aBlockSize + sizeof(size_t)) <= HALLOC_SMALL_ALLOCS_MAX_SIZE)
        {
            return AllocSmallBlock(aBlockSize);
        }
        else
        {
            //Store size
            size_t* lBlock = (size_t*)::malloc(aBlockSize + sizeof(size_t));
            *lBlock = aBlockSize + sizeof(size_t);
            mTotalAllocated += aBlockSize + sizeof(size_t);
            mTotalRequested += aBlockSize;
            return lBlock + 1;
        }
    }
    
    void CHeapAllocator::FreeBlock(void* aPtr)
    {
        SBX_ASSERT(aPtr != nullptr, "[CHeapAllocator::FreeBlock] Invalid Pointer");

        if(aPtr == nullptr) 
            return;

        size_t lBlockSize = GetBlockSize(aPtr);
        if(lBlockSize == 0) 
            return;
        
        aPtr = ((size_t*)aPtr) - 1;

        if(lBlockSize <= HALLOC_SMALL_ALLOCS_MAX_SIZE)
        {
            //find page info
            TPageInfo* lInfo = HALLOC_GET_INFO_FROM_BLOCK_PTR(aPtr);

            //make this block points to the current marker
            (*((uintptr_t**)aPtr)) = lInfo->mMarker;
        
            //make the marker points to this block
            lInfo->mMarker = (uintptr_t*)aPtr;
            lInfo->mAllocations--;
        }
        else
        {
            ::free(aPtr);
            mTotalAllocated -= lBlockSize;
        }

        mTotalRequested -= lBlockSize - sizeof(size_t);
    }

    size_t CHeapAllocator::GetBlockSize (void* aPtr)
    {
        return *(reinterpret_cast<size_t*>(aPtr) - 1);
    }

    size_t CHeapAllocator::GetBinAllocations (size_t aBinIndex) const
    {
        size_t lTotalAllocations = 0;
        SBX_ASSERT(aBinIndex < HALLOC_MAX_FREELIST_TYPES, "[CHeapAllocator::GetBinAllocations] Invalid bin index '%ull'.", aBinIndex);
        TPageInfo* lInfo = HALLOC_GET_PAGEINFO(mBook[aBinIndex]);
        do
        {
            lTotalAllocations += lInfo->mAllocations;
            lInfo = lInfo->mNextPageInfo;
        }
        while(lInfo != nullptr);

        return lTotalAllocations;
    }

    /*
    CHeapAllocator::TPageInfo const * CHeapAllocator::GetPageInfoForSizeIndex(uintptr_t aSizeIndex) const
    {
        SBX_ASSERT(aSizeIndex < HALLOC_MAX_FREELIST_TYPES, "[CHeapAllocator::GetPageInfoForSizeIndex] Size index out of bounds.");
        return HALLOC_GET_PAGEINFO(mBook[aSizeIndex]);

    }*/
} // sbx