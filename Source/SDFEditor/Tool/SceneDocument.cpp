
#include "SceneDocument.h"
#include "SceneData.h"

#include <SDFEditor/Utils/FileIO.h>

#include <ThirdParty/nlohmann/json.hpp>

CSceneDocument::CSceneDocument(CScene& aScene)
    : mScene(aScene)
{
}

void CSceneDocument::SetPendingChanges(bool aPendingChanges)
{ 
    mPendingChanges = aPendingChanges; 
    if (mPendingChangesCallback)
    {
        mPendingChangesCallback(aPendingChanges);
    }
}

void CSceneDocument::Save()
{
    if (HasFilePath())
    {
        size_t lStrokesBytesSize = mScene.mStorkesArray.size() * sizeof(TStrokeInfo);
        uint32_t lNumElements = mScene.mStorkesArray.size() & 0xFFFFFFFF;

        std::vector<char> lData(lStrokesBytesSize + sizeof(lNumElements));

        ::memcpy(lData.data(), &lNumElements, sizeof(lNumElements));
        ::memcpy(lData.data() + sizeof(lNumElements), mScene.mStorkesArray.data(), lStrokesBytesSize);

        WriteFile(mFilePath, lData);
        mPendingChanges = false;
    }
}

void CSceneDocument::Load()
{
    if (HasFilePath())
    {
        std::vector<char> lData = ReadFile(mFilePath);

        if (lData.size() > 0)
        {
            uint32_t lNumElements = 0;
            ::memcpy(&lNumElements, lData.data(), sizeof(lNumElements));

            mScene.mStorkesArray.clear();
            mScene.mSelectedItems.clear();
            mScene.mStack->Reset();
            mScene.mStorkesArray.resize(lNumElements);
            //TODO: json
            ::memcpy(mScene.mStorkesArray.data(), lData.data() + sizeof(lNumElements), lNumElements * sizeof(TStrokeInfo));
            mScene.SetDirty();
            mScene.mStack->PushState(EPushStateFlags::EPE_ALL);
            mPendingChanges = false;
        }
    }
}
