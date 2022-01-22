/*
 * @file    CHeapAllocator.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_HEAP_ALLOCATOR_H__
#define __SBX_HEAP_ALLOCATOR_H__

//#include <mutex>
//#include <atomic>

/*
#define SBX_HALLOC_MAX_PAGE_SIZE (0x10000UL)
#define SBX_HALLOC_MAX_FREELIST_TYPES 33
#define SBX_HALLOC_BIG_ALLOCS_INDEX (BONE_HALLOC_MAX_FREELIST_TYPES - 1)
#define SBX_HALLOC_BIG_ALLOCS_ALIGNMENT 256UL
*/

#if SBX_ARCH_32BITS
    #define SBX_64K_MASK (0xFFFF0000UL)
#elif SBX_ARCH_64BITS
    #define SBX_64K_MASK (0xFFFFFFFFFFFF0000UL)
#endif

namespace sbx
{
    class CHeapAllocator
    {
        enum 
        {
            HALLOC_PAGE_SIZE = (0x10000UL),
            HALLOC_MAX_FREELIST_TYPES = 32,
            HALLOC_SMALL_ALLOCS_MAX_SIZE = (HALLOC_MAX_FREELIST_TYPES) * 8UL,
            //HALLOC_BIG_ALLOCS_MAX_SIZE = 4096UL
        };

    public:
        struct TPageInfo;
        
                    CHeapAllocator      ();
                   ~CHeapAllocator      ();
        void        Startup             ();
        void        Shutdown            ();
        void*       AllocBlock          (size_t aBlockSize);
        void        FreeBlock           (void* aPtr);
        size_t      GetBlockSize        (void* aPtr);

        size_t      GetTotalAllocated   () const { return mTotalAllocated; }
        size_t      GetTotalRequested   () const { return mTotalRequested; }
        size_t      GetBinSizesCount    () const { return HALLOC_MAX_FREELIST_TYPES; }
        size_t      GetBinAllocations   (size_t aBinIndex) const;

    private:
        uintptr_t*  CreatePage          (TPageInfo * aParent, size_t aBlockDataSize, uintptr_t aFreeListIndex);
        void*       AllocSmallBlock     (size_t aBlockSize);

    private:
        uintptr_t*  mBook[HALLOC_MAX_FREELIST_TYPES]; //bytes: 8, 16, 24, 32, 40 ... 256 and bigAllocs
        //std::mutex mMutex[HALLOC_MAX_FREELIST_TYPES];
        size_t /*std::atomic_size_t*/  mTotalRequested {0};
        size_t /*std::atomic_size_t*/  mTotalAllocated {0};  
    };
}

#endif //__SBX_ERROR_HANDLING_H__