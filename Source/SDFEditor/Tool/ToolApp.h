// Copyright (c) 2022 David Gallardo and SDFEditor Project

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

    void ResetScene();
    
    void SaveScene();
    void SaveScene(const std::string& aFilePath);
    void LoadScene(const std::string& aFilePath);

    void WantClose();
    void Terminate();
    bool IsRunning() const { return mRunning; }

    void UpdateTitleBar();

    CScene& GetScene() { return mScene; }

private:
    void UpdateCamera(bool& aCameraMoving);
    bool HandleShortcuts();
   

private:

    TAppViewport mSceneViewport;

    CRenderer mRenderer;
    CScene mScene;
    std::string mTitle;
    bool mRunning{ true };
};