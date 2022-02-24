
#pragma once

#include "SceneData.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>



CScene::CScene()
    : mStack(std::make_unique<CSceneStack>(*this))
    , mClipboard(std::make_unique<CSceneClipboard>(*this))
    , mDirty(true)
    , mNextStrokeId(0)
{
    AddNewStroke();
    mStack->PushState(EPushStateFlags::EPE_ALL);

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

void CScene::Reset(bool aAddDefault)
{
    mStorkesArray.clear();
    mSelectedItems.clear();
    mStack->Reset();
    if (aAddDefault)
    {
        AddNewStroke();
        mStack->PushState(EPushStateFlags::EPE_ALL);
    }
    SetDirty();
    mNextStrokeId = 0;
}

uint32_t CScene::AddNewStroke(uint32_t aBaseStrokeIndex)
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
                {0.0f, 0.0f, 0.0f, 0.0f},     // posb
                {0.0, 0.0, 0.0, 0.0},           // quat
                {0.35f, 0.35f, 0.35f, 0.0f},    // param0
                {0.0, 0.0, 0.0, 0.0},           // param1
                {1, 0, 0, 0},                   // id
            };

            mStorkesArray.emplace_back(lDefaultShape, glm::vec3(0.0f), "");
        }
    }

    ::snprintf(mStorkesArray.back().mName, TStrokeInfo::MAX_NAME_SIZE, "Stroke_%d", mNextStrokeId);

    return mNextStrokeId++;
}

