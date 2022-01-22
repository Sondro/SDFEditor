/*
 * @file    CGPUProgram.h
 * @author  David Gallardo Moreno
 */

#ifndef _GPU_PROGRAM_H
#define _GPU_PROGRAM_H

#include <sbx/Library/FRefCounted.h>
#include <sbx/GPU/Resources/TGPUResourceRef.h>

namespace sbx
{
    struct TGPUProgramData;
    class CGPUProgram : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CGPUProgram)
    public: 
                                CGPUProgram     (TGPUProgramDesc::Ref const & aDesc, 
                                                 TGPUCommandCallback const & aCallback = nullptr, 
                                                 TGPUCommandCallback const & aDestroyCallback = nullptr);
                               ~CGPUProgram     ();
        TGPUResourceRef         GetRes          () const { return mResRef; }
        void                    Swap            (CGPUProgram & aOther);
    private:
        TGPUProgramDesc::Ref        mDesc;
        TGPUResourceRef             mResRef;
        TGPUCommandCallback         mDestroyCallback;
    };

} // sbx::gfx

#endif // _GPU_PROGRAM_H
