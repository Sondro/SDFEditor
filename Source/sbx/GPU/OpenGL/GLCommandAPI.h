/*
 * @file    GLCommandAPI.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_GL_COMMAND_API_H
#define _SBX_GL_COMMAND_API_H

#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/GPUCommandResourcesInc.h>

namespace sbx
{
    struct  TGPUCommand;
    class   CGPUCommandManager;
};

namespace sbx { namespace gl
{
    void    GLInitCommandManager        (CGPUCommandManager* aCommandManager);
    void    GLEndCommandManager         ();
    void    GLDeleteCommand             (TGPUCommand* aCommand);
    size_t  GLGetTextureMemoryAllocated ();

#   define SBX_GPU_COMMAND(_CommandName, ...) TGPUCommand* GLAlloc##_CommandName(__VA_ARGS__);
#   include <sbx/GPU/GPUCommandList.def>

}} // sbx::gl

#endif // #ifndef _SBX_GL_COMMAND_API_H