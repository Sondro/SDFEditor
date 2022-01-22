
#include "Renderer.h"
#include <ThirdParty/glad/glad.h>
#include "SDFEditor/Utils/ReadFile.h"

#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void CRenderer::Init()
{
    glGenVertexArrays(1, &mDummyVAO);
}

void CRenderer::Shutdown()
{
    glDeleteVertexArrays(1, &mDummyVAO);
}

void CRenderer::ReloadShaders()
{
    CShaderCodeRef lScreenQuadVSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/FullScreenTrinagle.vert.glsl")));
    CShaderCodeRef lColorFSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/Color.frag.glsl")));

    mFullscreenVertexProgram = std::make_shared<CGPUShaderProgram>(lScreenQuadVSCode, EShaderSourceType::VERTEX_SHADER, "ScreenQuadVS");
    mColorFragmentProgram = std::make_shared<CGPUShaderProgram>(lColorFSCode, EShaderSourceType::FRAGMENT_SHADER, "BaseFragmentFS");

    std::vector<CGPUShaderProgramRef> lPrograms = { mFullscreenVertexProgram, mColorFragmentProgram };

    mScreenQuadPipeline = std::make_shared<CGPUShaderPipeline>(lPrograms);
}

void CRenderer::UpdateViewData(uint32_t aWidth, uint32_t aHeight)
{
    mViewWidth = aWidth;
    mViewHeight = aHeight;
}

void CRenderer::RenderFrame()
{
    //Update Matrix
    glm::mat4 lProjection = glm::perspective(glm::radians(45.0f), (float)mViewWidth / (float)mViewHeight, 0.1f, 100.0f);
    glm::mat4 lView = glm::lookAt(glm::vec3(0.0f, -1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //lProjection[1][1] *= -1; // Invert glm convention for y-up, as in Vulkan is the opposite than in OpenGL
    glm::mat4 lViewProjection = lProjection * lView;
    glm::mat4 lInverseViewProjection = glm::inverse(lViewProjection);

    // Update program data
    glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), 0, 1, false, glm::value_ptr(lInverseViewProjection));

    //Draw full screen quad
    glBindVertexArray(mDummyVAO);
    mScreenQuadPipeline->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    mScreenQuadPipeline->Unbind();
    glBindVertexArray(0);
}