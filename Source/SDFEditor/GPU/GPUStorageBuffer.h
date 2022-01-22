
#pragma once

#include <cstdint>
#include <memory>

using CGPUShaderStorageObjectRef = std::shared_ptr<class CGPUShaderStorageObject>;

namespace EGPUBufferFlags
{
    enum Type
    {
        MAP_READ_BIT = 0x0001,
        MAP_WRITE_BIT = 0x0002,
        DYNAMIC_STORAGE = 0x0100,
        ALL = MAP_READ_BIT | MAP_WRITE_BIT | DYNAMIC_STORAGE,
    };
};

class CGPUShaderStorageObject
{
public:
    CGPUShaderStorageObject();
    ~CGPUShaderStorageObject();

    void SetData(size_t aSize, void* aData, uint32_t aFlags = EGPUBufferFlags::ALL);

    void* Map();
    void Unmap();
    void UpdateSubData(intptr_t aOffset, size_t aSize, void* aData);

    size_t GetStorageSize() const { return mStorageSize; }

    void Bind(uint32_t aBinding, uint32_t aProgramHandler);
    void Unbind();

private:
    uint32_t mBufferHandler;
    size_t mStorageSize;
};