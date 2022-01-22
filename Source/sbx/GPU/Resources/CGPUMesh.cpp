/*
 * @file    CGPUMesh.cpp
 * @author  David Gallardo Moreno
 */


#include <sbx/Common.h>
#include "CGPUMesh.h"
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>

namespace
{
    UID(MeshVBO);
    UID(MeshVAO);
}

namespace sbx
{
    CGPUMesh::CGPUMesh( TGPUMeshDesc::Ref const & aDesc, 
                        CRawBuffer::Ref const & aBuffer,
                        TGPUCommandCallback const & aCallback, 
                        TGPUCommandCallback const & aDestroyCallback)
        : mDesc(aDesc)
        , mDestroyCallback(aDestroyCallback)
        , mVAORes(0)
        , mVBORes(0)
    {
        mVAORes = GetGPUResourceManager()->CreateResource(0, uid(MeshVAO));
        mVBORes = GetGPUResourceManager()->CreateResource(0, uid(MeshVBO));
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCmd = lMgr->AllocCreateMeshObjectCmd(mVAORes, mVBORes, mDesc, aBuffer);
        lCmd->mCallback = aCallback;
        lMgr->QueueCommand(lCmd);
    }

    CGPUMesh::~CGPUMesh()
    {
        CGPUCommandManager* lMgr = GetGPUCommandManager();
        TGPUCommand* lCmd = lMgr->AllocDestroyMeshObjectCmd(mVAORes, mVBORes);
        lCmd->mCallback = mDestroyCallback;
        lMgr->QueueCommand(lCmd);
    }
} // sbx
