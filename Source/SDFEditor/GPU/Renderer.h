
#pragma once

#include <cstdint>
#include "SDFEditor/GPU/GPUShader.h"
#include "SDFEditor/GPU/GPUStorageBuffer.h"

#include <glm/glm.hpp>

class CRenderer
{
public:
    void Init();
    void Shutdown();
    void ReloadShaders();
    void UpdateSceneData(class CScene const& aScene);
    void RenderFrame();

    CGPUShaderStorageObjectRef GetStrokesBufferRef() { return mStrokesBuffer; }

private:
    // View data
    int32_t mViewWidth;
    int32_t mViewHeight;

    // Render data
    CGPUShaderProgramRef mFullscreenVertexProgram;
    CGPUShaderProgramRef mColorFragmentProgram;
    CGPUShaderPipelineRef mScreenQuadPipeline;
    uint32_t mDummyVAO;

    CGPUShaderStorageObjectRef mStrokesBuffer;    
};