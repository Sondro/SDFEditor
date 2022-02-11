
#include "Renderer.h"
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

#include "SDFEditor/Utils/FileIO.h"
#include "SDFEditor/Tool/SceneData.h"

#include <sbx/Core/Log.h>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace EUniformLoc
{
    enum Type
    {
        uViewMatrix = 0,
        uProjectionMatrix = 1,
        uStrokesNum = 20,
        uMaxSlotsCount = 21,
        uVoxelSide = 22,
        uVolumeExtent = 23,

        uSdfLutTexture = 30,
        uSdfAtlasTexture = 31,

        // Debug
        uVoxelPreview = 40,
    };
};

namespace ETexBinding
{
    enum Type
    {
        uSdfLut = 1,
        uSdfAtlas = 2
    };
}

namespace EBlockBinding
{
    enum Type
    {
        strokes_buffer = 0,
        slot_list_buffer = 1,
        slot_count_buffer = 2,
    };
};

#define LUT_RES (128)
//#define 

void CRenderer::Init()
{
    glGenVertexArrays(1, &mDummyVAO);
   
    int workGroupSizes[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSizes[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSizes[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSizes[2]);
    int workGroupCounts[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

    SBX_LOG("workGroupSizes: (%d, %d, %d)", workGroupSizes[0], workGroupSizes[1], workGroupSizes[2]);
    SBX_LOG("workGroupCounts: (%d, %d, %d)", workGroupCounts[0], workGroupCounts[1], workGroupCounts[2]);

    // Strokes buffer
    mStrokesBuffer = std::make_shared<CGPUShaderStorageObject>(EGPUBufferBindTarget::SHADER_BUFFER_STORAGE);
    mStrokesBuffer->SetData(16 * sizeof(stroke_t), nullptr, EGPUBufferFlags::DYNAMIC_STORAGE);
    mStrokesBuffer->BindShaderStorage(EBlockBinding::strokes_buffer);

    // SDF Lut texture
    TGPUTextureConfig lSdfLutConfig;
    lSdfLutConfig.mTarget = ETexTarget::TEXTURE_3D;
    lSdfLutConfig.mExtentX = LUT_RES;
    lSdfLutConfig.mExtentY = LUT_RES;
    lSdfLutConfig.mSlices = LUT_RES;
    lSdfLutConfig.mFormat = ETexFormat::RGBA8;
    lSdfLutConfig.mMinFilter = ETexFilter::LINEAR;
    lSdfLutConfig.mMagFilter = ETexFilter::LINEAR;
    lSdfLutConfig.mWrapS = ETexWrap::CLAMP_TO_EDGE;
    lSdfLutConfig.mWrapT = ETexWrap::CLAMP_TO_EDGE;
    lSdfLutConfig.mWrapR = ETexWrap::CLAMP_TO_EDGE;
    lSdfLutConfig.mMips = 1;
    mSdfLut = std::make_shared<CGPUTexture>(lSdfLutConfig);

    // SDF Atlas buffer
    TGPUTextureConfig lSdfAtlasConfig;
    lSdfAtlasConfig.mTarget = ETexTarget::TEXTURE_3D;
    lSdfAtlasConfig.mExtentX = 1024;
    lSdfAtlasConfig.mExtentY = 1024;
    lSdfAtlasConfig.mSlices = 256;
    lSdfAtlasConfig.mFormat = ETexFormat::R8;
    lSdfAtlasConfig.mMinFilter = ETexFilter::NEAREST;
    lSdfAtlasConfig.mMagFilter = ETexFilter::NEAREST;
    lSdfAtlasConfig.mWrapS = ETexWrap::CLAMP_TO_EDGE;
    lSdfAtlasConfig.mWrapT = ETexWrap::CLAMP_TO_EDGE;
    lSdfAtlasConfig.mWrapR = ETexWrap::CLAMP_TO_EDGE;
    lSdfAtlasConfig.mMips = 1;
    mSdfAtlas = std::make_shared<CGPUTexture>(lSdfAtlasConfig);

    // Slot list buffer
    mSlotListBuffer = std::make_shared<CGPUShaderStorageObject>(EGPUBufferBindTarget::SHADER_BUFFER_STORAGE);
    size_t lListSize = size_t(lSdfLutConfig.mExtentX) * 
                       size_t(lSdfLutConfig.mExtentY) * 
                       size_t(lSdfLutConfig.mSlices) * 
                       sizeof(uint32_t);
    mSlotListBuffer->SetData(lListSize, nullptr, EGPUBufferFlags::DYNAMIC_STORAGE);
    mSlotListBuffer->BindShaderStorage(EBlockBinding::slot_list_buffer);

    // Atomic Counter buffer
    mSlotCounterBuffer = std::make_shared<CGPUShaderStorageObject>(EGPUBufferBindTarget::SHADER_BUFFER_STORAGE);
    mSlotCounterBuffer->SetData(sizeof(uint32_t) * 3, nullptr, EGPUBufferFlags::DYNAMIC_STORAGE);
    mSlotCounterBuffer->BindShaderStorage(EBlockBinding::slot_count_buffer);
}

void CRenderer::Shutdown()
{
    glDeleteVertexArrays(1, &mDummyVAO);
}

void CRenderer::ReloadShaders()
{
    SBX_LOG("Loading shaders...");

    // Shared SDF code
    CShaderCodeRef lSdfCommonCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/SdfCommon.h.glsl")));
    
    // Compute lut shader program
    {
        CShaderCodeRef lComputeLutCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/ComputeSdfLut.comp.glsl")));
        mComputeLutProgram = std::make_shared<CGPUShaderProgram>(CShaderCodeRefList{ lSdfCommonCode, lComputeLutCode }, EShaderSourceType::COMPUTE_SHADER, "ComputeLut");
        mComputeLutPipeline = std::make_shared<CGPUShaderPipeline>(std::vector<CGPUShaderProgramRef>{ mComputeLutProgram });
    }

    // Compute atlas shader program
    {
        CShaderCodeRef lComputeAtlasCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/ComputeSdfAtlas.comp.glsl")));
        mComputeAtlasProgram = std::make_shared<CGPUShaderProgram>(CShaderCodeRefList{ lSdfCommonCode, lComputeAtlasCode }, EShaderSourceType::COMPUTE_SHADER, "ComputeAtlas");
        mComputeAtlasPipeline = std::make_shared<CGPUShaderPipeline>(std::vector<CGPUShaderProgramRef>{ mComputeAtlasProgram });
    }

    // Draw on screen shader program
    {
        CShaderCodeRef lScreenQuadVSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/FullScreenTrinagle.vert.glsl")));
        CShaderCodeRef lColorFSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/Color.frag.glsl")));
        mFullscreenVertexProgram = std::make_shared<CGPUShaderProgram>(CShaderCodeRefList{ lScreenQuadVSCode }, EShaderSourceType::VERTEX_SHADER, "ScreenQuadVS");
        mColorFragmentProgram = std::make_shared<CGPUShaderProgram>(CShaderCodeRefList{ lSdfCommonCode, lColorFSCode }, EShaderSourceType::FRAGMENT_SHADER, "BaseFragmentFS");
        std::vector<CGPUShaderProgramRef> lPrograms = { mFullscreenVertexProgram, mColorFragmentProgram };
        mScreenQuadPipeline = std::make_shared<CGPUShaderPipeline>(lPrograms);
    }


    // Static uniforms
    const std::vector<uint32_t> lProgramHandlers
    {
        mComputeLutProgram->GetHandler(),
        mComputeAtlasProgram->GetHandler(),
        mColorFragmentProgram->GetHandler()
    };

    for (uint32_t lHandler : lProgramHandlers)
    {
        glProgramUniform1ui(lHandler, EUniformLoc::uMaxSlotsCount, 491520);
        float lVoxelExt = 0.05f;
        float lLutSize = float(LUT_RES);
        glProgramUniform4f(lHandler, EUniformLoc::uVoxelSide, lVoxelExt, 1.0f / lVoxelExt, lVoxelExt / 8.0f, 1.0f / (lVoxelExt / 8.0f));
        glProgramUniform4f(lHandler, EUniformLoc::uVolumeExtent, lLutSize, 1.0f / lLutSize, 0.0f, 0.0f);
        glProgramUniform1i(lHandler, EUniformLoc::uSdfLutTexture, ETexBinding::uSdfLut);
        glProgramUniform1i(lHandler, EUniformLoc::uSdfAtlasTexture, ETexBinding::uSdfAtlas);
    }
}

void CRenderer::UpdateSceneData(CScene const& aScene)
{
    if (aScene.IsDirty())
    {
        size_t lSizeBytes = aScene.mStorkesArray.size() * sizeof(stroke_t);

        if (lSizeBytes > mStrokesBuffer->GetStorageSize())
        {
            mStrokesBuffer = std::make_shared<CGPUShaderStorageObject>(EGPUBufferBindTarget::SHADER_BUFFER_STORAGE);
            mStrokesBuffer->SetData(lSizeBytes + (16 * sizeof(stroke_t)), nullptr, EGPUBufferFlags::DYNAMIC_STORAGE);
            //mStrokesBuffer->BindToProgram(mColorFragmentProgram->GetHandler(), 0, "strokes_buffer");
            mStrokesBuffer->BindShaderStorage(EBlockBinding::strokes_buffer);
        }

        //stroke_t* lStrokesBufferMappedMemory = (stroke_t*)mStrokesBuffer->Map();
        for (size_t i = 0; i < aScene.mStorkesArray.size(); i++)
        {
            //::memcpy(lStrokesBufferMappedMemory + i, (void*)&aScene.mStorkesArray[i], sizeof(stroke_t));
            mStrokesBuffer->UpdateSubData(sizeof(stroke_t) * i, sizeof(stroke_t), (void*)&aScene.mStorkesArray[i]);
        }
        //mStrokesBuffer->Unmap();

        const std::vector<uint32_t> lProgramHandlers
        {
            mComputeLutProgram->GetHandler(),
            mComputeAtlasProgram->GetHandler(),
            mColorFragmentProgram->GetHandler()
        };

        for (uint32_t lHandler : lProgramHandlers)
        {
            glProgramUniform1ui(lHandler, EUniformLoc::uStrokesNum, aScene.mStorkesArray.size() & 0xFFFFFFFF);
            //glProgramUniform1ui(mComputeLutProgram->GetHandler(), EUniformLoc::uStrokesNum, aScene.mStorkesArray.size() & 0xFFFFFFFF);
            //glProgramUniform1ui(mComputeAtlasProgram->GetHandler(), EUniformLoc::uStrokesNum, aScene.mStorkesArray.size() & 0xFFFFFFFF);
            //glProgramUniform1ui(mColorFragmentProgram->GetHandler(), EUniformLoc::uStrokesNum, aScene.mStorkesArray.size() & 0xFFFFFFFF);
        }

        // clear slot count
        const static uint32_t sZero[] = { 0, 1, 1 };
        mSlotCounterBuffer->UpdateSubData(0, sizeof(uint32_t) * 3, (void*)sZero);

        // Execute compute lut
        mComputeLutPipeline->Bind();
        mSdfLut->BindImage(0, 0, EImgAccess::WRITE_ONLY);
        const uint32_t dispatchRes = LUT_RES / 8;
        glDispatchCompute(dispatchRes, dispatchRes, dispatchRes);
        
        // Execute compute atlas
        mComputeAtlasPipeline->Bind();
        mSlotCounterBuffer->BindTarget(EGPUBufferBindTarget::DISPATCH_INDIRECT_BUFFER);
        mSdfAtlas->BindImage(0, 0, EImgAccess::WRITE_ONLY);
        glDispatchComputeIndirect(0);
        mSlotCounterBuffer->UnbindTarget(EGPUBufferBindTarget::DISPATCH_INDIRECT_BUFFER);
    }

    //Update Matrix
    glm::mat4 lProjection = aScene.mCamera.GetProjectionMatrix(); //glm::perspective(aScene.mCamera.mFOV, aScene.mCamera.mAspect, 0.1f, 100.0f);
    glm::mat4 lView = aScene.mCamera.GetViewMatrix(); //glm::lookAt(aScene.mCamera.mOrigin, aScene.mCamera.mLookAt, aScene.mCamera.mViewUp);
    //lProjection[1][1] *= -1; // Remember to do this in Vulkan
    //glm::mat4 lViewProjection = lProjection * lView;
    //glm::mat4 lInverseViewProjection = glm::inverse(lViewProjection);

    // Update program data
    //glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), 0, 1, false, glm::value_ptr(lInverseViewProjection));
    glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), EUniformLoc::uViewMatrix, 1, false, glm::value_ptr(lView));
    glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), EUniformLoc::uProjectionMatrix, 1, false, glm::value_ptr(lProjection));
    glProgramUniform4i(mColorFragmentProgram->GetHandler(), EUniformLoc::uVoxelPreview, aScene.mUseVoxels ? 1 : 0, aScene.mPreviewSlice, 0, 0);
}

void CRenderer::RenderFrame()
{
    glfwGetFramebufferSize(glfwGetCurrentContext(), &mViewWidth, &mViewHeight);

    glViewport(0, 0, mViewWidth, mViewHeight);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Draw full screen quad
    glBindVertexArray(mDummyVAO);
    mScreenQuadPipeline->Bind();
    mSdfLut->BindTexture(ETexBinding::uSdfLut);
    mSdfAtlas->BindTexture(ETexBinding::uSdfAtlas);

    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glBindVertexArray(0);
}