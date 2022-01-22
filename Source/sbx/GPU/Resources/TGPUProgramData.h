/*
 * @file    TGPUKernelData.h
 * @author  David Gallardo Moreno
 */

#ifndef _GPU_PROGRAM_DATA_H
#define _GPU_PROGRAM_DATA_H

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/Resources/CGPUTexture.h>
#include <sbx/GPU/Resources/CGPUCubeMap.h>
#include <sbx/GPU/Resources/CGPUSharedBlock.h>

namespace sbx
{
    //-----------------------------------
    // GPU KERNEL EXECUTION DATA
    //-----------------------------------

    enum class EProgramParamType : int32_t
    {
        eParamType_Float = 0,
        eParamType_Integer,
        eParamType_Color,
        eParamType_IntVec3,
        eParamType_IntVec4,
        eParamType_Matrix3,
        eParamType_Matrix4,
        eParamType_Image,
        eParamType_Texture,
        eParamType_CubeMap,
        eParamType_SharedBlock,
        eParamType_Buffer, //wip
        eParamType_Count,
        eParamType_Invalid = -1
    };

    struct TGPUProgramParamInfo
    {
        EProgramParamType   mType { EProgramParamType::eParamType_Float };
        union
        {
            int32_t             mBinding;
            int32_t             mLocation;
        };
        CGPUTexture::Ref        mTextureRef;
        CGPUCubeMap::Ref        mCubeMapRef;
        CGPUSharedBlock::Ref    mSharedBlockRef;

        union
        {
            int32_t     mIntegerValue;
            float       mFloatValue;
            TColor32F   mColorValue;
            int32_t     mIVecValue[4];
            float       mMatrix3Value[9];
            float       mMatrix4Value[16];
        };

        TGPUProgramParamInfo() 
            : mBinding(0)
            , mColorValue(0.f, 0.f, 0.f, 0.f)
        {
            ::memset(mMatrix4Value, 0, 16 * sizeof(float));
        }

        TGPUProgramParamInfo(TGPUProgramParamInfo const & aOther)
        {
            CopyFrom(aOther);
        }

        TGPUProgramParamInfo& operator=(TGPUProgramParamInfo const & aOther)
        {
            CopyFrom(aOther);
            return *this;
        }
        
		//TODO: review this
        /*TGPUProgramParamInfo(TGPUProgramParamInfo && aOther)
        {
            MoveFrom(MoveCast(aOther));
        }


        TGPUProgramParamInfo& operator=(TGPUProgramParamInfo && aOther)
        {
            MoveFrom(MoveCast(aOther));
            return *this;
        }*/

        void CopyFrom(TGPUProgramParamInfo const & aOther)
        {
            mType = aOther.mType;
            mBinding = aOther.mBinding;
            mTextureRef = aOther.mTextureRef;
            mCubeMapRef = aOther.mCubeMapRef;
            ::memcpy(mMatrix4Value, aOther.mMatrix4Value, sizeof(float) * 16);
        }

        void MoveFrom(TGPUProgramParamInfo && aOther)
        {
            sbx::Swap( mType, aOther.mType );
            sbx::Swap( mBinding, aOther.mBinding );
            sbx::Swap( mTextureRef, aOther.mTextureRef );
            sbx::Swap( mCubeMapRef, aOther.mCubeMapRef );
            float lMatrixAux[16];
            ::memcpy(lMatrixAux, aOther.mMatrix4Value, sizeof(float) * 16);
            ::memcpy(aOther.mMatrix4Value, mMatrix4Value, sizeof(float) * 16);
            ::memcpy(mMatrix4Value, aOther.mMatrix4Value, sizeof(float) * 16);
        }

        ~TGPUProgramParamInfo()
        {
        }
    };

    //Store uniform data and textures
    struct TGPUProgramData : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TGPUProgramData)
    public:
        TGPUProgramData::Ref mBaseData;
        FVector< TGPUProgramParamInfo > mParameters;
        uint32_t mSizeX;
        uint32_t mSizeY;
        uint32_t mSizeZ;
        bool        mChopInvocations;
        uint32_t    mChopExtent;
        bool    mForceFinish { true };

        TGPUProgramData(uint32_t aSizeX = 1,
                        uint32_t aSizeY = 1,
                        uint32_t aSizeZ = 1)
            : mSizeX(aSizeX)
            , mSizeY(aSizeY)
            , mSizeZ(aSizeZ)
            , mChopInvocations(false)
            , mChopExtent(aSizeX)
        {;}

        ~TGPUProgramData()
        {
        }
    }; 
}

#endif //_GPU_KERNEL_DATA_H