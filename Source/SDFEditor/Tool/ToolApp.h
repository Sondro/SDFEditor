
#pragma once

#include <cstdint>
#include "SDFEditor/GPU/Renderer.h"
#include "SDFEditor/Tool/SceneData.h"

struct TAppViewport
{
    int32_t mWidth;
    int32_t mHeight;
};

class CToolApp
{
public:
    CToolApp();
    ~CToolApp();

    void Init();
    void Shutdown();

    void Update();
    void Render();

private:
    void UpdateCamera();

private:

    TAppViewport mSceneViewport;

    CRenderer mRenderer;
    CScene mScene;
};