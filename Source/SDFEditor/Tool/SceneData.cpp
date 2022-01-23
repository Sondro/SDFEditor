
#pragma once

#include "SceneData.h"

CScene::CScene()
    : mDirty(true)
{
    mStorkesArray.push_back({
        {-0.4f, 0.0f, -1.0f, 0.4f},
        {0.35f, 0.0f, 0.0f, 0.0f}
    });

    mStorkesArray.push_back({
        {0.4f, 0.0f, -1.0f, 0.4f},
        {0.35f, 0.0f, 0.0f, 0.0f}
    });
}

CScene::~CScene()
{
}
