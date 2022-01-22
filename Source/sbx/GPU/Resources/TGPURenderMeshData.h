/*
 * @file    TGPURenderMeshData.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_RENDER_MESH_DATA_H__
#define __SBX_GPU_RENDER_MESH_DATA_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/Resources/CGPUProgram.h>
#include <sbx/GPU/Resources/CGPURenderTarget.h>
#include <sbx/GPU/Resources/CGPUMesh.h>

#include <sbx/GPU/Resources/TGPUProgramData.h>
#include <sbx/GPU/Resources/TGPURenderTargetDesc.h>
#include <sbx/GPU/Resources/TGPUMeshDesc.h>


namespace sbx
{
    struct TGPURenderMeshData : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TGPURenderMeshData)
    public:
        TGPUResourceRef mMeshRes;
        TGPUResourceRef mProgramRes;
        TGPUResourceRef mRenderTargetRes;
        TGPUMeshDesc::Ref           mMeshDesc;
        TGPUProgramData::Ref        mProgramData;
        TGPURenderTargetDesc::Ref   mRenderTargetDesc;

        TGPURenderMeshData()
        {;}

        ~TGPURenderMeshData()
        {;}
    };
}

#endif //__SBX_GPU_RENDER_MESH_DATA_H__