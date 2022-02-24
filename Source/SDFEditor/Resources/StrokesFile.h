
#pragma once

#include <string>

class CSceneData;
void LoadStrokesFile(const std::string& aPath,       CSceneData& aScene);
void SaveStrokesFile(const std::string& aPath, const CSceneData& aScene);