/*
 * @file    CGPUSharedBlock.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_SHAREDBLOCK_H__
#define __SBX_GPU_SHAREDBLOCK_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>
#include <sbx/DataStructures/CRawBuffer.h>

namespace sbx
{    
    class CGPUSharedBlock : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CGPUSharedBlock)
    public:
                                    CGPUSharedBlock     (CRawBuffer::Ref const & aBuffer,
                                                         TGPUCommandCallback const & aCallback = nullptr, 
                                                         TGPUCommandCallback const & aDestroyCallback = nullptr);
                                   ~CGPUSharedBlock     ();
        TGPUResourceRef             GetBlockRes         () const { return mBlockRes; }
        size_t                      GetSize             () const { return mSize; }
        void                        Update              (CRawBuffer::Ref const & aBuffer, uint32_t aOffset, uint32_t aSize, TGPUCommandCallback const & aCallback = nullptr);

    private:
        TGPUResourceRef         mBlockRes;
        size_t                  mSize;
        TGPUCommandCallback     mDestroyCallback;
    };
    
} // sbx::gfx

#endif // __SBX_GPU_SHAREDBLOCK_H__
