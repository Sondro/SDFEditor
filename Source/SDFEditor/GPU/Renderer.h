// Copyright (c) 2022 David Gallardo and SDFEditor Project

#pragma once

#include <cstdint>
#include "SDFEditor/GPU/GPUShader.h"
#include "SDFEditor/GPU/GPUStorageBuffer.h"
#include "SDFEditor/GPU/GPUTexture.h"

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
    uint32_t mDummyVAO;
    CGPUShaderProgramRef mFullscreenVertexProgram;
    CGPUShaderProgramRef mColorFragmentProgram;
    CGPUShaderPipelineRef mScreenQuadPipeline;

    CGPUShaderProgramRef mComputeLutProgram;
    CGPUShaderPipelineRef mComputeLutPipeline;
    CGPUShaderProgramRef mComputeAtlasProgram;
    CGPUShaderPipelineRef mComputeAtlasPipeline;
    CGPUTextureRef mSdfLut;
    CGPUTextureRef mSdfAtlas;

    CGPUShaderStorageObjectRef mStrokesBuffer;
    CGPUShaderStorageObjectRef mSlotListBuffer;
    CGPUShaderStorageObjectRef mSlotCounterBuffer;
};