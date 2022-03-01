// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "GPUStorageBuffer.h"

#include "ThirdParty/glad/glad.h"

GLenum sGPUBufferBindTarget[] =
{
    GL_SHADER_STORAGE_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
};

CGPUShaderStorageObject::CGPUShaderStorageObject(EGPUBufferBindTarget::Type aTarget)
    : mStorageSize(0)
    , mTarget(aTarget)
{
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
    mStorageSize = aSize;
}

void CGPUShaderStorageObject::UpdateSubData(intptr_t aOffset, size_t aSize, void* aData)
{
    glNamedBufferSubData(mBufferHandler, aOffset, aSize, aData);
}

void* CGPUShaderStorageObject::Map()
{
    return glMapNamedBuffer(mBufferHandler, GL_WRITE_ONLY);
}

void CGPUShaderStorageObject::Unmap()
{
    glUnmapNamedBuffer(mBufferHandler);
}

void CGPUShaderStorageObject::BindShaderStorage(uint32_t aBindingIndex)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, aBindingIndex, mBufferHandler);
}

void CGPUShaderStorageObject::BindTarget(EGPUBufferBindTarget::Type aBindTarget)
{
    glBindBuffer(sGPUBufferBindTarget[aBindTarget], mBufferHandler);
}

void CGPUShaderStorageObject::UnbindTarget(EGPUBufferBindTarget::Type aBindTarget)
{
    glBindBuffer(sGPUBufferBindTarget[aBindTarget], 0);
}