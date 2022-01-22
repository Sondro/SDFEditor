/*
 * @file    GLCmdExecuteKernel.h
 * @author  David Gallardo Moreno
 */

namespace
{
    static const size_t kMaxSources = 10;

    uint32_t GLCompileSource(const char* aSourceStr, sbx::EShaderSourceType aType)
    {
        if(sbx::EShaderSourceType::eShaderType_Invalid == aType)
        {
            return UINT_MAX;
        }

        uint32_t lHandle = glCreateShader(kGLSourceType[static_cast< int32_t >(aType)]);
        glShaderSource(lHandle, (GLsizeiptr)1, &aSourceStr, NULL);
        glCompileShader(lHandle);
    
        GLint lStatus;
        glGetShaderiv(lHandle, GL_COMPILE_STATUS, &lStatus);
        if(lStatus != GL_TRUE)
        {
            GLchar lLog[1024];
            GLsizei lLogLen;
            glGetShaderInfoLog(lHandle, sizeof(GLchar)*1024, &lLogLen, lLog);
            SBX_ERROR("[GLCompileSource] Error: %s\n%.255s", lLog, aSourceStr);
            glDeleteShader(lHandle);
            lHandle = UINT_MAX;
        }

        return lHandle;
    }

    uint32_t GLBuildProgram(sbx::TGPUProgramDesc::Ref const & aDesc)
    {
        bool lOk = true;
        uint32_t lHandle = UINT_MAX;
        FVector< uint32_t > lSourceHandleList;
        lSourceHandleList.reserve(aDesc->mSources.size());
        lOk = lOk && aDesc->mSources.size() <= kMaxSources;
        
        if(lOk)
        {
            lHandle = glCreateProgram();
            for(auto const & lSource : aDesc->mSources)
            {
                const char* lSourceStr = reinterpret_cast< const char * >(lSource->mSource->GetByteArray());
                auto lType = lSource->mType;
                uint32_t lSourceHandle = GLCompileSource(lSourceStr, lSource->mType);
            
                if(lSourceHandle != UINT_MAX)
                {
                    lSourceHandleList.push_back(lSourceHandle);
                    glAttachShader(lHandle, lSourceHandle);
                }
                else
                {
                    lOk = false;
                }
            }
        }
        
        if(lOk)
        {
            glLinkProgram(lHandle);
            GLint lStatus;
            glGetProgramiv(lHandle, GL_LINK_STATUS, &lStatus);
            if(lStatus != GL_TRUE)
            {
                GLchar lLog[1024];
                GLsizei lLogLen;
                glGetProgramInfoLog(lHandle, sizeof(GLchar)*1024, &lLogLen, lLog);
                SBX_ERROR("[GLBuildProgram] Error: '%s'.", lLog);
                lOk = false;
            }
        }

        /*if(!lOk)
        {
            glDeleteProgram(lHandle);
            lHandle = UINT_MAX;
        }*/
        
        //Always delete shader sources after link or error
        for(auto lSourceHandle : lSourceHandleList)
        {
            glDeleteShader(lSourceHandle);
        }
        
        return lHandle;
    }
}

namespace sbx { namespace gl 
{
    class GLCreateProgramCmd : public TGPUCommand
    {
    public:
        GLCreateProgramCmd(TGPUResourceRef const & aResRef, TGPUProgramDesc::Ref const & aDesc)
            : mResRef(aResRef)
            , mDesc(aDesc)
        {
        }

        ~GLCreateProgramCmd()
        {
        }

        void Execute()
        {
            uint32_t lHandle = GLBuildProgram(mDesc);
            mResRef.SetHandle(TGPUHandle(lHandle));
            //TODO: check this is not needed for sure (as it seems to work out of the box now)
            /*for(auto & lBinding : mDesc->mBlockBindings)
            {
                glUniformBlockBinding(lHandle, lBinding.mIndex, lBinding.mBinding);
            }*/
        }

    private:
        TGPUResourceRef mResRef;
        TGPUProgramDesc::Ref mDesc;
    };

    class GLDestroyProgramCmd : public TGPUCommand
    {
    public:
        GLDestroyProgramCmd(TGPUResourceRef const & aResRef)
            : mResRef(aResRef)
        {
        }

        ~GLDestroyProgramCmd()
        {
        }

        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            if(lHandle != UINT_MAX)
            {
                glDeleteProgram(lHandle);
            }
        }

    private:
        TGPUResourceRef mResRef;
    };

    class GLExecuteComputeShaderCmd : public TGPUCommand
    {
    public:
        GLExecuteComputeShaderCmd(TGPUResourceRef const & aResRef, TGPUProgramData::Ref const & aData)
            : mResRef(aResRef)
            , mData(aData)
        {
        }

        ~GLExecuteComputeShaderCmd()
        {
        }

        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            glUseProgram(lHandle);

            uint32_t lNextActiveTexture = 0;
            uint32_t lNextCubeActiveTexture = 20;
            GLShaderUniformsSetup(mData.GetPtr(), lNextActiveTexture, lNextCubeActiveTexture);
            
            
           /* glUniform3i(100, mData->mSizeX, mData->mSizeY, mData->mSizeZ);

            const uint32_t kTileSize = 128;
            for(uint32_t y = 0, ly = mData->mSizeY / kTileSize; y < ly; ++y)
            {
                for(uint32_t x = 0, lx = mData->mSizeX / kTileSize; x < lx; ++x)
                {
                    glUniform3i(101, x * kTileSize, y * kTileSize, mData->mSizeZ);
                    glDispatchCompute(kTileSize, kTileSize, mData->mSizeZ);
                }
            }*/

            glDispatchCompute(mData->mSizeX, mData->mSizeY, mData->mSizeZ);
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            lNextActiveTexture = 0;
            lNextCubeActiveTexture = 20;
            GLShaderUniformsCleanup(mData.GetPtr(), lNextActiveTexture, lNextCubeActiveTexture);

            glUseProgram(0);
            if(mData->mForceFinish)
            {
                glFinish();
            }
        }

    private:
        TGPUResourceRef mResRef;
        TGPUProgramData::Ref mData;
    };  

}}