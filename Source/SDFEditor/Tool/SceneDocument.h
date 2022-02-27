
#pragma once

#include <string>
#include <functional>

class CScene;

class CSceneDocument
{
public:
    CSceneDocument(CScene& aScene);

    bool HasFilePath() const { return mFilePath.length() > 0; }
    std::string GetFilePath() const { return mFilePath; }
    void SetFilePath(const std::string& aFilePath) { mFilePath = aFilePath; };
    
    bool HasPendingChanges() const { return mPendingChanges; }
    void SetPendingChanges(bool aPendingChanges);

    void Save();
    void Load();
private:

    CScene& mScene;

    std::string mFilePath;
    bool mPendingChanges{ false };

    std::function<void(bool)> mPendingChangesCallback;
};