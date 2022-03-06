// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "SceneStack.h"
#include "Scene.h"

#include <sbx/Core/Log.h>

constexpr size_t kMaxStackElements = 20;

TStoredSceneState::TStoredSceneState(TPushStateFlags aFlags)
    : mFlags(aFlags)
    , mStrokesArray(nullptr)
    , mSelectedItems(nullptr)
    , mGlobalMaterial(nullptr)

{
}

TStoredSceneState::~TStoredSceneState()
{
    if (mStrokesArray != nullptr)
    {
        delete mStrokesArray;
        mStrokesArray = nullptr;
    }

    if (mSelectedItems != nullptr)
    {
        delete mSelectedItems;
        mSelectedItems = nullptr;
    }

    if (mGlobalMaterial != nullptr)
    {
        delete mGlobalMaterial;
        mGlobalMaterial = nullptr;
    }
}

void CSceneStack::Reset()
{
    mPopedStates.clear();
    mPushedStates.clear();
}

void CSceneStack::PushState(TPushStateFlags aFlags)
{
    if (aFlags != 0)
    {
        mPopedStates.clear();

        while (mPushedStates.size() >= kMaxStackElements)
        {
            mPushedStates.pop_front();
        }

        TStoredSceneStateRef lState = std::make_shared< TStoredSceneState >(aFlags);

        if (aFlags & EPushStateFlags::EPE_STROKES)
        {
            lState->mStrokesArray = new std::vector< TStrokeInfo >(mScene.mStrokesArray);
        }

        if (aFlags & EPushStateFlags::EPE_SELECTION)
        {
            lState->mSelectedItems = new std::vector< uint32_t >(mScene.mSelectedItems);
        }

        // TODO: material not in undo & redo for now
        //if (aFlags & EPushStateFlags::EPE_MATERIAL)
        //{
        //    lState->mGlobalMaterial = new TGlobalMaterialBufferData(mScene.mGlobalMaterial);
        //}

        mPushedStates.push_back(lState);
    }
}

bool CSceneStack::PopState()
{
    if(mPushedStates.size() > 1)
    {
        mPopedStates.push_back(mPushedStates.back());
        mPushedStates.pop_back();

        ApplySceneState(true);

        return true;
    }

    return false;
}

bool CSceneStack::RestorePopedState()
{
    if (mPopedStates.size() > 0)
    {
        mPushedStates.push_back(mPopedStates.back());
        mPopedStates.pop_back();

        ApplySceneState(false);

        return true;
    }

    return false;
}

void CSceneStack::ApplySceneState(bool aPopedSelection)
{
    //appply scene changes
    if (mPushedStates.size() > 0)
    {
        if (mPushedStates.back()->mStrokesArray)
        {
            mScene.mStrokesArray = *mPushedStates.back()->mStrokesArray;
            mScene.SetDirty();
        }

        if (mPushedStates.back()->mGlobalMaterial)
        {
            mScene.mGlobalMaterial = *mPushedStates.back()->mGlobalMaterial;
            mScene.SetMaterialDirty();
        }
    }

    if(aPopedSelection)
    {
        if (mPopedStates.size() > 0 && mPopedStates.back()->mSelectedItems)
        {
            mScene.mSelectedItems = *mPopedStates.back()->mSelectedItems;
        }
    }
    else
    {
        if (mPushedStates.size() > 0 && mPushedStates.back()->mSelectedItems)
        {
            mScene.mSelectedItems = *mPushedStates.back()->mSelectedItems;
        }
    }
}
