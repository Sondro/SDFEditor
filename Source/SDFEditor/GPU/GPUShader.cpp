
#include "sbx/Common.h"

#include "GPUShader.h"

#include "ThirdParty/glad/glad.h"
#include <iostream>

GLenum sShaderTypes[] =
{
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_COMPUTE_SHADER
};

GLenum sShaderStages[] =
{
    GL_VERTEX_SHADER_BIT,
    GL_FRAGMENT_SHADER_BIT,
    GL_COMPUTE_SHADER_BIT
};

CGPUShaderProgram::CGPUShaderProgram(CShaderCodeRef aCode, EShaderSourceType aType, std::string const& aName)
    : mShaderProgramHandler(UINT32_MAX)
    , mType(aType)
    , mName(aName)
{
    const char* lCode = aCode->data();
    mShaderProgramHandler = glCreateShaderProgramv(sShaderTypes[(uint32_t)aType], 1, &lCode);

    GLint status;
    glGetProgramiv(mShaderProgramHandler, GL_LINK_STATUS, &status);

    if (status != GL_TRUE)
    {
        GLchar  log[1024] = { 0 };
        glGetProgramInfoLog(mShaderProgramHandler, 1024, NULL, log);
        SBX_ERROR("ERROR compiling/linking shader [%s] :\n%s", mName.c_str(), log);
    }
}

CGPUShaderProgram::~CGPUShaderProgram()
{
    glDeleteProgram(mShaderProgramHandler);
    mShaderProgramHandler = UINT32_MAX;
}

CGPUShaderPipeline::CGPUShaderPipeline(std::vector<CGPUShaderProgramRef> const& aPrograms)
    : mPrograms(aPrograms)
    , mShaderPipelineHandler(UINT32_MAX)
{
    glGenProgramPipelines(1, &mShaderPipelineHandler);

    for (auto const& lProgram : mPrograms)
    {
        glUseProgramStages(mShaderPipelineHandler, sShaderStages[(uint32_t)lProgram->mType], lProgram->mShaderProgramHandler);
    }
}

CGPUShaderPipeline::~CGPUShaderPipeline()
{
    glDeleteProgramPipelines(1, &mShaderPipelineHandler);
    mShaderPipelineHandler = UINT32_MAX;
}

void CGPUShaderPipeline::Bind() const
{
    glBindProgramPipeline(mShaderPipelineHandler);
}

void CGPUShaderPipeline::Unbind() const
{
//    glBindProgramPipeline(0);
}
