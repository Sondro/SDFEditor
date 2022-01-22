/*
 * @file    Memory.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_CORE_MEMORY_H__
#define __SBX_CORE_MEMORY_H__

//Memory Allocation
void*   sbxMalloc           (size_t aSize);
void    sbxFree             (void* aPtr);
void*   sbxRealloc          (void* aPtr, size_t aSize);
size_t  sbxAllocatedMemory  ();
size_t  sbxRequestedMemory  ();

size_t  sbxGetAllocatorBinSizesCount    ();
size_t  sbxGetAllocatorBinAllocs        (size_t aBinIndex);

#endif // __SBX_CORE_MEMORY_H__
