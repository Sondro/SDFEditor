// Copyright (c) 2022 David Gallardo and SDFEditor Project

#pragma once

#include <vector>
#include <SDFEditor/Tool/StrokeInfo.h>

class CSceneClipboard
{
public:
    CSceneClipboard(class CScene& aScene);

    void CopySelectedItems();
    bool AddCopiedItems();
private:

    class CScene& mScene;

    std::vector< TStrokeInfo > mCopiedElements;
};