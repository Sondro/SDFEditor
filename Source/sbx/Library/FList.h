/*
 * @file    FList.h
 * @author  David Gallardo Moreno
 * @brief   sbx map
 */

#ifndef __SBX_LIBRARY_LIST_H__
#define __SBX_LIBRARY_LIST_H__

#include <sbx/Common.h>
#include <sbx/Library/TypeTraits.h>
#include <sbx/Library/FObjectPool.h>

namespace sbx
{
    template <typename T>
    class FList
    {
    public:
        struct  TIterator;
        struct  TConstIterator;
        typedef T               value_type;
        typedef T &             TReference;
        typedef T const &       TConstReference;


                        FList     ();
                       ~FList     ();
                        FList     (const FList<T> & aOther);
                        FList     (      FList<T> && aOther);

        FList<T>&     operator=   (const FList<T> & aOther);
        FList<T>&     operator=   (      FList<T> && aOther);

        //Iterator
        TIterator       begin       ();
        TConstIterator  begin       () const;
        TIterator       end         ();
        TConstIterator  end         () const;

        //Capacity
        uint32_t        size32      () const;
        size_t          size        () const;
        bool            empty       () const;

        TReference      front       ();
        TConstReference front       () const;
        TReference      back        ();
        TConstReference back        () const;

        //Modifiers
        void            push_back   (T const & aElement);
        void            push_back   (T&& aElement);
        void            push_front  (T const & aElement);
        void            push_front  (T&& aElement);
        void            pop_back    ();
        void            pop_front   ();
        TIterator       insert      (TConstIterator aIterator, T const & aElement);
        TIterator       insert      (TConstIterator aIterator, T&& aElement);
        TIterator       erase       (TConstIterator aIterator);
        void            clear       ();
        void            swap        (FList<T> & aOther);

        template <typename... Args>
        void            emplace_back    (Args... args);

    private:
        struct TNode;
        void            CopyFrom    (const FList<T> & aOther);
        void            MoveFrom    (      FList<T> & aOther);
        TNode*          AllocNode   (T const & aObject);
        TNode*          AllocNode   (T&& aObject);
        void            FreeNode    (TNode* aNode);

    private:
        TNode   mHead;
        size_t  mElementsCount;
        FObjectPool< TNode > mNodesPool;
    };

    template <typename T>
    struct FList<T>::TNode
    {
        TNode* mNext;
        TNode* mPrev;
        T mValue;

        TNode()
            : mValue()
            , mNext(this)
            , mPrev(this)
        {
        }

        TNode(T const & aValue)
            : mValue(aValue)
            , mNext(this)
            , mPrev(this)
        {
        }

        TNode(T&& aValue)
            : mValue(MoveCast(aValue))
            , mNext(this)
            , mPrev(this)
        {
        }
    };

    template <typename T>
    struct FList<T>::TIterator
    {
        typedef T               value_type;
        typedef T&              TReference;
        typedef T const&        TConstReference;

        TIterator(TNode* aNode, FList<T>* aParentList)
            : mNodePtr(aNode)
            , mParentListPtr(aParentList)
        {
        }

        bool operator!=(TIterator const & aOther) const
        {
            return aOther.mNodePtr != mNodePtr;
        }

        TIterator& operator++()
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr != &mParentListPtr->mHead);
            if(mNodePtr && (mNodePtr != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mNext;
            }

            return *this;
        }

        TIterator operator++(int32_t)
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr != &mParentListPtr->mHead);
            TIterator lCopy = *this;
            if(mNodePtr && (mNodePtr != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mNext;
            }

            return lCopy;
        }

        TIterator& operator--()
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr->mPrev != &mParentListPtr->mHead);
            if(mNodePtr && (mNodePtr->mPrev != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mPrev;
            }

            return *this;
        }
            
        TIterator operator--(int32_t)
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr->mPrev != &mParentListPtr->mHead);
            TIterator lCopy = *this;
            if(mNodePtr && (mNodePtr->mPrev != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mPrev;
            }

            return lCopy;
        }

        T* operator->()
        {
            return &mNodePtr->mValue;
        }

        T& operator*() 
        {
            return mNodePtr->mValue;
        }

    private:
        using TNodeType = typename FList<T>::TNode;
        mutable TNodeType* mNodePtr;
        FList<T>* mParentListPtr;

        friend class FList<T>;
        friend typename FList<T>::TConstIterator;
    };

    template <typename T>
    struct FList<T>::TConstIterator
    {
        TConstIterator(typename FList<T>::TNode* aNode, FList<T> const * aParentList)
            : mNodePtr(aNode)
            , mParentListPtr(aParentList)
        {
        }

        TConstIterator(typename FList<T>::TIterator const & aOther)
            : mNodePtr(aOther.mNodePtr)
            , mParentListPtr(aOther.mParentListPtr)
        {
        }

        bool operator!=(TConstIterator const & aOther) const
        {
            return aOther.mNodePtr != mNodePtr;
        }

        TConstIterator& operator++()
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr != &mParentListPtr->mHead);
            if(mNodePtr && (mNodePtr != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mNext;
            }

            return *this;
        }

        TConstIterator operator++(int32_t)
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr != &mParentListPtr->mHead);
            TConstIterator lCopy = *this;
            if(mNodePtr && (mNodePtr != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mNext;
            }

            return lCopy;
        }

        TConstIterator& operator--()
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr->mPrev != &mParentListPtr->mHead);
            if(mNodePtr && (mNodePtr->mPrev != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mPrev;
            }
        }
            
        TConstIterator operator--(int32_t)
        {
            SBX_ASSERT(mNodePtr);
            SBX_ASSERT(mNodePtr->mPrev != &mParentListPtr->mHead);
            TConstIterator lCopy = *this;
            if(mNodePtr && (mNodePtr->mPrev != &mParentListPtr->mHead))
            {
                mNodePtr = mNodePtr->mPrev;
            }

            return lCopy;
        }

        T const * operator->()
        {
            return &mNodePtr->mValue;
        }

        T const & operator*()
        {
            return mNodePtr->mValue;
        }

    private:
        using TNodeType = typename FList<T>::TNode;
        mutable TNodeType* mNodePtr;
        FList<T> const * mParentListPtr;

        friend class FList<T>;
    };

    //Constructors / Destructor / Copy / Move
    template <typename T> 
    FList<T>::FList()
        : mElementsCount(0)
    {}

    template <typename T> 
    FList<T>::~FList()
    {
        clear();
    }

    template <typename T> 
    FList<T>::FList(const FList<T> & aOther)
        : mElementsCount(0)
    {
        CopyFrom(aOther);
    }
    
    template <typename T> 
    FList<T>::FList(FList<T> && aOther)
        : mElementsCount(0)
    {
        //SBX_ERROR("Move constructor for FList not inmplemented.");
        static_assert("Move constructor for FList not inmplemented.");
        MoveFrom(aOther);
    }

    template <typename T> 
    FList<T>& FList<T>::operator=(const FList<T> & aOther)
    {
        CopyFrom(aOther);
        return *this;
    }

    template <typename T> 
    FList<T>& FList<T>::operator=(FList<T> && aOther)
    {
        MoveFrom(aOther);
        return *this;
    }

    //Iterator
    template <typename T>
    typename FList<T>::TIterator FList<T>::begin ()
    {
        return TIterator(mHead.mNext, this);
    }
    
    template <typename T>
    typename FList<T>::TConstIterator FList<T>::begin () const
    {
        return TConstIterator(const_cast<TNode*>(mHead.mNext), this);
    }
    
    template <typename T>
    typename FList<T>::TIterator FList<T>::end ()
    {
        return TIterator(&mHead, this);
    }
    
    template <typename T>
    typename FList<T>::TConstIterator FList<T>::end () const
    {
        return TConstIterator(const_cast<TNode*>(&mHead), this);
    }
    
    //Capacity
    template <typename T>
    uint32_t FList<T>::size32 () const
    {
        return mElementsCount & 0xFFFFFFFF;
    }

    template <typename T>
    size_t FList<T>::size () const
    {
        return mElementsCount;
    }

    template <typename T>
    bool FList<T>::empty() const
    {
        return mElementsCount == 0;
    }

    template <typename T>
    typename FList<T>::TReference FList<T>::front ()
    {
        SBX_ASSERT(mHead.mNext != &mHead, "Calling front() in an empty list");
        return mHead.mNext->mValue;
    }

    template <typename T>
    typename FList<T>::TConstReference FList<T>::front () const
    {
        SBX_ASSERT(mHead.mNext != &mHead, "Calling front() in an empty list");
        return mHead.mNext->mValue;
    }

    template <typename T>
    typename FList<T>::TReference FList<T>::back ()
    {
        SBX_ASSERT(mHead.mNext != &mHead, "Calling back() in an empty list");
        return mHead.mPrev->mValue;
    }

    template <typename T>
    typename FList<T>::TConstReference FList<T>::back () const
    {
        SBX_ASSERT(mHead.mNext != &mHead, "Calling back() in an empty list");
        return mHead.mPrev->mValue;
    }

    //Modifiers
    template <typename T>
    void FList<T>::push_back (const T& aElement)
    {
        insert(end(), aElement);
    }

    template <typename T>
    void FList<T>::push_back (T&& aElement)
    {
        insert(end(), MoveCast(aElement));
    }

    template <typename T>
    void FList<T>::push_front (const T& aElement)
    {
        insert(begin(), aElement);
    }

    template <typename T>
    void FList<T>::push_front (T&& aElement)
    {
        insert(begin(), MoveCast(aElement));
    }

    template <typename T>
    void FList<T>::pop_back ()
    {
        if(mHead.mPrev != &mHead)
        {
            erase(TIterator(mHead.mPrev, this));
        }
    }

    template <typename T>
    void FList<T>::pop_front ()
    {
        if(mHead.mNext != &mHead)
        {
            erase(TIterator(mHead.mNext, this));
        }
    }

    template <typename T>
    typename FList<T>::TIterator FList<T>::insert (TConstIterator aIterator, T const & aElement)
    {
        TNode* lNewNode = AllocNode(aElement);
        lNewNode->mNext = aIterator.mNodePtr;
        lNewNode->mPrev = aIterator.mNodePtr->mPrev;
        aIterator.mNodePtr->mPrev->mNext = lNewNode;
        aIterator.mNodePtr->mPrev = lNewNode;
        ++mElementsCount;
        return TIterator(lNewNode, this);
    }

    template <typename T>
    typename FList<T>::TIterator FList<T>::insert (TConstIterator aIterator, T&& aElement)
    {
        TNode* lNewNode = AllocNode(MoveCast(aElement));
        lNewNode->mNext = aIterator.mNodePtr;
        lNewNode->mPrev = aIterator.mNodePtr->mPrev;
        aIterator.mNodePtr->mPrev->mNext = lNewNode;
        aIterator.mNodePtr->mPrev = lNewNode;
        ++mElementsCount;
        return TIterator(lNewNode, this);
    }

    template <typename T>
    typename FList<T>::TIterator FList<T>::erase (TConstIterator aIterator)
    {
        SBX_ASSERT(aIterator.mNodePtr != &mHead, "Trying to erase internal head node");
        if(aIterator.mNodePtr != &mHead)
        {
            TNode* lToRemoveNode = aIterator.mNodePtr;
            lToRemoveNode->mPrev->mNext = lToRemoveNode->mNext;
            lToRemoveNode->mNext->mPrev = lToRemoveNode->mPrev;
            TNode* lNextNode = lToRemoveNode->mNext;
            FreeNode(lToRemoveNode);
            --mElementsCount;
            return TIterator(lNextNode, this);
        }

        return TIterator(&mHead, this);
    }

    template <typename T>
    void FList<T>::clear ()
    {
        TNode* lNext = mHead.mNext;
        while(lNext != &mHead)
        {
            TNode* lToDelete = lNext;
            lNext = lNext->mNext;
            FreeNode(lToDelete);   
        }

        mHead.mNext = &mHead;
        mHead.mPrev = &mHead;
        mElementsCount = 0;
    }

    template <typename T>
    void FList<T>::swap(FList<T> & aOther)
    {
        MoveFrom(aOther);
    }

    
    template <typename T>
    template <typename... Args>
    void FList<T>::emplace_back (Args... aArgs)
    {
        push_back(MoveCast(T(aArgs...)));
    }

    template <typename T>
    void FList<T>::CopyFrom (const FList<T> & aOther)
    {
        clear();
        for(auto lObject : aOther)
        {
            push_back(lObject);
        }
    }

    template <typename T>
    void FList<T>::MoveFrom (FList<T> & aOther)
    {
        Swap(mHead, aOther.mHead);
        Swap(mElementsCount, aOther.mElementsCount);
    }

    template <typename T>
    typename FList<T>::TNode* FList<T>::AllocNode (T const & aObject)
    {
        if(!mNodesPool.IsOk())
        {
            mNodesPool.Init(32);
        }
        using TNodeType = typename FList<T>::TNode;
        //return SBX_NEW(TNodeType, aObject);
        return mNodesPool.AllocOjbect(aObject);
    }

    template <typename T>
    typename FList<T>::TNode* FList<T>::AllocNode (T&& aObject)
    {
        if(!mNodesPool.IsOk())
        {
            mNodesPool.Init(32);
        }
        using TNodeType = typename FList<T>::TNode;
        //return SBX_NEW(TNodeType, MoveCast(aObject));
        return mNodesPool.AllocOjbect(MoveCast(aObject));
    }

    template <typename T>
    void FList<T>::FreeNode (TNode* aNode)
    {
        //SBX_DELETE(aNode);
        mNodesPool.FreeObject(aNode);
    }
} // sbx

#endif // __SBX_LIBRARY_LIST_H__