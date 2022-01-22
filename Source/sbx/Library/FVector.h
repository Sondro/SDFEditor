/*
 * @file    FVector.h
 * @author  David Gallardo Moreno
 * @brief   sbx vector
 */

#ifndef __SBX_VECTOR_H__
#define __SBX_VECTOR_H__

#include <sbx/Common.h>
#include <sbx/Library/TypeTraits.h>

namespace sbx
{
    template <typename T> class FVector
    {
    public:
        typedef T*          TIterator;
        typedef T const *   TConstIterator;
        typedef T           TValueType;
        typedef T &         TReference;
        typedef T const &   TConstReference;

        //Constructors / Destructor / Copy / Move
                        FVector     ();
                       ~FVector     ();
                        FVector     (const FVector<T> & aOther);
                        FVector     (      FVector<T> && aOther);
                        FVector     (std::initializer_list<T> aList);

        FVector<T>&     operator=   (const FVector<T> & aOther);
        FVector<T>&     operator=   (      FVector<T> && aOther);

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
        TReference      at          (size_t aIndex);
        TConstReference at          (size_t aIndex) const;
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
        void            push_back   (T&& aElement);
        void            pop_back    ();
        void            erase       (TIterator aIterator);
        void            erase_fast  (TIterator aIterator);
        void            clear       ();
        void            swap        (FVector<T> & aOther);
        void            append      (FVector<T> & aOther);

        template <typename... Args>
        TReference      emplace_back (Args... args);

    private:

        void            CopyFrom    (const FVector<T> & aOther);
        void            MoveFrom    (      FVector<T> & aOther);

    private:
        uint8_t*    mBuffer;
        size_t      mBufferSize;
        size_t      mElementsCount;
    };

    //Free iterators for ranged for
    template <typename T> T*        begin       (FVector<T> & aVector)          { return aVector.begin(); };
    template <typename T> const T*  begin       (FVector<T> const & aVector)    { return aVector.begin(); };
    template <typename T> T*        end         (FVector<T> & aVector)          { return aVector.end(); };
    template <typename T> const T*  end         (FVector<T> const & aVector)    { return aVector.end(); };

    //TODO: separate in two clases, partially specialized by IsPod

    //Constructors / Destructor / Copy / Move
    template <typename T> 
    FVector<T>::FVector()
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {}

    template <typename T> 
    FVector<T>::~FVector()
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

    template <typename T> 
    FVector<T>::FVector(const FVector<T> & aOther)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        CopyFrom(aOther);
    }
    
    template <typename T> 
    FVector<T>::FVector(FVector<T> && aOther)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        MoveFrom(aOther);
    }
    
    template <typename T> 
    FVector<T>::FVector(std::initializer_list<T> aList)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        for(auto lElement : aList)
        {
            push_back(lElement);
        }
    }

    template <typename T> 
    FVector<T>& FVector<T>::operator=(const FVector<T> & aOther)
    {
        CopyFrom(aOther);
        return *this;
    }

    template <typename T> 
    FVector<T>& FVector<T>::operator=(FVector<T> && aOther)
    {
        MoveFrom(aOther);
        return *this;
    }

    template <typename T>
    void FVector<T>::CopyFrom(const FVector<T> & aOther)
    {
        clear();
        reserve(aOther.mElementsCount);
        for(const T& lElement : aOther)
        {
            push_back(lElement);
        }
    }

    template <typename T>
    void FVector<T>::MoveFrom(FVector<T> & aOther)
    {
        //TODO: check if this is the proper way to do a move
        auto lTempBuffer = mBuffer;
        auto lTempBufferSize = mBufferSize;
        auto lTempElementsCount = mElementsCount;

        mBuffer = aOther.mBuffer;
        mBufferSize = aOther.mBufferSize;
        mElementsCount = aOther.mElementsCount;

        aOther.mBuffer = lTempBuffer;
        aOther.mBufferSize = lTempBufferSize;
        aOther.mElementsCount = lTempElementsCount;
    }

    //Iterator
    template <typename T>
    typename FVector<T>::TIterator FVector<T>::begin ()
    {
        return data();
    }

    template <typename T>
    typename FVector<T>::TConstIterator FVector<T>::begin () const
    {
        return data();
    }

    template <typename T>
    typename FVector<T>::TIterator FVector<T>::end ()
    {
        return data() + mElementsCount;
    }

    template <typename T>
    typename FVector<T>::TConstIterator FVector<T>::end () const
    {
        return data() + mElementsCount;
    }

    //Capacity
    template <typename T>
    uint32_t FVector<T>::size32 () const
    {
        return mElementsCount & 0xFFFFFFFF;
    }

    template <typename T>
    size_t FVector<T>::size () const
    {
        return mElementsCount;
    }

    template <typename T>
    size_t FVector<T>::capacity () const
    {
        return mBufferSize / sizeof(T);
    }

    template <typename T>
    bool FVector<T>::empty () const
    {
        return mElementsCount == 0;
    }

    template <typename T>
    void FVector<T>::reserve (size_t aNumElements)
    {
        if(capacity() < aNumElements)
        {
            const size_t lNewBufferSize = aNumElements * sizeof(T);
            T* lBuffer = (T*)SBX_MALLOC(lNewBufferSize);
            for(size_t i = 0, l = mElementsCount; i < l; ++i)
            {
                SBX_PLACEMENT_NEW(T, &lBuffer[i], MoveCast(data()[i]));
                data()[i].~T();
            }

            if(mBuffer)
            {
                SBX_FREE(mBuffer);
            }

            mBuffer = reinterpret_cast<uint8_t*>(lBuffer);
            mBufferSize = lNewBufferSize;
        }
    }

    //Element access
    template <typename T>
    typename FVector<T>::TReference FVector<T>::at (size_t aIndex)
    {
        SBX_ASSERT(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename T>
    typename FVector<T>::TConstReference FVector<T>::at (size_t aIndex) const
    {
        SBX_ASSERT(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename T>
    typename FVector<T>::TReference FVector<T>::operator[] (size_t aIndex)
    {
        SBX_ASSERT(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename T>
    typename FVector<T>::TConstReference FVector<T>::operator[] (size_t aIndex) const
    {
        SBX_ASSERT(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename T>
    typename FVector<T>::TReference FVector<T>::front ()
    {
        SBX_ASSERT(data() != nullptr, "Trying to access element from an uninitialized FVector");
        SBX_ASSERT(mElementsCount > 0, "Trying to access element from an empty FVector");
        return *data();
    }

    template <typename T>
    typename FVector<T>::TConstReference FVector<T>::front () const
    {
        SBX_ASSERT(data() != nullptr, "Trying to access element from an uninitialized FVector");
        SBX_ASSERT(mElementsCount > 0, "Trying to access element from an empty FVector");
        return *data();
    }

    template <typename T>
    typename FVector<T>::TReference FVector<T>::back ()
    {
        SBX_ASSERT(data() != nullptr, "Trying to access element from an uninitialized FVector");
        SBX_ASSERT(mElementsCount > 0, "Trying to access element from an empty FVector");
        return *(data() + mElementsCount - 1);
    }

    template <typename T>
    typename FVector<T>::TConstReference FVector<T>::back () const
    {
        SBX_ASSERT(data() != nullptr, "Trying to access element from an uninitialized FVector");
        SBX_ASSERT(mElementsCount > 0, "Trying to access element from an empty FVector");
        return *(data() + mElementsCount - 1);
    }

    //Modifiers
    template <typename T>
    void FVector<T>::push_back (const T& aElement)
    {
        if(size() == capacity())
        {
            reserve(size() + 16);
        }
        SBX_PLACEMENT_NEW(T, end(), aElement);
        ++mElementsCount;
    }

    template <typename T>
    void FVector<T>::push_back (T&& aElement)
    {
        if(size() == capacity())
        {
            reserve(size() + 16);
        }
        SBX_PLACEMENT_NEW(T, end(), MoveCast(aElement));
        ++mElementsCount;
    }

    template <typename T>
    void FVector<T>::pop_back ()
    {
        SBX_ASSERT(back() != nullptr, "Trying to pop an element from an uninitialized FVector");
        SBX_ASSERT(mElementsCount > 0, "Trying to pop an element from an empty FVector");
        if( (back() != nullptr) && (mElementsCount > 0))
        {
            back().~T();
            --mElementsCount;
        }
    }

    template <typename T>
    void FVector<T>::erase (TIterator aIterator)
    {
        SBX_ASSERT((aIterator >= begin()) && (aIterator < end()), "Out of range iterator");
        aIterator->~T();
        for(auto lIt = aIterator; (lIt + 1) != end(); ++lIt)
        {
            SBX_PLACEMENT_NEW(T, lIt, MoveCast(*(lIt + 1)));
        }
        --mElementsCount;
    }

    template <typename T>
    void FVector<T>::erase_fast (TIterator aIterator)
    {
        SBX_ASSERT((aIterator >= begin()) && (aIterator < end()), "Out of range iterator");
        aIterator->~T();
        if(aIterator != (end() - 1))
        {
            SBX_PLACEMENT_NEW(T, aIterator, MoveCast(back()));
        }
        --mElementsCount;
    }

    template <typename T>
    void FVector<T>::clear ()
    {
        for(auto lIt = begin(); lIt != end(); ++lIt)
        {
            lIt->~T();
        }
        mElementsCount = 0;
    }

    template <typename T>
    void FVector<T>::swap(FVector<T> & aOther)
    {
        MoveFrom(aOther);
    }

    
    template <typename T>
    void FVector<T>::append(FVector<T> & aOther)
    {
        reserve(size() + aOther.size());

        for(auto lElement : aOther)
        {
            push_back(lElement);
        }
    }

    template <typename T>
    template <typename... Args>
    typename FVector<T>::TReference FVector<T>::emplace_back (Args... aArgs)
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