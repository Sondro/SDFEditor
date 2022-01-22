/*
 * @file    GLShared.inl
 * @author  David Gallardo Moreno
 */

namespace
{
    using namespace sbx;

    static const GLenum kGLFormat[] = 
    {
        GL_RED,
        GL_RGBA,
        GL_DEPTH_COMPONENT,
    };

    static const GLenum kGLChannelSize[] = 
    {
        GL_UNSIGNED_BYTE,
        GL_FLOAT,
        GL_FLOAT
    };

    static const GLint kGLInternalFormat[] = 
    {
        GL_R8,
        GL_R16F,
        GL_R32F,
        GL_RGBA8,
        GL_RGBA16F,
        GL_RGBA32F,
        GL_DEPTH_COMPONENT32
    };

    static const GLenum kGLSourceType[] = 
    {
        GL_VERTEX_SHADER,
        GL_FRAGMENT_SHADER,
        GL_COMPUTE_SHADER,
        GL_TESS_CONTROL_SHADER,
        GL_TESS_EVALUATION_SHADER,
        GL_GEOMETRY_SHADER
    };

    static const GLenum kGLVertexAttribType[] =
    {
        GL_BYTE,
        GL_UNSIGNED_BYTE,
        GL_INT,
        GL_UNSIGNED_INT,
        GL_FLOAT
    };

    void GLShaderPushUniform(TGPUProgramParamInfo const & aParam, 
                             uint32_t & aNextActiveTexture,
                             uint32_t & aNextCubeActiveTexture)
    {
        static TTextureDesc::TTextureFormatSizeInfo lFormatSizeInfo;

        auto & lParam = aParam;
        switch(lParam.mType)
        {
        case EProgramParamType::eParamType_Float:
            glUniform1f(lParam.mLocation, lParam.mFloatValue);
            break;
        case EProgramParamType::eParamType_Integer:
            glUniform1i(lParam.mLocation, lParam.mIntegerValue);
        case EProgramParamType::eParamType_Color:
            glUniform4f(lParam.mLocation, lParam.mColorValue.r, lParam.mColorValue.g, lParam.mColorValue.b, lParam.mColorValue.a);
            break;
        case EProgramParamType::eParamType_IntVec3:
            glUniform3iv(lParam.mLocation, 1, lParam.mIVecValue);
            break;
        case EProgramParamType::eParamType_IntVec4:
            glUniform4iv(lParam.mLocation, 1, lParam.mIVecValue);
            break;
        case EProgramParamType::eParamType_Matrix3:
            glUniformMatrix3fv(lParam.mLocation, 1, false, lParam.mMatrix3Value);
			break;
        case EProgramParamType::eParamType_Matrix4:
            glUniformMatrix4fv(lParam.mLocation, 1, false, lParam.mMatrix4Value);
            break;
        case EProgramParamType::eParamType_Image:
            lParam.mTextureRef->GetDesc().GetFormatSizeInfo(lFormatSizeInfo);
            glBindImageTexture( lParam.mBinding, 
                                lParam.mTextureRef->GetRes().GetHandle(), 
                                0, 
                                GL_FALSE, 
                                0, 
                                GL_READ_WRITE, 
                                kGLInternalFormat[lFormatSizeInfo.mInternalFormat]); 
            break;
        case EProgramParamType::eParamType_Texture:
            SBX_ASSERT(lParam.mTextureRef, "No texture set for shader program parameter");
            if(lParam.mTextureRef)
            {
                glActiveTexture(GL_TEXTURE0 + aNextActiveTexture);
                glBindTexture(GL_TEXTURE_2D, lParam.mTextureRef->GetRes().GetHandle());
                glUniform1i(lParam.mBinding, aNextActiveTexture);
                ++aNextActiveTexture;
            }
            break;
        case EProgramParamType::eParamType_CubeMap:
            SBX_ASSERT(lParam.mCubeMapRef, "No cube map set for shader program parameter");
            if(lParam.mCubeMapRef)
            {
                glActiveTexture(GL_TEXTURE0 + aNextCubeActiveTexture);
                glBindTexture(GL_TEXTURE_CUBE_MAP, lParam.mCubeMapRef->GetRes().GetHandle());
                glUniform1i(lParam.mBinding, aNextCubeActiveTexture);
                ++aNextCubeActiveTexture;
            }
            break;
        case EProgramParamType::eParamType_SharedBlock:
            SBX_ASSERT(lParam.mSharedBlockRef, "No shared block set for shader program parameter");
            if(lParam.mSharedBlockRef)
            {
                glBindBufferRange(GL_UNIFORM_BUFFER, lParam.mBinding, lParam.mSharedBlockRef->GetBlockRes().GetHandle(), 0, lParam.mSharedBlockRef->GetSize());
            }
            break;
        case EProgramParamType::eParamType_Buffer:
            //TODO implement buffer and other needed types
            break;
        }
    }

    void GLShaderCleanUniform(TGPUProgramParamInfo const & aParam, 
                              uint32_t & aNextActiveTexture,
                              uint32_t & aNextCubeActiveTexture)
    {
        auto & lParam = aParam;
        switch(lParam.mType)
        {
        case EProgramParamType::eParamType_Image:
            //lParam.mTextureRef->GetDesc()->GetFormatSizeInfo(lFormatSizeInfo);
            glBindImageTexture( lParam.mBinding, 
                                UINT_MAX, 
                                0, 
                                GL_FALSE, 
                                0, 
                                GL_READ_WRITE, 
                                0);
            break;
        case EProgramParamType::eParamType_Texture:
            glActiveTexture(GL_TEXTURE0 + aNextActiveTexture);
            glBindTexture(GL_TEXTURE_2D, 0);
            ++aNextActiveTexture;
            break;
        case EProgramParamType::eParamType_CubeMap:
            glActiveTexture(GL_TEXTURE0 + aNextCubeActiveTexture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            ++aNextCubeActiveTexture;
            break;
        case EProgramParamType::eParamType_SharedBlock:
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        case EProgramParamType::eParamType_Buffer:
            //TODO implement buffer and other needed types
            break;
        }
    }

    void GLShaderUniformsSetup(const TGPUProgramData* aData, 
                              uint32_t & aNextActiveTexture,
                              uint32_t & aNextCubeActiveTexture)
    {

        for(auto & lParam : aData->mParameters)
        {
            GLShaderPushUniform(lParam, aNextActiveTexture, aNextCubeActiveTexture);
        }

        if(aData->mBaseData)
        {
            GLShaderUniformsSetup(aData->mBaseData.GetPtr(), aNextActiveTexture, aNextCubeActiveTexture);
        }
    }

    void GLShaderUniformsCleanup(const TGPUProgramData* aData, 
                              uint32_t & aNextActiveTexture,
                              uint32_t & aNextCubeActiveTexture)
    {
        for(auto & lParam : aData->mParameters)
        {
            GLShaderCleanUniform(lParam, aNextActiveTexture, aNextCubeActiveTexture);
        }

        if(aData->mBaseData)
        {
            GLShaderUniformsCleanup(aData->mBaseData.GetPtr(), aNextActiveTexture, aNextCubeActiveTexture);
        }
    }
}
