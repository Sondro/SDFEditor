

#include "SceneClipboard.h"
#include "SceneData.h"

CSceneClipboard::CSceneClipboard(CScene& aScene)
    : mScene(aScene)
{
}

void CSceneClipboard::CopySelectedItems()
{
    // TODO: Idea: Copy with camera transform, to paste in the relative position and orientation?

    // don't do anything of there is no selection
    if (mScene.mSelectedItems.size() > 0)
    {
        // clear previusly copied elements
        mCopiedElements.clear();

        for (uint32_t lIndex : mScene.mSelectedItems)
        {
            if (lIndex < mScene.mStorkesArray.size())
            {
                mCopiedElements.push_back(mScene.mStorkesArray[lIndex]);
            }
        }
    }
}

bool CSceneClipboard::AddCopiedItems()
{
    if (mCopiedElements.size() > 0)
    {
        // iterate copied elements and add to the strokes array
        for (auto& lStroke : mCopiedElements)
        {
            mScene.mStorkesArray.push_back(lStroke);
        }

        // select the added items
        mScene.mSelectedItems.clear();
        size_t lLastIndex = mScene.mStorkesArray.size() - 1;
        for (size_t i = 0, l = mCopiedElements.size(); i < l; ++i)
        {
            mScene.mSelectedItems.push_back((lLastIndex - i) & 0xFFFFFFF);
        }

        return true;
    }

    return false;
}
