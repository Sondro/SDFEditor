/*
 * @file    FSharedPtr.h
 * @author  David Gallardo Moreno
 */

#ifndef _SBX_FSHARED_PTR_H_
#define _SBX_FSHARED_PTR_H_

#include <sbx/Library/FVector.h> 

namespace sbx
{
    template <typename T> class FSharedPtr 
    {
    public:

        FSharedPtr()
            : mInstance(nullptr)
            , mCounter(nullptr)
        {;}

        ~FSharedPtr()
        {
            RemoveInstance();
        }

        FSharedPtr(FSharedPtr const & aOther)
            : mInstance(aOther.mInstance)
            , mCounter(aOther.mCounter)
        {
             AddInstance();
        }

        FSharedPtr & operator=(FSharedPtr const & aOther)
        {
            if(mInstance != aOther.mInstance)
            {
                RemoveInstance();
                mInstance = aOther.mInstance;
                mCounter = aOther.mCounter;
                AddInstance();
            }
            return *this;
        }

        FSharedPtr(nullptr_t aOtherPtr)
            : mInstance(nullptr)
            , mCounter(nullptr)
        {
        }

        FSharedPtr & operator=(nullptr_t aOther)
        {
            if(mInstance != aOther)
            {
                RemoveInstance();
            }
            mInstance = nullptr;
            mCounter = nullptr;
            return *this;
        }

    protected:
        FSharedPtr(T* aOtherPtr)
            : mInstance(aOtherPtr)
            , mCounter(nullptr)
        {
            AddInstance();
        }

        FSharedPtr & operator=(T* aOther)
        {
            if(mInstance != aOther)
            {
                RemoveInstance();
                mInstance = aOther;
                AddInstance();
            }
            return *this;
        }
        //friend template <typename T, typename ...Args> FSharedPtr<T> MakeShared(Args... args);
    public:
        T* GetPtr() const
        {
            return mInstance;
        }

        T& operator*() const
        {
            return *mInstance;
        }

        T* operator->() const
        {
            return mInstance;
        }

        operator bool() const
        {
            return 0 != mInstance;
        }

        bool operator==(FSharedPtr const & aOther) const
        {
            return mInstance == aOther.mInstance;
        }

        bool operator!=(FSharedPtr const & aOther) const
        {
            return mInstance != aOther.mInstance;
        }

        bool operator<(FSharedPtr const & aOther) const
        {
            return mInstance < aOther.mInstance;
        }

    private:

        void RemoveInstance()
        {
            if(mInstance != nullptr)
            {
                --(*mCounter);
                //SBX_LOG("[FSharedPtr] Remove Ref to %p [%d]", mInstance, *mCounter);
                CheckCount();
            }
        }

        void AddInstance()
        {
            if(mInstance != nullptr)
            {
                if(mCounter == nullptr)
                {
                    mCounter = SBX_NEW(size_t);
                    *mCounter = 0;
                }

                ++(*mCounter);
                //SBX_LOG("[FSharedPtr] Add Ref to %p [%d]", mInstance, *mCounter);
            }
        }

        void CheckCount()
        {
            if((mInstance != nullptr) && (mCounter != nullptr) && (*mCounter == 0))
            {
                //SBX_LOG("[FSharedPtr] DELETE %p", mInstance);
                SBX_DELETE(mInstance);
                SBX_DELETE(mCounter);
                mInstance = nullptr;
                mCounter = nullptr;
            }
            if((mInstance == nullptr) && (mCounter != nullptr))
            {
                SBX_ASSERT("[FSharedPtr::CheckCount] Invalid shared ptr data");
            }
        }

        //template <typename... TArgs> friend FSharedPtr< T > T::CreateInstance(TArgs... args);
        template <typename T1, typename... TArgs> friend FSharedPtr< T1 > MakeShared(TArgs... args);

    private:
        T*      mInstance;
        size_t* mCounter;
    };

    template <typename T, typename... TArgs> FSharedPtr<T> MakeShared(TArgs... args)
    {
        return FSharedPtr(SBX_NEW(T, args...));
    }

} //sbx
/*
#define SBX_SHARED_MAKER(_T) \
        public: \
        template <typename... TArgs> static FSharedPtr< _T > CreateInstance(TArgs... args) \
        { \
            return FSharedPtr< _T >(SBX_NEW(_T, args...)); \
        } \
        typedef sbx::FSharedPtr< _T > Ref;
        */

#define SBX_SHARED_MAKER(_T) \
        public: \
        typedef sbx::FSharedPtr< _T > Ref;

#endif