/*
 * @file    GPUCommandManager.h
 * @author  David Gallardo Moreno
 */

#ifndef _GPU_COMMAND_MANAGER_H_
#define _GPU_COMMAND_MANAGER_H_

#include <sbx/Utils/LangUtils.h>
#include <sbx/GPU/GPUTypes.h>
#include <sbx/GPU/GPUCommandResourcesInc.h>
#include <functional>

struct TMsgAppPreRender;

namespace sbx
{
    struct TGPUCommand
    {
    public:

        virtual        ~TGPUCommand (){}
        virtual void    Execute     () = 0;

        typedef std::function< void(void) > TGPUCommandCallback;
        TGPUCommandCallback mCallback;
        uint64_t mOwnerId {0};
        uint32_t mSubIndex {0};
    };

    class CGPUCommandManager
    {
    public:
                CGPUCommandManager  ();
               ~CGPUCommandManager  ();
        bool    Init                ();
        void    End                 ();

        void    QueueCommand        (TGPUCommand* aCommand);
        void    CancelCommandQueue  ();
        void    OnPreRender         (TMsgAppPreRender const * aMsg);
        
        
        size_t  GetTextureMemoryAllocated () const;
    public:
#       define SBX_GPU_COMMAND(_CommandName, ...) typedef TGPUCommand*(*F##_CommandName)(__VA_ARGS__);
#       include <sbx/GPU/GPUCommandList.def>
#       define SBX_GPU_COMMAND(_CommandName, ...) F##_CommandName Alloc##_CommandName;
#       include <sbx/GPU/GPUCommandList.def>

    private:
        struct TImpl;
        TImpl* mImpl;
    };

    void                InitGPUCommandManager   ();
    void                EndGPUCommandManager    ();
    CGPUCommandManager* GetGPUCommandManager    ();

} // sbx

#endif //_GPU_COMMAND_MANAGER_H_