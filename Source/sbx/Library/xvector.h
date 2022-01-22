/*
 * @file    xvector.h
 * @author  David Gallardo Moreno
 * @brief   pod aware container
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <type_traits>

namespace nostd
{
    template <typename _Type, bool _IsPod = std::is_trivial<_Type>() && std::is_standard_layout<_Type>()> 
    class xvector
    {
    public:
        using iterator              = _Type*;
        using const_iterator        = _Type const *;
        using value_type            = _Type;
        using reference             = _Type &; 
        using const_reference       = _Type const &;
        inline static bool const is_pod   = _IsPod;

        //Constructors / Destructor / Copy / Move
                        xvector     ();
                       ~xvector     ();
                        xvector     (xvector<_Type, _IsPod> const &  aOther);
                        xvector     (xvector<_Type, _IsPod>       && aOther);
                        xvector     (std::initializer_list<_Type> aList);

        xvector<_Type, _IsPod> &    operator=   (xvector<_Type, _IsPod> const &  aOther);
        xvector<_Type, _IsPod> &    operator=   (xvector<_Type, _IsPod>       && aOther);

        //Iterator
        iterator        begin       ();
        const_iterator  begin       () const;
        iterator        end         ();
        const_iterator  end         () const;

        //Capacity
        uint32_t        size32      () const;
        size_t          size        () const;
        size_t          capacity    () const;
        bool            empty       () const;
        void            reserve     (size_t aNumElements);

        //Element access
        reference       operator[]  (size_t aIndex);
        const_reference operator[]  (size_t aIndex) const;
        reference       front       ();
        const_reference front       () const;
        reference       back        ();
        const_reference back        () const;
        _Type*          data        ()          { return reinterpret_cast<_Type*       > (mBuffer); }
        _Type const *   data        () const    { return reinterpret_cast<_Type const *> (mBuffer); }

        //Modifiers
        void            push_back   (const _Type& aElement);
        void            pop_back    ();
        void            erase       (iterator aIterator);
        void            erase_fast  (iterator aIterator);
        void            clear       ();
        void            swap        (xvector<_Type, _IsPod> & aOther);

        template <typename... Args>
        reference      emplace_back (Args... args);

    private:

        void            CopyFrom    (const xvector<_Type, _IsPod> & aOther);
        void            MoveFrom    (      xvector<_Type, _IsPod> & aOther);

    private:
        uint8_t*    mBuffer;
        size_t      mBufferSize;
        size_t      mElementsCount;
    };


    //Constructors / Destructor / Copy / Move
    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>::xvector()
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {}

    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>::~xvector()
    {
        clear();
        if(mBuffer)
        {
            ::free(mBuffer);
            mBuffer = nullptr;
            mBufferSize = 0;
            mElementsCount = 0;
        }
    }

    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>::xvector(const xvector<_Type, _IsPod> & aOther)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        CopyFrom(aOther);
    }
    
    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>::xvector(xvector<_Type, _IsPod> && aOther)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        MoveFrom(aOther);
    }
    
    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>::xvector(std::initializer_list<_Type> aList)
        : mBuffer(nullptr)
        , mBufferSize(0)
        , mElementsCount(0)
    {
        for(auto lElement : aList)
        {
            PushBack(lElement);
        }
    }

    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>& xvector<_Type, _IsPod>::operator=(const xvector<_Type, _IsPod> & aOther)
    {
        CopyFrom(aOther);
        return *this;
    }

    template <typename _Type, bool _IsPod> 
    xvector<_Type, _IsPod>& xvector<_Type, _IsPod>::operator=(xvector<_Type, _IsPod> && aOther)
    {
        if constexpr (!_IsPod)
        {
            clear();
        }

        ::free(mBuffer);
        mBuffer = nullptr;
        mBufferSize = 0;

        MoveFrom(aOther);
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::CopyFrom(const xvector<_Type, _IsPod> & aOther)
    {
        if constexpr (!_IsPod)
        {
            clear();
        }
        
        reserve(aOther.mElementsCount);

        if constexpr (_IsPod)
        {
            ::memcpy(mBuffer, aOther.mBuffer, aOther.mElementsCount * sizeof(_Type));
            mElementsCount = aOther.mElementsCount;
        }
        else
        {
            for(const _Type& lElement : aOther)
            {
                push_back(lElement);
            }
        }
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::MoveFrom(xvector<_Type, _IsPod> & aOther)
    {
        //TODO: proper way to do a move?
        assert(mBuffer == nullptr);

        mBuffer         = aOther.mBuffer;
        mBufferSize     = aOther.mBufferSize;
        mElementsCount  = aOther.mElementsCount;

        aOther.mBuffer          = nullptr;
        aOther.mBufferSize      = 0;
        aOther.mElementsCount   = 0;
    }

    //Iterator
    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::iterator xvector<_Type, _IsPod>::begin ()
    {
        return data();
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::const_iterator xvector<_Type, _IsPod>::begin () const
    {
        return data();
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::iterator xvector<_Type, _IsPod>::end ()
    {
        return data() + mElementsCount;
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::const_iterator xvector<_Type, _IsPod>::end () const
    {
        return data() + mElementsCount;
    }

    //Capacity
    template <typename _Type, bool _IsPod>
    uint32_t xvector<_Type, _IsPod>::size32 () const
    {
        return mElementsCount & 0xFFFFFFFF;
    }

    template <typename _Type, bool _IsPod>
    size_t xvector<_Type, _IsPod>::size () const
    {
        return mElementsCount;
    }

    template <typename _Type, bool _IsPod>
    size_t xvector<_Type, _IsPod>::capacity () const
    {
        return mBufferSize / sizeof(_Type);
    }

    template <typename _Type, bool _IsPod>
    bool xvector<_Type, _IsPod>::empty () const
    {
        return mElementsCount == 0;
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::reserve (size_t aNumElements)
    {
        if(capacity() < aNumElements)
        {
            size_t const lNewBufferSize = aNumElements * sizeof(_Type);
            _Type* lNewBuffer = reinterpret_cast<_Type*>( ::malloc(lNewBufferSize) );

            if constexpr (_IsPod)
            {
                if(mElementsCount > 0)
                {
                    ::memcpy(lNewBuffer, mBuffer, mBufferSize);
                }
            }
            else
            {
                for(size_t i = 0, l = mElementsCount; i < l; ++i)
                {
                    new (&lNewBuffer[i]) _Type(std::move(data()[i]));
                    data()[i].~_Type();
                }
            }

            if(mBuffer)
            {
                ::free(mBuffer);
            }

            mBuffer = reinterpret_cast<uint8_t*>(lNewBuffer);
            mBufferSize = lNewBufferSize;
        }
    }

    //Element access
    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::reference xvector<_Type, _IsPod>::operator[] (size_t aIndex)
    {
        assert(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::const_reference xvector<_Type, _IsPod>::operator[] (size_t aIndex) const
    {
        assert(aIndex < mElementsCount);
        return *(data() + aIndex);
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::reference xvector<_Type, _IsPod>::front ()
    {
        return *data();
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::const_reference xvector<_Type, _IsPod>::front () const
    {
        return *data();
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::reference xvector<_Type, _IsPod>::back ()
    {
        return *(data() + mElementsCount - 1);
    }

    template <typename _Type, bool _IsPod>
    typename xvector<_Type, _IsPod>::const_reference xvector<_Type, _IsPod>::back () const
    {
        return *(data() + mElementsCount - 1);
    }

    //Modifiers
    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::push_back (const _Type& aElement)
    {
        if(size() == capacity())
        {
            reserve(size() + 16);
        }

        new (end()) _Type(aElement);

        ++mElementsCount;
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::pop_back ()
    {
        back().~_Type();
        --mElementsCount;
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::erase (iterator aIterator)
    {
        assert(aIterator >= begin() && aIterator < end());

        if constexpr (!_IsPod)
        {
            aIterator->~_Type();
            for(auto lIt = aIterator; (lIt + 1) != end(); ++lIt)
            {
                new (lIt) _Type(std::move(*(lIt + 1)));
            }
        }

        --mElementsCount;
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::erase_fast (iterator aIterator)
    {
        assert(aIterator >= begin() && aIterator < end());
        
        if constexpr (!_IsPod)
        {
            aIterator->~_Type();
        }

        *aIterator = *reinterpret_cast<_Type*>(end() - 1);
        --mElementsCount;
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::clear ()
    {
        if constexpr (!_IsPod)
        {
            for(auto lIt = begin(); lIt != end(); ++lIt)
            {
                lIt->~_Type();
            }
        }

        mElementsCount = 0;
    }

    template <typename _Type, bool _IsPod>
    void xvector<_Type, _IsPod>::swap (xvector<_Type, _IsPod> & aOther)
    {
        xvector<_Type, _IsPod> lTemp(std::move(*this));
        MoveFrom(aOther);
        aOther.MoveFrom(lTemp);
    }

    template <typename _Type, bool _IsPod>
    template <typename... Args>
    typename xvector<_Type, _IsPod>::reference xvector<_Type, _IsPod>::emplace_back (Args... aArgs)
    {
        if(size() == capacity())
        {
            reserve(size() + 16);
        }
        new  (end()) _Type(aArgs...);
        ++mElementsCount;

        return back();
    }
} // nostd
