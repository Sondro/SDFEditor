
#pragma once

#include <cstdint>
#include <vector>
#include <memory>


#include <SDFEditor/Tool/StrokeInfo.h>
#include <SDFEditor/Tool/SceneStack.h>
#include <SDFEditor/Tool/SceneClipboard.h>
#include <SDFEditor/Tool/Camera.h>

/*
struct SShape
{
    vec4  attr0; //xyz: pos, w: 
    vec4  attr1; //xyz: scale, w: 
    vec4  attr2; //x: blend, y: hole, z: round, w: 
    vec4  quat;  //rotation quaternion
    ivec4 id;    //x: primitive, y: material, z: flags (SUBTRACTIVE | MASK | MIRRORX | MIRRORY | MIRRORZ | REPEAT_CIRCLE), w: repeats
};
*/

class CScene
{
public:
    CScene();
    ~CScene();
    
    void Reset(bool aAddDefault);

    bool IsDirty() const { return mDirty; }
    void SetDirty() { mDirty = true; }
    void CleanDirtyFlag() { mDirty = false; }



    uint32_t AddNewStroke(uint32_t aBaseStrokeIndex = UINT32_MAX);

    // Scene data
    std::vector< TStrokeInfo > mStorkesArray;
    std::vector<uint32_t> mSelectedItems;
    CCamera mCamera;

    // Components
    std::unique_ptr<CSceneStack> mStack;
    std::unique_ptr<CSceneClipboard> mClipboard;

    // Debug
    int32_t mPreviewSlice{ 64 };
    bool    mUseVoxels{ true };
    bool    mLutNearestFilter{ false };
    bool    mAtlasNearestFilter{ false };
private:
    bool mDirty;
    uint32_t mNextStrokeId;
};

