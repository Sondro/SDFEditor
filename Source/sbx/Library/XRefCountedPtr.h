/*
 * @file    XRefCountedPtr.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_XREFCOUNTEDPTR_H__
#define __SBX_XREFCOUNTEDPTR_H__

#include <sbx/Common.h>

namespace sbx
{
    //-----------------------------------------------------------------------
    // XRefCountedPtr
    //-----------------------------------------------------------------------
    template <typename T> 
    class XRefCountedPtr
    {
    public:
        struct XType : public T 
        {
            public:
                        XType () 
                            : mRefCount(0) 
                        {;}

            void        AddRef      () { ++mRefCount; /*SBX_LOG("[XRefCountedPtr] Add Ref to %p [%d]", this, mRefCount);*/ }
            void        RemoveRef   () { --mRefCount; /*SBX_LOG("[XRefCountedPtr] Remove Ref to %p [%d]", this, mRefCount);*/ }
            uint32_t    GetRefCount () const { return mRefCount; } 
            void        Reset       () { mRefCount = 0; }
        private:
            uint32_t mRefCount;
        };

        static void* Alloc()
        {
            return SBX_MALLOC(sizeof(XType));
        }

        XRefCountedPtr ()
            : mPtr(nullptr)
        {;}

        XRefCountedPtr  (XRefCountedPtr const & aOther)
            : mPtr(reinterpret_cast<XType*>(aOther.mPtr))
        {
            if(nullptr != mPtr)
            {
                mPtr->AddRef();
            }
        };

        XRefCountedPtr  (T* aPtr)
            : mPtr(reinterpret_cast<XType*>(aPtr))
        {
            if(nullptr != mPtr)
            {
                mPtr->AddRef();
            }
        };

        ~XRefCountedPtr ()
        {
            if(nullptr != mPtr)
            {
                mPtr->RemoveRef();
                CheckPtr();
                mPtr = nullptr;
            }
        };

        XRefCountedPtr & operator=(XRefCountedPtr const & aOther)
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

        XRefCountedPtr & operator=(T* aPtr)
        {
            if(aPtr != mPtr)
            {
                if(nullptr != mPtr)
                {
                    mPtr->RemoveRef();
                    CheckPtr();
                }

                mPtr = reinterpret_cast<XType*>(aPtr);

                if(nullptr != mPtr)
                {
                    mPtr->AddRef();
                }
            }

            return *this;
        }

        inline             operator bool   () const    { return (mPtr != nullptr) && (mPtr->GetRefCount() > 0); }
        inline T *         operator->      () const    { return mPtr;  }
        inline T &         operator*       ()          { return *mPtr; }
        inline T const &   operator*       () const    { return *mPtr; }
        inline T *         GetPtr          () const    { return mPtr;  }
        inline bool        operator==      (XRefCountedPtr const & aOther) const { return mPtr == aOther.mPtr;   }
        inline bool        operator!=      (XRefCountedPtr const & aOther) const { return mPtr != aOther.mPtr;   }
        inline bool        operator<       (XRefCountedPtr const & aOther) const { return mPtr <  aOther.mPtr;   }
    private:
        void CheckPtr()
        {
            if((nullptr != mPtr) && (mPtr->GetRefCount() == 0))
            {
                GetPtr()->~T();
                //SBX_LOG("[XRefCountedPtr] Free %p", mPtr);
                SBX_FREE(mPtr);
            }
        }

    private:
        XType* mPtr;
    };

    template <typename T, typename... TArgs>
    XRefCountedPtr<T> MakeXRefCounted(TArgs... aArgs)
    {
        T* lPtr = SBX_PLACEMENT_NEW(T, XRefCountedPtr<T>::Alloc(), aArgs...);
        reinterpret_cast<typename XRefCountedPtr<T>::XType*>(lPtr)->Reset();
        return XRefCountedPtr<T>(lPtr);
    }

    /*
#   define  SBX_REFCOUNTED_MAKER(_T) \
            public: \
                using Ref = FRefCountedPtr<_T>; \
                template <typename... TArgs> static FRefCountedPtr<_T> CreateInstance(TArgs... aArgs){ return MakeRefCounted<_T>(aArgs...); } \
            protected:*/
} // sbx

#endif // __SBX_XREFCOUNTEDPTR_H__
