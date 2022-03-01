// Copyright (c) 2022 David Gallardo and SDFEditor Project

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
    , mDocument(std::make_unique<CSceneDocument>(*this))
    , mDirty(true)
    , mNextStrokeId(0)
{
    Reset(true);
}

CScene::~CScene()
{
}

void CScene::Reset(bool aAddDefault)
{
    mNextStrokeId = 0;
    mStorkesArray.clear();
    mSelectedItems.clear();
    if (aAddDefault)
    {
        AddNewStroke();
    }
    mStack->Reset();
    SetDirty();
    mDocument->SetFilePath("");
    mDocument->SetPendingChanges(false, true);
}

void CScene::SetDirty() 
{
    mDirty = true; 
    mDocument->SetPendingChanges(true, false); 
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
                {0.0f, 0.0f, 0.0f, 0.0f},       // posb
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

