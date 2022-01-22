

#include <sbx/Common.h>

#include <sbx/Core/CHeapAllocator.h>

namespace
{
    sbx::CHeapAllocator gHeapMemoryAllocator;
}

void* sbxMalloc (size_t aSize)
{
    return gHeapMemoryAllocator.AllocBlock(aSize);
}

void sbxFree (void* aPtr)
{
    SBX_ASSERT(aPtr, "[sbxFree] Trying to free memory from a NULL ptr");
    gHeapMemoryAllocator.FreeBlock(aPtr);
}

void* sbxRealloc(void* aPtr, size_t aSize)
{
    void* lNewPtr = gHeapMemoryAllocator.AllocBlock(aSize);
    ::memmove(lNewPtr, aPtr, gHeapMemoryAllocator.GetBlockSize(aPtr));
    gHeapMemoryAllocator.FreeBlock(aPtr);
    return lNewPtr;
}

size_t sbxAllocatedMemory ()
{
    return gHeapMemoryAllocator.GetTotalAllocated();
}

size_t sbxRequestedMemory ()
{
    return gHeapMemoryAllocator.GetTotalRequested();
}

size_t sbxGetAllocatorBinSizesCount ()
{
    return gHeapMemoryAllocator.GetBinSizesCount();
}

size_t sbxGetAllocatorBinAllocs (size_t aBinIndex)
{
    return gHeapMemoryAllocator.GetBinAllocations(aBinIndex);
}