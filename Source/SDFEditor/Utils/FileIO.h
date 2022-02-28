
#pragma once

#include <vector>
#include <string>


std::vector<char> ReadFile(const std::string& aFilename);

void WriteFile(const std::string& aFilename, std::vector<char> const& aData);