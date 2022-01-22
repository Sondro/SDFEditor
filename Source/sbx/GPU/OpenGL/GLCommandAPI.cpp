#include <sbx/Common.h>
#include "GLCommandAPI.h"

#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/CGPUCommandManager.h>
#include <sbx/GPU/Resources/CGPUResourceManager.h>
#include <sbx/DataStructures/CPropertySet.h>

#include <glad/glad.h>
#include <glad/glad_wgl.h>
#include <sbx/Library/FRawPool.h>

namespace
{
    size_t gGLTextureMemoryAllocated = 0;
}

#include "GLShared.inl"
#include "GLTextureCommands.inl"
#include "GLProgramCommands.inl"
#include "GLRenderCommands.inl"
#include "GLDataBuffersCommands.inl"

namespace sbx { namespace gl
{
    constexpr size_t GetBigger(size_t a, size_t b)
    {
        return (a > b) ? a : b;
    }

    constexpr size_t GetBiggerCommandSize()
    {
        return
#       define SBX_GPU_COMMAND(_CommandName, ...) GetBigger(sizeof( GL##_CommandName ),
#       include <sbx/GPU/GPUCommandList.def>
        0
#       define SBX_GPU_COMMAND(_CommandName, ...) )
#       include <sbx/GPU/GPUCommandList.def>
        ;
    }

    uint8_t AsBiggerAsTheBiggest[GetBiggerCommandSize()];

    FRawPool gGLCommandsRawPool;

    template <typename TCmd, typename... TArgs> TGPUCommand* GLAllocGenericTemplateCommand(TArgs... args)
    {
        //TODO: make a pool for each type of command
        return SBX_PLACEMENT_NEW(TCmd, gGLCommandsRawPool.Alloc(), args...);
    }

    void GLInitCommandManager(CGPUCommandManager* aCommandManager)
    {
        //Initialize command manager stubs
#       define SBX_GPU_COMMAND(_CommandName, ...) aCommandManager->Alloc##_CommandName = GLAllocGenericTemplateCommand<GL##_CommandName, __VA_ARGS__>;
#       include <sbx/GPU/GPUCommandList.def>

        gGLCommandsRawPool.Init(GetBiggerCommandSize(), 32);
    }

    void GLEndCommandManager()
    {
        gGLCommandsRawPool.End();
    }

    void GLDeleteCommand(TGPUCommand* aCommand)
    {
        aCommand->~TGPUCommand();
        gGLCommandsRawPool.Free(aCommand);
    }

    size_t GLGetTextureMemoryAllocated()
    {
        return gGLTextureMemoryAllocated;
    }

}} // sbx::gl
