/*
 * @file    FContainer.h
 * @author  David Gallardo Moreno
 * @brief   private pod aware container for vector and other container classes 
 * TODO (WIP)
 */

#ifndef __SBX_CONTAINER_H__
#define __SBX_CONTAINER_H__


#include <sbx/Common.h>
#include <sbx/Library/TypeTraits.h>

namespace sbx
{
    template <typename T, bool IsPod = sbx::IsPod<T>()> class FContainer
    {
    public:
        typedef T*          TIterator;
        typedef T const *   TConstIterator;
        typedef T           TValueType;
        typedef T &         TReference;
        typedef T const &   TConstReference;

        //Constructors / Destructor / Copy / Move
                        FContainer     ();
                       ~FContainer     ();
                        FContainer     (const FContainer<T, IsPod> & aOther);
                        FContainer     (      FContainer<T, IsPod> && aOther);
                        FContainer     (std::initializer_list<T> aList);

        FContainer<T, IsPod>&   operator=   (const FContainer<T, IsPod> & aOther);
        FContainer<T, IsPod>&   operator=   (      FContainer<T, IsPod> && aOther);

        //Iterator
        TIterator       begin       ();
        TConstIterator  begin       () const;
        TIterator       end         ();
        TConstIterator  end         () const;

        //Capacity
        uint32_t        size32      () const;
        size_t          size        () const;
        size_t          capacity    () const;
        bool            empty       () const;
        void            reserve     (size_t aNumElements);

        //Element access
        TReference      operator[]  (size_t aIndex);
        TConstReference operator[]  (size_t aIndex) const;
        TReference      front       ();
        TConstReference front       () const;
        TReference      back        ();
        TConstReference back        () const;
        T*              data        () { return reinterpret_cast<T*>(mBuffer); }
        const T*        data        () const { return reinterpret_cast<const T*>(mBuffer); }

        //Modifiers
        void            push_back   (const T& aElement);
        void            pop_back    ();
        void            erase       (TIterator aIterator);
        void            erase_fast  (TIterator aIterator);
        void            clear       ();
        //TODO: swap

        template <typename... Args>
        TReference      emplace_back (Args... args);

    private:

        void            CopyFrom    (const FContainer<T, IsPod> & aOther);
        void            MoveFrom    (      FContainer<T, IsPod> & aOther);

    private:
        uint8_t*    mBuffer;
        size_t      mBufferSize;
        size_t      mElementsCount;
    };


    //Constructors / Destructor / Copy / Move
    template <typename T, bool IsPod> 
    FContainer<T, IsPod>::FContainer()
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {}

    template <typename T, bool IsPod> 
    FContainer<T, IsPod>::~FContainer()
    {
        clear();
        if(mBuffer)
        {
            SBX_FREE(mBuffer);
            mBuffer = nullptr;
            mBufferSize = 0;
            mElementsCount = 0;
        }
    }

    template <typename T, bool IsPod> 
    FContainer<T, IsPod>::FContainer(const FContainer<T, IsPod> & aOther)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        CopyFrom(aOther);
    }
    
    template <typename T, bool IsPod> 
    FContainer<T, IsPod>::FContainer(FContainer<T, IsPod> && aOther)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        MoveFrom(aOther);
    }
    
    template <typename T, bool IsPod> 
    FContainer<T, IsPod>::FContainer(std::initializer_list<T> aList)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        for(auto lElement : aList)
        {
            PushBack(lElement);
        }
    }

    template <typename T, bool IsPod> 
    FContainer<T, IsPod>& FContainer<T, IsPod>::operator=(const FContainer<T, IsPod> & aOther)
    {
        CopyFrom(aOther);
        return *this;
    }

    template <typename T, bool IsPod> 
    FContainer<T, IsPod>& FContainer<T, IsPod>::operator=(FContainer<T, IsPod> && aOther)
    {
        MoveFrom(aOther);
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::CopyFrom(const FContainer<T, IsPod> & aOther)
    {
        clear();
        reserve(aOther.mElementsCount);
        for(const T& lElement : aOther)
        {
            push_back(lElement);
        }
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::MoveFrom(FContainer<T, IsPod> & aOther)
    {
        //TODO: not the proper way to do a move
        FContainer<T, IsPod> lTemp;
        lTemp.mBuffer = mBuffer;
        lTemp.mBufferSize = mBufferSize;
        lTemp.mElementsCount = mElementsCount;

        mBuffer = aOther.mBuffer;
        mBufferSize = aOther.mBufferSize;
        mElementsCount = aOther.mElementsCount;

        aOther.mBuffer = lTemp.mBuffer;
        aOther.mBufferSize = lTemp.mBufferSize;
        aOther.mElementsCount = lTemp.mElementsCount;
    }

    //Iterator
    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TIterator FContainer<T, IsPod>::begin ()
    {
        return data();
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TConstIterator FContainer<T, IsPod>::begin () const
    {
        return data();
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TIterator FContainer<T, IsPod>::end ()
    {
        return data() + mElementsCount;
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TConstIterator FContainer<T, IsPod>::end () const
    {
        return data() + mElementsCount;
    }

    //Capacity
    template <typename T, bool IsPod>
    uint32_t FContainer<T, IsPod>::size32 () const
    {
        return mElementsCount & 0xFFFFFFFF;
    }

    template <typename T, bool IsPod>
    size_t FContainer<T, IsPod>::size () const
    {
        return mElementsCount;
    }

    template <typename T, bool IsPod>
    size_t FContainer<T, IsPod>::capacity () const
    {
        return mBufferSize - (mElementsCount * sizeof(T));
    }

    template <typename T, bool IsPod>
    bool FContainer<T, IsPod>::empty () const
    {
        return mElementsCount == 0;
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::reserve (size_t aNumElements)
    {
        if(capacity() < aNumElements)
        {
            T* lNewBuffer = reinterpret_cast<T*>( SBX_MALLOC(aNumElements * sizeof(T)) );

            if constexpr (IsPod)
            {
                ::memcpy(lNewBuffer, mBuffer, mBufferSize);
            }
            else
            {
                for(size_t i = 0, l = mElementsCount; i < l; ++i)
                {
                    SBX_PLACEMENT_NEW(T, &lNewBuffer[i], static_cast<T&&>(data()[i]));
                    data()[i].~T();
                }
            }

            if(mBuffer)
            {
                SBX_FREE(mBuffer);
            }

            mBuffer = reinterpret_cast<uint8_t*>(lNewBuffer);
            mBufferSize = aNumElements * sizeof(T);
        }
    }

    //Element access
    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TReference FContainer<T, IsPod>::operator[] (size_t aIndex)
    {
        SBX_ASSERT(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TConstReference FContainer<T, IsPod>::operator[] (size_t aIndex) const
    {
        SBX_ASSERT(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TReference FContainer<T, IsPod>::front ()
    {
        return *data();
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TConstReference FContainer<T, IsPod>::front () const
    {
        return *data();
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TReference FContainer<T, IsPod>::back ()
    {
        return *(data() + mElementsCount - 1);
    }

    template <typename T, bool IsPod>
    typename FContainer<T, IsPod>::TConstReference FContainer<T, IsPod>::back () const
    {
        return *(data() + mElementsCount - 1);
    }

    //Modifiers
    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::push_back (const T& aElement)
    {
        if(size() == capacity())
        {
            reserve(size() + 16);
        }

        SBX_PLACEMENT_NEW(T, end(), aElement);

        ++mElementsCount;
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::pop_back ()
    {
        back().~T();
        --mElementsCount;
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::erase (TIterator aIterator)
    {
        SBX_ASSERT(aIterator >= begin() && aIterator < end(), "Out of range iterator");

        if constexpr (!IsPod)
        {
            aIterator->~T();
            for(auto lIt = aIterator; (lIt + 1) != end(); ++lIt)
            {
                SBX_PLACEMENT_NEW(T, lIt, static_cast<T&&>(*(lIt + 1)));
            }
        }

        --mElementsCount;
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::erase_fast (TIterator aIterator)
    {
        SBX_ASSERT(aIterator >= begin() && aIterator < end(), "Out of range iterator");
        
        if constexpr (!IsPod)
        {
            aIterator->~T();
        }

        *aIterator = static_cast<T&&>(end() - 1);
        --mElementsCount;
    }

    template <typename T, bool IsPod>
    void FContainer<T, IsPod>::clear ()
    {
        if constexpr (!IsPod)
        {
            for(auto lIt = begin(); lIt != end(); ++lIt)
            {
                lIt->~T();
            }
        }

        mElementsCount = 0;
    }

    template <typename T, bool IsPod>
    template <typename... Args>
    typename FContainer<T, IsPod>::TReference FContainer<T, IsPod>::emplace_back (Args... aArgs)
    {
        if(size() == capacity())
        {
            reserve(size() + 16);
        }
        SBX_PLACEMENT_NEW(T, end(), aArgs...);
        ++mElementsCount;

        return back();
    }
} // sbx

#endif // __SBX_VECTOR_H__