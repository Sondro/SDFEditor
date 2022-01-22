
#pragma once

#include <cstdint>
#include "SDFEditor/GPU/GPUShader.h"

struct TCamera
{

};

class CRenderer
{
public:
    void Init();
    void Shutdown();
    void ReloadShaders();
    void UpdateViewData(uint32_t aWidth, uint32_t aHeight);
    void RenderFrame();
    TCamera* GetCameraPtr() { return mCamera;  }
private:
    // View data
    uint32_t mViewWidth;
    uint32_t mViewHeight;
    TCamera* mCamera;

    // Render data
    CGPUShaderProgramRef mFullscreenVertexProgram;
    CGPUShaderProgramRef mColorFragmentProgram;
    CGPUShaderPipelineRef mScreenQuadPipeline;
    uint32_t mDummyVAO;


    
};