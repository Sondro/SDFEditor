// Copyright (c) 2022 David Gallardo and SDFEditor Project
// Holds the state changes of the scene to do Undo/Redo

#pragma once

#include <deque>
#include <vector>
#include <memory>

#include <SDFEditor/Tool/StrokeInfo.h>

class CScene;

namespace EPushStateFlags
{
    enum Type
    {
        EPE_STROKES = 1 << 0,
        EPE_SELECTION = 1 << 1,
        EPE_ALL = EPE_STROKES | EPE_SELECTION,
    };
};

using TPushStateFlags = uint32_t;

struct TStoredSceneState
{
    TPushStateFlags             mFlags;
    std::vector< TStrokeInfo >* mStorkesArray;
    std::vector< uint32_t >*    mSelectedItems;

    TStoredSceneState(TPushStateFlags aFlags);
    ~TStoredSceneState();
};

using TStoredSceneStateRef = std::shared_ptr< TStoredSceneState >;

class CSceneStack
{
public:
    CSceneStack(CScene& aScene) : mScene(aScene) {}

    // Undo/Redo functionality
    void Reset();
    void PushState(TPushStateFlags aFlags);
    bool PopState();
    bool RestorePopedState();
    bool HavePushedStates() const { return mPushedStates.size() > 1; }
    bool HavePopedStates() const { return mPopedStates.size() > 0; }

private:
    void ApplySceneState(bool aPopedSelection);

private:
    CScene& mScene;

    // Undo/Redo data
    std::deque< TStoredSceneStateRef > mPushedStates;
    std::deque< TStoredSceneStateRef > mPopedStates;
};
