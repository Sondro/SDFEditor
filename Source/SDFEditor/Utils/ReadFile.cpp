
#include "sbx/Common.h"
#include "ReadFile.h"

#include <cstdint>
#include <fstream>

std::vector<char> ReadFile(const std::string& aFilename)
{
    // Open stream from given file
    // std::ios::binary tells stream to read file as binary
    // std::ios::ate tells stream to start reading from end of file
    std::ifstream file(aFilename, std::ios::binary | std::ios::ate);

    // Check if file stream successfully opened
    if (!file.is_open())
    {
        SBX_ERROR("Failed to open a file!");
    }

    // Get current read position and use to resize file buffer
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> fileBuffer(fileSize + 1);

    // Move read position (seek to) the start of the file
    file.seekg(0);

    // Read the file data into buffer (stream "fileSize" in total)
    file.read(fileBuffer.data(), fileSize);

    fileBuffer.push_back(0);

    file.close();

    return fileBuffer;
}
