// Copyright (c) 2022 David Gallardo and SDFEditor Project
// Serialization for Save/Load

#pragma once

#include <string>
#include <functional>

class CScene;

using TDocStateChangeCallback = std::function<void(bool)>;

class CSceneDocument
{
public:
    CSceneDocument(CScene& aScene);

    bool HasFilePath() const { return mFilePath.length() > 0; }
    std::string GetFilePath() const { return mFilePath; }
    void SetFilePath(const std::string& aFilePath) { mFilePath = aFilePath; };
    
    bool HasPendingChanges() const { return mPendingChanges; }
    void SetPendingChanges(bool aPendingChanges, bool aForceCallback);

    void SetDocStateChangeCallback(TDocStateChangeCallback aCallback) { mDocStateChangeCallback = aCallback; }

    void Save();
    void Load();
private:

    CScene& mScene;

    std::string mFilePath;
    bool mPendingChanges{ false };

    TDocStateChangeCallback mDocStateChangeCallback;
};