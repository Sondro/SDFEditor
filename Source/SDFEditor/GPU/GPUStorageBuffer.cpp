// Copyright (c) 2022 David Gallardo and SDFEditor Project

#include "GPUStorageBuffer.h"

#include "ThirdParty/glad/glad.h"

GLenum sGPUBufferBindTarget[] =
{
    GL_UNIFORM_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
};

CGPUBufferObject::CGPUBufferObject(EGPUBufferBindTarget::Type aTarget)
    : mStorageSize(0)
    , mTarget(aTarget)
{
    glCreateBuffers(1, &mBufferHandler);
}

CGPUBufferObject::~CGPUBufferObject()
{
    glDeleteBuffers(1, &mBufferHandler);
}

void CGPUBufferObject::SetData(size_t aSize, void* aData, uint32_t aFlags)
{
    uint32_t lFlags = 0;
    lFlags |= (aFlags & EGPUBufferFlags::DYNAMIC_STORAGE) ? GL_DYNAMIC_STORAGE_BIT : 0;
    lFlags |= (aFlags & EGPUBufferFlags::MAP_READ_BIT) ? GL_MAP_READ_BIT : 0;
    lFlags |= (aFlags & EGPUBufferFlags::MAP_WRITE_BIT) ? GL_MAP_WRITE_BIT : 0;
    glNamedBufferStorage(mBufferHandler, aSize, aData, lFlags);
    mStorageSize = aSize;
}

void CGPUBufferObject::UpdateSubData(intptr_t aOffset, size_t aSize, void* aData)
{
    glNamedBufferSubData(mBufferHandler, aOffset, aSize, aData);
}

void* CGPUBufferObject::Map()
{
    return glMapNamedBuffer(mBufferHandler, GL_WRITE_ONLY);
}

void CGPUBufferObject::Unmap()
{
    glUnmapNamedBuffer(mBufferHandler);
}

void CGPUBufferObject::BindShaderStorage(uint32_t aBindingIndex)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, aBindingIndex, mBufferHandler);
}

void CGPUBufferObject::BindUniformBuffer(uint32_t aBindingIndex)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, aBindingIndex, mBufferHandler);
}

void CGPUBufferObject::BindTarget(EGPUBufferBindTarget::Type aBindTarget)
{
    glBindBuffer(sGPUBufferBindTarget[aBindTarget], mBufferHandler);
}

void CGPUBufferObject::UnbindTarget(EGPUBufferBindTarget::Type aBindTarget)
{
    glBindBuffer(sGPUBufferBindTarget[aBindTarget], 0);
}