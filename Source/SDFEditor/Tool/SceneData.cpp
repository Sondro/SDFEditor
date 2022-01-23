
#pragma once

#include "SceneData.h"

CScene::CScene()
    : mDirty(true)
{
    mStorkesArray.push_back({
        {-0.4, 0.0, -1.0, 0.4},
        {0.35, 0.0, 0.0, 0.0}
    });

    mStorkesArray.push_back({
        {0.4, 0.0, -1.0, 0.4},
        {0.35, 0.0, 0.0, 0.0}
    });
}

CScene::~CScene()
{
}
