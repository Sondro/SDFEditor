
#pragma once

#include "SceneData.h"

CScene::CScene()
    : mDirty(true)
    , mNextStrokeId(0)
{
    AddNewStorke();

    /*mStorkesArray.push_back({
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
        });*/
}

CScene::~CScene()
{
}

uint32_t CScene::AddNewStorke(uint32_t aBaseStrokeIndex)
{
    if (aBaseStrokeIndex < mStorkesArray.size())
    {
        mStorkesArray.push_back(mStorkesArray[aBaseStrokeIndex]);
        
    }
    else
    {
        if (mStorkesArray.size() > 0)
        {
            mStorkesArray.push_back(mStorkesArray.back());
        }
        else
        {
            stroke_t lDefaultShape =
            {
                {0.4f, 0.0f, -1.0f, 0.01f},
                {0.35f, 0.35f, 0.35f, 0.0f},
                {1, 0, 0, 0},

            };

            mStorkesArray.emplace_back(lDefaultShape, "");
        }
    }

    ::snprintf(mStorkesArray.back().mName, TStrokeInfo::MAX_NAME_SIZE, "Stroke_%d", mNextStrokeId);

    return mNextStrokeId++;
}
