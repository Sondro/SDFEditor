/*
 * @file    FObjectPool.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_OBJECT_POOL_H__
#define __SBX_OBJECT_POOL_H__

#include <sbx/Library/FRawPool.h>

namespace sbx
{
    template <typename T> class FObjectPool
    {
        SBX_DISALLOW_COPY(FObjectPool)
    public:
        FObjectPool()
        {
        }

        ~FObjectPool()
        {
            End();
        }

        bool Init(size_t aPageObjects)
        {
            return mRawPool.Init(sizeof(T), aPageObjects);
        }

        void End()
        {
            mRawPool.End();
        }

        bool IsOk() const
        {
            return mRawPool.IsOk();
        }

        template <typename... Args> T* AllocOjbect(Args... aArgs)
        {
            return SBX_PLACEMENT_NEW(T, mRawPool.Alloc(), aArgs...);
        }

        void FreeObject(T* aPtr)
        {
            aPtr->~T();
            mRawPool.Free(aPtr);
        }
    
    private:
        FRawPool mRawPool;
    };
}

#endif //__SBX_OBJECT_POOL_H__
