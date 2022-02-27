
#include "SceneDocument.h"
#include "SceneData.h"

#include <SDFEditor/Utils/FileIO.h>

#include <ThirdParty/nlohmann/json.hpp>

CSceneDocument::CSceneDocument(CScene& aScene)
    : mScene(aScene)
{
}

void CSceneDocument::SetPendingChanges(bool aPendingChanges, bool aForceCallback)
{ 
    if (mPendingChanges != aPendingChanges || aForceCallback)
    {
        mPendingChanges = aPendingChanges;
        if (mDocStateChangeCallback)
        {
            mDocStateChangeCallback(aPendingChanges);
        }
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
        //TODO: json
        WriteFile(mFilePath, lData);
        SetPendingChanges(false, true);
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
            mScene.mStorkesArray.resize(lNumElements);
            //TODO: json
            ::memcpy(mScene.mStorkesArray.data(), lData.data() + sizeof(lNumElements), lNumElements * sizeof(TStrokeInfo));
            mScene.SetDirty();
            mScene.mStack->Reset();
            SetPendingChanges(false, true);
        }
    }
}
