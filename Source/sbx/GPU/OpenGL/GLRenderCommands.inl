/*
 * @file    GLRenderCommands.inl
 * @author  David Gallardo Moreno
 */

namespace sbx { namespace gl 
{
    //-----------------------------------------------------------º
    // SETUP RENDER TARGET
    //-----------------------------------------------------------
    class GLCreateRenderTargetCmd : public TGPUCommand
    {
    public:
        GLCreateRenderTargetCmd(TGPUResourceRef const & aResRef, TGPURenderTargetDesc::Ref aDesc)
            : mResRef(aResRef)
            , mDesc(aDesc)
        {
        }

        void Execute()
        {
            TGPUHandle lHandle;
            glGenFramebuffers(1, &lHandle);
            mResRef.SetHandle(lHandle);
            //TGPUHandle lHandle = mResRef.GetHandle();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lHandle);
            
            uint32_t lNumAttahcments = 0;
            for(int i = 0; i < mDesc->mColorAttachments.size(); ++i)
            {   
                CGPUTexture::Ref lColorAttachment = mDesc->mColorAttachments[i];
                if(lColorAttachment)
                {
                    GLenum lTarget = (lColorAttachment->GetDesc().mFlags & TTextureDesc::eFlag_Multisample) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                    TGPUHandle lAttachHandle = lColorAttachment->GetRes().GetHandle();
                    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                                           GL_COLOR_ATTACHMENT0 + lNumAttahcments,
                                           lTarget,
                                           lAttachHandle,
                                           0);

                    lNumAttahcments++;
                }
            }
    
            if(mDesc->mDepthAttachment)
            {
                GLenum lTarget = (mDesc->mDepthAttachment->GetDesc().mFlags & TTextureDesc::eFlag_Multisample) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                TGPUHandle lAttachHandle = mDesc->mDepthAttachment->GetRes().GetHandle();
                glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                                       GL_DEPTH_ATTACHMENT,
                                       lTarget,
                                       lAttachHandle,
                                       0);
            }
    
            static const GLenum sDrawBuffers[] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3,
                GL_COLOR_ATTACHMENT4,
                GL_COLOR_ATTACHMENT5,
                GL_COLOR_ATTACHMENT6,
                GL_COLOR_ATTACHMENT7,
                GL_COLOR_ATTACHMENT8,
                GL_COLOR_ATTACHMENT9
            };
    
            glDrawBuffers(lNumAttahcments, sDrawBuffers);
    
            GLenum lCheckResult = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

            switch(lCheckResult)
            {
            case GL_FRAMEBUFFER_COMPLETE:
                //All ok
                break;
            default:
                SBX_LOG("[GLSetupRenderTargetCmd::Execute] Framebuffer [%u] not properly setup!", lHandle);
                break;
            }

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
    private:
        TGPUResourceRef mResRef;
        TGPURenderTargetDesc::Ref const mDesc;
    };

    //-----------------------------------------------------------
    // DESTROY RENDER TARGET
    //-----------------------------------------------------------
    class GLDestroyRenderTargetCmd : public TGPUCommand
    {
    public:
        GLDestroyRenderTargetCmd(TGPUResourceRef const & aResRef)
            : mResRef(aResRef)
        {
        }
    
        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            glDeleteFramebuffers(1, &lHandle);
        }
    private:
        TGPUResourceRef mResRef;
    };

    //-----------------------------------------------------------
    // CLEAR RENDER TARGET
    //-----------------------------------------------------------
    class GLClearRenderTargetCmd : public TGPUCommand
    {
    public:
        GLClearRenderTargetCmd(TGPUResourceRef const & aResRef, uint32_t aAttachments, TColor32F const & aColor)
            : mResRef(aResRef)
            , mAttachments(aAttachments)
            , mColor(aColor)
        {
        }
    
        void Execute()
        {
            TGPUHandle lHandle = mResRef.GetHandle();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lHandle);
            for(uint32_t i = 0; i < mAttachments; ++i)
            {
                glClearBufferfv(GL_COLOR, i, mColor.mBuffer);
            }
            float lOne = 1.0f;
            glClearBufferfv(GL_DEPTH, 0, &lOne);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }
    private:
        TGPUResourceRef mResRef;
        uint32_t        mAttachments;
        TColor32F       mColor;

    };

    //-----------------------------------------------------------
    // BLIT RENDER TARGET
    //-----------------------------------------------------------
    class GLBlitRenderTargetCmd : public TGPUCommand
    {
    public:
        GLBlitRenderTargetCmd(TGPUResourceRef const & aInputRT, TGPUResourceRef const & aOutputRT, TBlitInfo const & aBlitInfo)
            : mInputRTResRef(aInputRT)
            , mOutputRTResRef(aOutputRT)
            , mBlitInfo(aBlitInfo)
        {
        }

        void Execute()
        {
            //Set Render Target (framebuffer)
            TGPUHandle lInputRTHandle = mInputRTResRef.GetHandle();
            TGPUHandle lOutputRTHandle = mOutputRTResRef.GetHandle();
            // now resolve multisampled buffer(s) into intermediate FBO
            glBindFramebuffer(GL_READ_FRAMEBUFFER, lInputRTHandle);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lOutputRTHandle);
            for(uint32_t i = 0; i < mBlitInfo.mColorAttachments; ++i)
            {
                glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
                glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
                glBlitFramebuffer(  mBlitInfo.mSrcX0, mBlitInfo.mSrcY0, mBlitInfo.mSrcX1, mBlitInfo.mSrcX1, 
                                    mBlitInfo.mDstX0, mBlitInfo.mDstY0, mBlitInfo.mDstX1, mBlitInfo.mDstY1, 
                                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST); //TODO: this can go in the blit info 
            }
            // now scene is stored as 2D texture image, so use that image for post-processing
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    private:
        TGPUResourceRef mInputRTResRef;
        TGPUResourceRef mOutputRTResRef;
        TBlitInfo       mBlitInfo;
    };

    //-----------------------------------------------------------
    // CREATE MESH OBJECT
    //-----------------------------------------------------------
    class GLCreateMeshObjectCmd : public TGPUCommand
    {
    public:
        GLCreateMeshObjectCmd(  TGPUResourceRef const & aVAORes, 
                                TGPUResourceRef const & aVBORes, 
                                TGPUMeshDesc::Ref const & aMeshDesc, 
                                CRawBuffer::Ref const & aVertexList)
            : mVAOResRef(aVAORes)
            , mVBOResRef(aVBORes)
            , mMeshDesc(aMeshDesc)
            , mVertexList(aVertexList)
        {

        }

        void Execute()
        {
            TGPUHandle lVAOHandle, lVBOHandle;
            uint32_t lVertexCount = 0;

            glGenVertexArrays(1, &lVAOHandle);
            glBindVertexArray(lVAOHandle);
            glGenBuffers(1, &lVBOHandle);
            glBindBuffer(GL_ARRAY_BUFFER, lVBOHandle);
            glBufferData(GL_ARRAY_BUFFER, mVertexList->GetSize(), mVertexList->GetByteArray(), GL_STATIC_DRAW);
    
            for(TGPUVertexAttribDesc const & lAttrib : mMeshDesc->mVertexAttributesList.mList)
            {
                glVertexAttribPointer(  lAttrib.mIndex, 
                                        lAttrib.mComponents, 
                                        kGLVertexAttribType[static_cast< uint8_t >(lAttrib.mType)], 
                                        GL_FALSE, 
                                        static_cast< GLsizei >(mMeshDesc->mVertexAttributesList.mVertexSize), 
                                        reinterpret_cast< GLvoid* >(uintptr_t(lAttrib.mOffset)));
                glEnableVertexAttribArray(lAttrib.mIndex);
            }
            
            mVAOResRef.SetHandle(lVAOHandle);
            mVBOResRef.SetHandle(lVBOHandle);

            glBindVertexArray(0);
        }
    private:
        TGPUResourceRef mVAOResRef;
        TGPUResourceRef mVBOResRef;
        TGPUMeshDesc::Ref mMeshDesc; 
        CRawBuffer::Ref mVertexList; 
    };

    //-----------------------------------------------------------
    // DESTORY MESH OBJECT
    //-----------------------------------------------------------
    class GLDestroyMeshObjectCmd : public TGPUCommand
    {
    public:
        GLDestroyMeshObjectCmd( TGPUResourceRef const & aVAORes, 
                                TGPUResourceRef const & aVBORes)
            : mVAOResRef(aVAORes)
            , mVBOResRef(aVBORes)
        {

        }

        void Execute()
        {
            TGPUHandle lVBOHandle = mVBOResRef.GetHandle();
            TGPUHandle lVAOHandle = mVAOResRef.GetHandle();
            glDeleteBuffers(1, &lVBOHandle);
            glDeleteVertexArrays(1, &lVAOHandle);
        }
    private:
        TGPUResourceRef mVAOResRef;
        TGPUResourceRef mVBOResRef;
    };

    //-----------------------------------------------------------
    // RENDER MESH OBJECT
    //-----------------------------------------------------------
    class GLRenderMeshObjectCmd : public TGPUCommand
    {
    public:
        GLRenderMeshObjectCmd(TGPURenderMeshData::Ref const & aRenderMeshData)
            : mRenderMeshData(aRenderMeshData)
        {
        }

        void Execute()
        {
            //Set Render Target (framebuffer)
            TGPUHandle lFramebufferHandle = mRenderMeshData->mRenderTargetRes.GetHandle();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lFramebufferHandle);
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            glViewport(0, 0, mRenderMeshData->mRenderTargetDesc->mViewportWidth, mRenderMeshData->mRenderTargetDesc->mViewportHeight);

            //Set Program and uniforms
            TGPUHandle lProgramHandle = mRenderMeshData->mProgramRes.GetHandle();
            glUseProgram(lProgramHandle);
            {
                uint32_t lNextActiveTexture = 0;
                uint32_t lNextCubeActiveTexture = 20;
                GLShaderUniformsSetup(mRenderMeshData->mProgramData.GetPtr(), lNextActiveTexture, lNextCubeActiveTexture);
            }

            //Set VAO to draw
            TGPUHandle lVAOHandle = mRenderMeshData->mMeshRes.GetHandle();
            glBindVertexArray(lVAOHandle);

            //Draw
            //glDrawArrays(GL_TRIANGLES, 0, 6);

            //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); 
            //glEnable(GL_CULL_FACE);
            //glCullFace(GL_FRONT);

            glDrawArrays(GL_TRIANGLES, 0, static_cast< GLsizei >(mRenderMeshData->mMeshDesc->mNumVertex * 3));


            //glDrawArrays(GL_PATCHES, 0, static_cast< GLsizei >(mRenderMeshData->mMeshDesc->mNumVertex * 3));

            //Reset VAO
            glBindVertexArray(0);

            //Clean up uniforms & reset Program
            {
                uint32_t lNextActiveTexture = 0;
                uint32_t lNextCubeActiveTexture = 20;
                GLShaderUniformsCleanup(mRenderMeshData->mProgramData.GetPtr(), lNextActiveTexture, lNextCubeActiveTexture);
            }
            glUseProgram(0);

            //Reset Render Target (framebuffer)
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);           
        }
    private:
        TGPURenderMeshData::Ref mRenderMeshData;
    };


}} // sbx::gl