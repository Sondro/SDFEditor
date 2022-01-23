
#pragma once

#include "SceneData.h"

CScene::CScene()
    : mDirty(true)
{
    mStorkesArray.push_back({
        {-0.4f, 0.0f, -1.0f, 0.4f},
        {0.35f, 0.35f, 0.35f, 0.0f},
        {0, 0, 0, 0}
    });

    mStorkesArray.push_back({
        {0.4f, 0.0f, -1.0f, 0.4f},
        {0.35f, 0.35f, 0.35f, 0.0f},
        {1, 0, 0, 0}
    });

    mStorkesArray.push_back({
        {0.4f, 0.8f, -1.0f, 0.4f},
        {0.5f, 0.1f, 0.35f, 0.0f},
        {2, 0, 0, 0}
        });
}

CScene::~CScene()
{
}
