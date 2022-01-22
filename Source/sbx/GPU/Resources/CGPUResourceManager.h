/*
 * @file    GPUResourceManager.h
 * @author  David Gallardo Moreno
 */

#ifndef _GPU_RESOURCE_MANAGER_H_
#define _GPU_RESOURCE_MANAGER_H_

#include <sbx/Utils/LangUtils.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>

namespace sbx
{    
    class CGPUResourceManager
    {
    public:
                CGPUResourceManager     ();
               ~CGPUResourceManager     ();
        bool    Init                    ();
        void    End                     ();

        TGPUResourceRef CreateResource  (TGPUHandle aHandle, TUniqueId const & aTag);
        
        //TODO: remove old public api
        /*
        void            SetResHandle    (TGPUResourceRef const & aResourceRef, TGPUHandle aHandle);
        TGPUHandle      GetResHandle    (TGPUResourceRef const & aResourceRef) const;

        void            Retain          (TGPUResourceRef const & aResourceRef);
        void            Release         (TGPUResourceRef const & aResourceRef);*/

        //TODO: add methods to iterate resources
    private:
        void            SetResHandle    (void* aInnerData, TGPUHandle aHandle);
        TGPUHandle      GetResHandle    (void* aInnerData) const;
        void            Retain          (void* aInnerData);
        void            Release         (void* aInnerData);
        void            RemoveResource  (void* aInnerData);
    private:
        struct TImpl;
        std::unique_ptr< TImpl > mImpl;

        friend TGPUResourceRef;
    };

    void                    InitGPUResourceManager  ();
    void                    EndGPUResourceManager   ();
    CGPUResourceManager*    GetGPUResourceManager   ();
} // sbx

#endif // _GPU_RESOURCE_MANAGER_H_
