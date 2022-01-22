/*
 * @file    GLTextureCommands.inl
 * @author  David Gallardo Moreno
 */

namespace sbx { namespace gl 
{
    //-----------------------------------------------------------
    // CREATE TEXTURE
    //-----------------------------------------------------------
    class GLCreateTextureCmd : public TGPUCommand
    {
    public:
        GLCreateTextureCmd(TGPUResourceRef const & aResRef, TTextureDesc const & aDesc)
            : mResRef(aResRef)
            , mDesc(aDesc)
        {
            TTextureDesc::TTextureFormatSizeInfo lFormatSizeInfo;
            mDesc.GetFormatSizeInfo(lFormatSizeInfo);
            mGLFormat = kGLFormat[lFormatSizeInfo.mFormat];
            mGLChannelSize = kGLChannelSize[lFormatSizeInfo.mChannelSize];
            mGLInternalFormat = kGLInternalFormat[lFormatSizeInfo.mInternalFormat];
        }

        ~GLCreateTextureCmd()
        {
        }

        void Execute()
        {
            GLuint lHandle;            
            if(mDesc.mFlags & TTextureDesc::eFlag_Multisample) 
            {
                glGenTextures(1, &lHandle);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, lHandle);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, mDesc.mSamples, mGLInternalFormat, mDesc.mWidth, mDesc.mHeight, false);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            }
            else
            {
                glGenTextures(1, &lHandle);
                glBindTexture(GL_TEXTURE_2D, lHandle);
                //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glTexImage2D(GL_TEXTURE_2D, 0, mGLInternalFormat, mDesc.mWidth, mDesc.mHeight, 0, mGLFormat, mGLChannelSize, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glGenerateMipmap(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            gGLTextureMemoryAllocated += mDesc.GetBufferSize();

            mResRef.SetHandle(TGPUHandle(lHandle));
        }

    private:
        TTextureDesc const mDesc;
        TGPUResourceRef mResRef;
        GLenum mGLFormat;
        GLenum mGLChannelSize;
        GLint  mGLInternalFormat;
    };

    //-----------------------------------------------------------
    // CREATE TEXTURE
    //-----------------------------------------------------------
    class GLGenerateTextureMipMapCmd : public TGPUCommand
    {
    public:
        GLGenerateTextureMipMapCmd(TGPUResourceRef const & aResRef, TTextureDesc const & aDesc)
            : mResRef(aResRef)
            , mDesc(aDesc)
        {
        }

        ~GLGenerateTextureMipMapCmd()
        {
        }

        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            SBX_ASSERT((mDesc.mFlags & TTextureDesc::eFlag_Multisample) == 0, "Trying to generate mip maps from a multisample texture");
            glBindTexture(GL_TEXTURE_2D, lHandle);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            
        }

    private:
        TTextureDesc const mDesc;
        TGPUResourceRef mResRef;
    };

    //-----------------------------------------------------------
    // DESTROY TEXTURE
    //-----------------------------------------------------------
    class GLDestroyTextureCmd : public TGPUCommand
    {
    public:
        GLDestroyTextureCmd(TGPUResourceRef const & aResRef, TTextureDesc const & aDesc)
            : mResRef(aResRef)
            , mDesc(aDesc)
        {
        }

        ~GLDestroyTextureCmd()
        {
        }

        void Execute()
        {
            TGPUHandle lHandle =    mResRef.GetHandle();
            glDeleteTextures(1, &lHandle);
            gGLTextureMemoryAllocated -= mDesc.GetBufferSize();
        }

    private:
        TTextureDesc const mDesc;
        TGPUResourceRef mResRef;
    };

    //-----------------------------------------------------------
    // COPY TEXTURE TO BUFFER
    //-----------------------------------------------------------
    class GLCopyTextureToBufferCmd : public TGPUCommand
    {
    public:
        GLCopyTextureToBufferCmd(TGPUResourceRef const & aResRef, TTextureDesc const & aDesc, CRawBuffer::Ref aBufferRet)
            : mResRef(aResRef)
            , mDesc(aDesc)
            , mBufferRef(aBufferRet)
        {
            TTextureDesc::TTextureFormatSizeInfo lFormatSizeInfo;
            mDesc.GetFormatSizeInfo(lFormatSizeInfo);
            mGLFormat = kGLFormat[lFormatSizeInfo.mFormat];
            mGLChannelSize = kGLChannelSize[lFormatSizeInfo.mChannelSize];
        }

        ~GLCopyTextureToBufferCmd()
        {
        }

        void Execute()
        {

            TGPUHandle lHandle = mResRef.GetHandle();
            glBindTexture(GL_TEXTURE_2D, lHandle);
            glGetTexImage(GL_TEXTURE_2D, 0, mGLFormat, mGLChannelSize, mBufferRef->GetByteArray());
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    private:
        TTextureDesc const mDesc;
        TGPUResourceRef mResRef;
        CRawBuffer::Ref mBufferRef;
        GLenum mGLFormat;
        GLenum mGLChannelSize;
    };

    //-----------------------------------------------------------
    // COPY TEXTURE FROM BUFFER
    //-----------------------------------------------------------
    class GLCopyTextureFromBufferCmd : public TGPUCommand
    {
    public:
        GLCopyTextureFromBufferCmd(TGPUResourceRef const & aResRef, TTextureDesc const & aDesc, CRawBuffer::Ref const & aBufferRet)
            : mResRef(aResRef)
            , mDesc(aDesc)
            , mBufferRef(aBufferRet)
        {
            TTextureDesc::TTextureFormatSizeInfo lFormatSizeInfo;
            mDesc.GetFormatSizeInfo(lFormatSizeInfo);
            mGLFormat = kGLFormat[lFormatSizeInfo.mFormat];
            mGLChannelSize = kGLChannelSize[lFormatSizeInfo.mChannelSize];
            mGLInternalFormat = kGLInternalFormat[lFormatSizeInfo.mInternalFormat];
        }

        ~GLCopyTextureFromBufferCmd()
        {
        }

        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            glBindTexture(GL_TEXTURE_2D, lHandle);
            glTexImage2D(GL_TEXTURE_2D, 0, mGLInternalFormat, mDesc.mWidth, mDesc.mHeight, 0, mGLFormat, mGLChannelSize, mBufferRef->GetByteArray());
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    private:
        TGPUResourceRef mResRef;
        TTextureDesc const mDesc;
        CRawBuffer::Ref const mBufferRef;
        GLenum mGLFormat;
        GLenum mGLChannelSize;
        GLint mGLInternalFormat;
    };

    //-----------------------------------------------------------
    // CREATE CUBEMAP
    //-----------------------------------------------------------
    class GLCreateCubeMapTextureCmd : public TGPUCommand
    {
    public:
        GLCreateCubeMapTextureCmd(TGPUResourceRef const & aResRef)
            : mResRef(aResRef)
        {
        }

        ~GLCreateCubeMapTextureCmd()
        {
        }

        void Execute()
        {
            GLuint lHandle;
            glGenTextures(1, &lHandle);
            glBindTexture(GL_TEXTURE_CUBE_MAP, lHandle);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            mResRef.SetHandle(TGPUHandle(lHandle));
        }

    private:
        TGPUResourceRef mResRef;
    };

    
    //-----------------------------------------------------------
    // SETUP CUBEMAP
    //-----------------------------------------------------------
    class GLSetupCubeMapTextureCmd : public TGPUCommand
    {
    public:
        GLSetupCubeMapTextureCmd(TGPUResourceRef const & aResRef, TTexture::Ref aSides, uint32_t aMipLevel)
            : mResRef(aResRef)
            , mSides(aSides)
            , mMipLevel(aMipLevel)
        {
        }

        ~GLSetupCubeMapTextureCmd()
        {
        }

        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            glBindTexture(GL_TEXTURE_CUBE_MAP, lHandle);
    
            for(int32_t iSlice = 0; iSlice < mSides->mSlices; iSlice++)
            {
                TColor32F* lBuffer = mSides->AsRGBA32FBuffer() + (mSides->mWidth * mSides->mHeight * iSlice);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iSlice,
                             mMipLevel,
                             GL_RGBA32F,
                             mSides->mWidth,
                             mSides->mHeight,
                             0,
                             GL_RGBA,
                             GL_FLOAT,
                             lBuffer);
            }
    
            if(mMipLevel == 0)
            {
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
    
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    private:
        TGPUResourceRef mResRef;
        TTexture::Ref mSides;
        uint32_t mMipLevel;
    };
}}