/*
 * @file    CGPUMesh.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_MESH_H__
#define __SBX_GPU_MESH_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>
#include <sbx/GPU/Resources/TGPUMeshDesc.h>
#include <sbx/DataStructures/CRawBuffer.h>

namespace sbx
{    
    class CGPUMesh : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CGPUMesh)
    public:
                                    CGPUMesh        (TGPUMeshDesc::Ref const & aDesc, 
                                                     CRawBuffer::Ref const & aBuffer,
                                                     TGPUCommandCallback const & aCallback = nullptr, 
                                                     TGPUCommandCallback const & aDestroyCallback = nullptr);

                                   ~CGPUMesh        ();
        TGPUMeshDesc::Ref const &   GetDesc         () const { return mDesc; }
        TGPUResourceRef             GetVAORes       () const { return mVAORes; }
        TGPUResourceRef             GetVBORes       () const { return mVBORes; }

    private:
        TGPUMeshDesc::Ref       mDesc;
        TGPUResourceRef         mVAORes;
        TGPUResourceRef         mVBORes;
        TGPUCommandCallback     mDestroyCallback;
    };
    
} // sbx::gfx

#endif // __SBX_GPU_MESH_H__
