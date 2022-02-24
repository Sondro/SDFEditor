
#include "SceneDocument.h"
#include <ThirdParty/nlohmann/json.hpp>

CSceneDocument::CSceneDocument(CSceneData& aScene)
    : mScene(aScene)
{
}

void CSceneDocument::SavePendingChanges()
{
}

void CSceneDocument::Load()
{
}
