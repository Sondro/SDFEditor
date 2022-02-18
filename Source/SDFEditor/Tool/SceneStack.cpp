
#include "SceneStack.h"

constexpr size_t kMaxStackElements = 20;

TStoredSceneState::TStoredSceneState(TPushStateFlags aFlags)
    : mFlags(aFlags)
    , mStorkesArray(nullptr)
    , mSelectedItems(nullptr)

{
}

TStoredSceneState::~TStoredSceneState()
{
    if (mStorkesArray != nullptr)
    {
        delete mStorkesArray;
        mStorkesArray = nullptr;
    }

    if (mSelectedItems != nullptr)
    {
        delete mSelectedItems;
        mSelectedItems = nullptr;
    }
}

void CSceneStack::Reset()
{
    mPopedStates.clear();
    mPushedStates.clear();
    PushState(EPushStateFlags::EPE_ALL);
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
            lState->mStorkesArray = new std::vector< TStrokeInfo >(mScene.mStorkesArray);
        }

        if (aFlags & EPushStateFlags::EPE_SELECTION)
        {
            lState->mSelectedItems = new std::vector< uint32_t >(mScene.mSelectedItems);
        }

        mPushedStates.push_back(lState);
    }
}

bool CSceneStack::PopState()
{
    if(mPushedStates.size() > 1)
    {
        mPopedStates.push_back(mPushedStates.back());
        mPushedStates.pop_back();

        ApplySceneState();

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

        ApplySceneState();

        return true;
    }

    return false;
}

void CSceneStack::ApplySceneState()
{
    //appply scene changes
    if (mPushedStates.back()->mStorkesArray)
    {
        mScene.mStorkesArray = *mPushedStates.back()->mStorkesArray;
    }

    if (mPushedStates.back()->mSelectedItems)
    {
        mScene.mSelectedItems = *mPushedStates.back()->mSelectedItems;
    }
}
