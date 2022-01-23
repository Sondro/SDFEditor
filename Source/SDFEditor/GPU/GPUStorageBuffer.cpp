
#include "GPUStorageBuffer.h"

#include "ThirdParty/glad/glad.h"

CGPUShaderStorageObject::CGPUShaderStorageObject()
    : mStorageSize(0)
{
    //glGenBuffers(1, &mBufferHandler);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferHandler);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, 0, 0, GL_DYNAMIC_COPY);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferHandler);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, aSize, nullptr, GL_DYNAMIC_COPY);

    glCreateBuffers(1, &mBufferHandler);
    
}

CGPUShaderStorageObject::~CGPUShaderStorageObject()
{
    glDeleteBuffers(1, &mBufferHandler);
}

void CGPUShaderStorageObject::SetData(size_t aSize, void* aData, uint32_t aFlags)
{
    uint32_t lFlags = 0;
    lFlags |= (aFlags & EGPUBufferFlags::DYNAMIC_STORAGE) ? GL_DYNAMIC_STORAGE_BIT : 0;
    lFlags |= (aFlags & EGPUBufferFlags::MAP_READ_BIT) ? GL_MAP_READ_BIT : 0;
    lFlags |= (aFlags & EGPUBufferFlags::MAP_WRITE_BIT) ? GL_MAP_WRITE_BIT : 0;
    glNamedBufferStorage(mBufferHandler, aSize, aData, lFlags);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBufferHandler);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, aSize, aData, GL_DYNAMIC_COPY);
    mStorageSize = aSize;
}

void* CGPUShaderStorageObject::Map()
{
    return glMapNamedBuffer(mBufferHandler, GL_WRITE_ONLY);
}

void CGPUShaderStorageObject::Unmap()
{
    glUnmapNamedBuffer(mBufferHandler);
}

void CGPUShaderStorageObject::UpdateSubData(intptr_t aOffset, size_t aSize, void* aData)
{
    glNamedBufferSubData(mBufferHandler, aOffset, aSize, aData);
}

void CGPUShaderStorageObject::BindToProgram(uint32_t aProgramHandler, uint32_t aBinding, const char* aLayoutName)
{
    uint32_t block_index = glGetProgramResourceIndex(aProgramHandler, GL_SHADER_STORAGE_BLOCK, aLayoutName);
    glShaderStorageBlockBinding(aProgramHandler, block_index, aBinding);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, aBinding, mBufferHandler);
}

void CGPUShaderStorageObject::Unbind()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
