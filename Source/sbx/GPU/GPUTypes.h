/*
 * @file    GPUTypes.h
 * @author  David Gallardo Moreno
 */

#ifndef _GPU_TYPES_H_
#define _GPU_TYPES_H_

#include <sbx/Library/FRefCounted.h>
#include <sbx/DataStructures/CRawBuffer.h>
#include <sbx/Library/FVector.h>
#include <functional>

namespace sbx
{
    //-----------------------------------
    // GPU RESOURCES
    //-----------------------------------

    typedef uint32_t  TGPUHandle;

    typedef std::function< void(void) > TGPUCommandCallback;
   
   

    //-----------------------------------
    // GPU SHADER SOURCES
    //-----------------------------------

    enum class EShaderSourceType : int8_t
    {
        eShaderType_Vertex = 0,
        eShaderType_Fragment,
        eShaderType_Compute,
        eShaderType_TessellationControl,
        eShaderType_TessellationEvaluation,
        eShaderType_Geometry,
        eShaderType_Count,
        eShaderType_Invalid = -1
    };

    struct TGPUShaderSource : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TGPUShaderSource)
        SBX_DELETE_DEFAULT_CTORS(TGPUShaderSource)
    public:
        CRawBuffer::Ref       mSource;
        EShaderSourceType   mType;

        TGPUShaderSource(CRawBuffer::Ref    aSource,
                         EShaderSourceType  aType)
            : mSource(aSource)
            , mType(aType)
        {
        }
    };

    struct TGPUProgramDesc : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(TGPUProgramDesc)
    public:
        FVector< TGPUShaderSource::Ref > mSources;
        TGPUProgramDesc() = default;
        ~TGPUProgramDesc() = default;
    };

    struct TBlitInfo
    {
        uint32_t mSrcX0;
 	    uint32_t mSrcY0;
 	    uint32_t mSrcX1;
 	    uint32_t mSrcY1;
 	    uint32_t mDstX0;
 	    uint32_t mDstY0;
 	    uint32_t mDstX1;
 	    uint32_t mDstY1;
        
        uint32_t mColorAttachments;

        TBlitInfo(uint32_t aSrcX0,
                  uint32_t aSrcY0,
                  uint32_t aSrcX1,
                  uint32_t aSrcY1,
                  uint32_t aDstX0,
                  uint32_t aDstY0,
                  uint32_t aDstX1,
                  uint32_t aDstY1,
                  uint32_t aColorAttachments)
            : mSrcX0(aSrcX0)
            , mSrcY0(aSrcY0)
            , mSrcX1(aSrcX1)
            , mSrcY1(aSrcY1)
            , mDstX0(aDstX0)
            , mDstY0(aDstY0)
            , mDstX1(aDstX1)
            , mDstY1(aDstY1)
            , mColorAttachments(aColorAttachments)
        {;}
    };
} // sbx

#endif // _GPU_TYPES_H_