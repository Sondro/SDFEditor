
#pragma once

#include <string>

class CSceneData;

class CSceneDocument
{
public:
    CSceneDocument(CSceneData& aScene);

    bool HasFilePath() const { return mFilePath.length() > 0; }
    std::string GetFilePath() const { return mFilePath; }
    void SetFilePath(const std::string& aFilePath) { mFilePath = aFilePath; };
    
    bool HasPendingChanges() const { return mPendingChanges; }
    bool SetPendingChanges() { mPendingChanges = true; }

    void SavePendingChanges();
    void Load();
private:

    CSceneData& mScene;

    std::string mFilePath;
    bool mPendingChanges{ false };
};