
// @Brief holds the state changes of the scene to do Undo/Redo

#pragma once

#include <SDFEditor/Tool/SceneData.h>
#include <deque>

class CScene;
struct TStrokeInfo;

namespace EPushStateFlags
{
    enum Type
    {
        EPE_STROKES = 1 << 0,
        EPE_SELECTION = 1 << 1,
        EPE_ALL = EPE_STROKES | EPE_STROKES,
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
    void PopState();
    void RestorePopedState();
    bool HavePushedStates() const { return mPushedStates.size() > 1; }
    bool HavePopedStates() const { return mPopedStates.size() > 0; }

private:
    void ApplySceneState();

private:
    CScene& mScene;

    // Undo/Redo data
    std::deque< TStoredSceneStateRef > mPushedStates;
    std::deque< TStoredSceneStateRef > mPopedStates;
};
