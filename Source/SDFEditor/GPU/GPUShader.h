// Copyright (c) 2022 David Gallardo and SDFEditor Project

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

using CShaderCodeRef = std::shared_ptr<std::vector<char>>;
using CShaderCodeRefList = std::vector<CShaderCodeRef>;
using CGPUShaderProgramRef = std::shared_ptr<class CGPUShaderProgram>;
using CGPUShaderPipelineRef = std::shared_ptr<class CGPUShaderPipeline>;

enum class EShaderSourceType
{
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    COMPUTE_SHADER
};

class CGPUShaderProgram
{
public:
    CGPUShaderProgram(CShaderCodeRefList const & aCode, EShaderSourceType aType, std::string const & aName);
    ~CGPUShaderProgram();
    uint32_t GetHandler() const { return mShaderProgramHandler; }
    uint32_t GetStorageBlockIndex(const char* aBlockName) const;
    uint32_t GetUniformBlockIndex(const char* aBlockName) const;
    void StorageBlockBinding(const char* aBlockName, uint32_t aBlockBinding) const;
    void StorageBlockBinding(uint32_t aBlockIndex, uint32_t aBlockBinding) const;
    void UniformBlockBinding(const char* aBlockName, uint32_t aBlockBinding) const;
    void UniformBlockBinding(uint32_t aBlockIndex, uint32_t aBlockBinding) const;

private:
    
    uint32_t mShaderProgramHandler;
    EShaderSourceType mType;
    std::string mName;

    friend class CGPUShaderPipeline;
};

class CGPUShaderPipeline
{
public:
    CGPUShaderPipeline(std::vector<CGPUShaderProgramRef> const & aPrograms);
    ~CGPUShaderPipeline();
    uint32_t GetHandler() const { return mShaderPipelineHandler; };
    void Bind() const;
    void Unbind() const;

private:

    std::vector<CGPUShaderProgramRef> mPrograms;
    uint32_t mShaderPipelineHandler;
};