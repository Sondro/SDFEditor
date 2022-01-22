/*
 * @file    TGPUResourceRef.cpp
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_GPU_RESOURCE_REF_H_
#define _SBX_GPU_RESOURCE_REF_H_

#include <sbx/GPU/GPUTypes.h>

namespace sbx
{
    struct TGPUResourceRef
    {
    public:
                            TGPUResourceRef     ();
                            TGPUResourceRef     (void* aInnerData);
                            TGPUResourceRef     (TGPUResourceRef const & aOther);
                           ~TGPUResourceRef     ();
        TGPUResourceRef &   operator=           (TGPUResourceRef const & aOther);
        void                SetHandle           (TGPUHandle aHandle);
        TGPUHandle          GetHandle           ();
    private:
        void* mInnerData;
    };
} // sbx

#endif  //_SBX_GPU_RESOURCE_REF_H_