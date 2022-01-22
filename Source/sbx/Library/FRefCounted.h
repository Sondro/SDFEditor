/*
 * @file    FRefCounted.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_FREFCOUNTED_H__
#define __SBX_FREFCOUNTED_H__

#include <cstdint>

namespace sbx
{
    //-----------------------------------------------------------------------
    // FRefCounted
    //-----------------------------------------------------------------------
    class FRefCounted
    {
    public:
                    FRefCounted () 
                        : mRefCount(0) 
                    {;}

        void        AddRef      () { ++mRefCount; };
        void        RemoveRef   () { --mRefCount; };
        uint32_t    GetRefCount() const { return mRefCount; } 
    private:
        uint32_t mRefCount;

    };

    //-----------------------------------------------------------------------
    // FRefCountedPtr
    //-----------------------------------------------------------------------
    template <typename T> 
    class FRefCountedPtr
    {
    public:
        FRefCountedPtr ()
            : mPtr(nullptr)
        {;}

        FRefCountedPtr  (FRefCountedPtr const & aOther)
            : mPtr(aOther.mPtr)
        {
            if(nullptr != mPtr)
            {
                mPtr->AddRef();
            }
        };

        FRefCountedPtr  (T* aPtr)
            : mPtr(aPtr)
        {
            if(nullptr != mPtr)
            {
                mPtr->AddRef();
            }
        };

        ~FRefCountedPtr ()
        {
            if(nullptr != mPtr)
            {
                mPtr->RemoveRef();
                CheckPtr();
                mPtr = nullptr;
            }
        };

        FRefCountedPtr & operator=(FRefCountedPtr const & aOther)
        {
            if(aOther.mPtr != mPtr)
            {
                if(nullptr != mPtr)
                {
                    mPtr->RemoveRef();
                    CheckPtr();
                }

                mPtr = aOther.mPtr;

                if(nullptr != mPtr)
                {
                    mPtr->AddRef();
                }
            }

            return *this;
        }

        FRefCountedPtr & operator=(T* aPtr)
        {
            if(aPtr != mPtr)
            {
                if(nullptr != mPtr)
                {
                    mPtr->RemoveRef();
                    CheckPtr();
                }

                mPtr = aPtr;

                if(nullptr != mPtr)
                {
                    mPtr->AddRef();
                }
            }

            return *this;
        }

        operator bool               () const    { return (mPtr != nullptr) && (mPtr->GetRefCount() > 0); }
        //T*          operator->      ()          { return mPtr; }
        T *         operator->      () const    { return mPtr;  }
        T &         operator*       ()          { return *mPtr; }
        T const &   operator*       () const    { return *mPtr; }
        T *         GetPtr          () const    { return mPtr;  }
        bool        operator==      (FRefCountedPtr const & aOther) const { return mPtr == aOther.mPtr;   }
        bool        operator!=      (FRefCountedPtr const & aOther) const { return mPtr != aOther.mPtr;   }
        bool        operator<       (FRefCountedPtr const & aOther) const { return mPtr <  aOther.mPtr;   }
    private:
        void CheckPtr()
        {
            if((nullptr != mPtr) && (mPtr->GetRefCount() == 0))
            {
                SBX_DELETE(mPtr);
            }
        }

    private:
        T* mPtr;
    };

    template <typename T, typename... TArgs>
    FRefCountedPtr<T> MakeRefCounted(TArgs... aArgs)
    {
        return FRefCountedPtr<T>(SBX_NEW(T, aArgs...));
    }

#   define  SBX_REFCOUNTED_MAKER(_T) \
            public: \
                using Ref = FRefCountedPtr<_T>; \
                template <typename... TArgs> static FRefCountedPtr<_T> CreateInstance(TArgs... aArgs){ return MakeRefCounted<_T>(aArgs...); } \
            protected:
} // sbx

#endif // __SBX_FREFCOUNTED_H__
