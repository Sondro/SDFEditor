
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
    };
};

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

    mStrokesBuffer = std::make_shared<CGPUShaderStorageObject>();
    mStrokesBuffer->SetData(16 * sizeof(stroke_t), nullptr, EGPUBufferFlags::MAP_WRITE_BIT | EGPUBufferFlags::DYNAMIC_STORAGE);
}

void CRenderer::Shutdown()
{
    glDeleteVertexArrays(1, &mDummyVAO);
}

void CRenderer::ReloadShaders()
{
    SBX_LOG("Loading shaders...");

    CShaderCodeRef lSDFCommonCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/SDFCommon.h.glsl")));
    CShaderCodeRef lScreenQuadVSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/FullScreenTrinagle.vert.glsl")));
    CShaderCodeRef lColorFSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/Color.frag.glsl")));

    mFullscreenVertexProgram = std::make_shared<CGPUShaderProgram>(CShaderCodeRefList{ lScreenQuadVSCode }, EShaderSourceType::VERTEX_SHADER, "ScreenQuadVS");
    mColorFragmentProgram = std::make_shared<CGPUShaderProgram>(CShaderCodeRefList{ lSDFCommonCode, lColorFSCode }, EShaderSourceType::FRAGMENT_SHADER, "BaseFragmentFS");

    std::vector<CGPUShaderProgramRef> lPrograms = { mFullscreenVertexProgram, mColorFragmentProgram };

    mScreenQuadPipeline = std::make_shared<CGPUShaderPipeline>(lPrograms);

    mStrokesBuffer->BindToProgram(mColorFragmentProgram->GetHandler(), 0, "strokes_buffer");
}

void CRenderer::UpdateSceneData(CScene const& aScene)
{
    if (aScene.IsDirty())
    {
        size_t lSizeBytes = aScene.mStorkesArray.size() * sizeof(stroke_t);

        if (lSizeBytes > mStrokesBuffer->GetStorageSize())
        {
            mStrokesBuffer = std::make_shared<CGPUShaderStorageObject>();
            mStrokesBuffer->SetData(lSizeBytes + (16 * sizeof(stroke_t)), nullptr, EGPUBufferFlags::MAP_WRITE_BIT | EGPUBufferFlags::DYNAMIC_STORAGE);
            mStrokesBuffer->BindToProgram(mColorFragmentProgram->GetHandler(), 0, "strokes_buffer");
        }

        stroke_t* lStrokesBufferMappedMemory = (stroke_t*)mStrokesBuffer->Map();
        for (size_t i = 0; i < aScene.mStorkesArray.size(); i++)
        {
            ::memcpy(lStrokesBufferMappedMemory + i, (void*)&aScene.mStorkesArray[i], sizeof(stroke_t));
        }
        mStrokesBuffer->Unmap();
        

        glProgramUniform1ui(mColorFragmentProgram->GetHandler(), EUniformLoc::uStrokesNum, aScene.mStorkesArray.size() & 0xFFFFFFFF);
    }

    //Update Matrix
    glm::mat4 lProjection = aScene.mCamera.GetProjectionMatrix(); //glm::perspective(aScene.mCamera.mFOV, aScene.mCamera.mAspect, 0.1f, 100.0f);
    glm::mat4 lView = aScene.mCamera.GetViewMatrix(); //glm::lookAt(aScene.mCamera.mOrigin, aScene.mCamera.mLookAt, aScene.mCamera.mViewUp);
    //lProjection[1][1] *= -1; // Invert glm convention for y-up, as in Vulkan is the opposite than in OpenGL
    //glm::mat4 lViewProjection = lProjection * lView;
    //glm::mat4 lInverseViewProjection = glm::inverse(lViewProjection);

    // Update program data
    //glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), 0, 1, false, glm::value_ptr(lInverseViewProjection));
    glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), EUniformLoc::uViewMatrix, 1, false, glm::value_ptr(lView));
    glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), EUniformLoc::uProjectionMatrix, 1, false, glm::value_ptr(lProjection));
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

    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glBindVertexArray(0);
}