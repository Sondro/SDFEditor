// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "sbx/Core/ErrorHandling.h"

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

CGPUShaderProgram::CGPUShaderProgram(CShaderCodeRefList const & aCode, EShaderSourceType aType, std::string const& aName)
    : mShaderProgramHandler(UINT32_MAX)
    , mType(aType)
    , mName(aName)
{
    std::vector <char*> lCodeStrings;
    lCodeStrings.push_back("#version 450\nprecision mediump float;\n");
    for (auto& lStr : aCode)
    {
        lCodeStrings.push_back(lStr->data());
    }

    mShaderProgramHandler = glCreateShaderProgramv(sShaderTypes[(uint32_t)aType], (GLsizei)lCodeStrings.size(), lCodeStrings.data());

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


uint32_t CGPUShaderProgram::GetStorageBlockIndex(const char* aBlockName) const
{
    return glGetProgramResourceIndex(mShaderProgramHandler, GL_SHADER_STORAGE_BLOCK, aBlockName);
}

uint32_t CGPUShaderProgram::GetUniformBlockIndex(const char* aBlockName) const
{
    return glGetProgramResourceIndex(mShaderProgramHandler, GL_UNIFORM_BLOCK, aBlockName);
}

void CGPUShaderProgram::StorageBlockBinding(const char* aBlockName, uint32_t aBlockBinding) const
{
    uint32_t lBlockIndex = GetStorageBlockIndex(aBlockName);
    StorageBlockBinding(lBlockIndex, aBlockBinding);
}

void CGPUShaderProgram::StorageBlockBinding(uint32_t aBlockIndex, uint32_t aBlockBinding) const
{
    glShaderStorageBlockBinding(mShaderProgramHandler, aBlockIndex, aBlockBinding);
}

void CGPUShaderProgram::UniformBlockBinding(const char* aBlockName, uint32_t aBlockBinding) const
{
    uint32_t lBlockIndex = GetUniformBlockIndex(aBlockName);
    UniformBlockBinding(lBlockIndex, aBlockBinding);
}

void CGPUShaderProgram::UniformBlockBinding(uint32_t aBlockIndex, uint32_t aBlockBinding) const
{
    glUniformBlockBinding(mShaderProgramHandler, aBlockIndex, aBlockBinding);
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


