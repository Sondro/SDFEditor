
#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

using CShaderCodeRef = std::shared_ptr<std::vector<char>>;
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
    CGPUShaderProgram(CShaderCodeRef aCode, EShaderSourceType aType, std::string const & aName);
    ~CGPUShaderProgram();
    uint32_t GetHandler() const { return mShaderProgramHandler; }

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