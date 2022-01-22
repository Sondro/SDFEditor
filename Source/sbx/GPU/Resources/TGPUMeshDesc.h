/*
 * @file    TGPUMeshDesc.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_GPU_MESH_DESC_H__
#define __SBX_GPU_MESH_DESC_H__

#include <sbx/Library/FRefCounted.h>
#include <sbx/DataStructures/CRawBuffer.h>
#include <sbx/Library/FVector.h>

namespace sbx
{
    enum class EGPUAttribType
    {
        eAttrib_Byte = 0, 
        eAttrib_UnsignedByte, 
        eAttrib_Integer, 
        eAttrib_UnsignedInt,
        eAttrib_Float,
        eAttrib_Count,
        eAttrib_Invalid = -1
    };

    struct TGPUVertexAttribDesc
    {
        uint8_t         mIndex;
        uint8_t         mComponents;
        EGPUAttribType  mType;
        uint32_t        mOffset;

        TGPUVertexAttribDesc(   uint8_t         aIndex,
                                uint8_t         aComponents,
                                EGPUAttribType  aType,
                                uint32_t        aOffset)
            : mIndex(aIndex)
            , mComponents(aComponents)
            , mType(aType)
            , mOffset(aOffset)
        {}

    };

    struct TGPUVertexAtrributesList
    {
    public:
        FVector< TGPUVertexAttribDesc > mList;
        size_t mVertexSize;

        TGPUVertexAtrributesList()
            : mVertexSize(0)
            , mList()
        {;}

       ~TGPUVertexAtrributesList()
       {
           /*for(auto * lAttrib : mList)
           {
               SBX_DELETE(lAttrib);
           }*/

           mList.clear();
       }

        void AddAttribute(uint8_t aIndex, uint8_t aComponents, EGPUAttribType aType, uint32_t aOffset)
        {
            //mList.push_back(SBX_NEW(TGPUVertexAttribDesc, aIndex, aComponents, aType, aOffset));        
            mList.emplace_back(aIndex, aComponents, aType, aOffset);
        }
    };

    struct TGPUMeshDesc : FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TGPUMeshDesc)
    public:

        size_t                      mNumVertex;
        TGPUVertexAtrributesList    mVertexAttributesList;

        TGPUMeshDesc()
            : mNumVertex(0)
        {
            //mVertexAttributesList = SBX_NEW(TGPUVertexAtrributesList);
        }

        ~TGPUMeshDesc()
        {
            //SBX_DELETE(mVertexAttributesList);
        }

        static size_t GetAttribSize(EGPUAttribType aAttribType)
        {
            static const size_t sAttribSizes[] = { 1, 1, 4, 4, 4 };
            return ((aAttribType < EGPUAttribType::eAttrib_Count) && 
                    (aAttribType != EGPUAttribType::eAttrib_Invalid)) 
                    ? sAttribSizes[static_cast< uint8_t >(aAttribType)] 
                    : 0;
        }
    };
}

#endif //__SBX_GPU_MESH_DESC_H__