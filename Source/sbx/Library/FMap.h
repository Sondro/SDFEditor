/*
 * @file    FMap.h
 * @author  David Gallardo Moreno
 * @brief   sbx map
 */

#ifndef __SBX_LIBRARY_MAP_H__
#define __SBX_LIBRARY_MAP_H__

#include <sbx/Common.h>
#include <sbx/Library/TypeTraits.h>
#include <sbx/Library/FVector.h>

namespace sbx
{
    template <typename Key, typename T> 
    struct TPair
    {
        Key first;
        T   second;

        TPair(Key const & aFirst, T const & aSecond)
            : first(aFirst)
            , second(aSecond)
        {;}

        TPair(Key&& aFirst, T&& aSecond)
            : first(MoveCast(aFirst))
            , second(MoveCast(aSecond))
        {;}
    };

    template <typename Key, typename T>
    TPair<Key, T> MakePair(Key aKey, T aValue)
    {
        return TPair(aKey, aValue);
    }
    
    template <typename Key, typename T> 
    class FMap
    {
    public:
        typedef TPair< Key, T >*        TIterator;
        typedef TPair< Key, T > const * TConstIterator;
        typedef T                       TValueType;
        typedef T &                     TReference;
        typedef T const &               TConstReference;

                        FMap        ();
                       ~FMap        ();
                        FMap        (const FMap<Key, T> & aOther);
                        FMap        (      FMap<Key, T> && aOther);

        FMap<Key, T>&   operator=   (const FMap<Key, T> & aOther);
        FMap<Key, T>&   operator=   (      FMap<Key, T> && aOther);

        //Iterator
        TIterator       begin       ();
        TConstIterator  begin       () const;
        TIterator       end         ();
        TConstIterator  end         () const;

        TReference      operator[]  (Key const & aKey);
        TIterator       find        (Key const & aKey);
        TConstIterator  find        (Key const & aKey) const;
        
        TIterator       insert      (TIterator aIterator, TPair< Key, T > const & aPair);
        TIterator       insert      (TIterator aIterator, TPair< Key, T > && aPair);
        TIterator       insert_or_assign (Key const & aKey, T const & aValue);
        void            clear       ();

        FVector< TPair< Key, T > > & GetVector() { return mVector; }

    private:
        FVector< TPair< Key, T > > mVector;
    };

    template <typename Key, typename T>
    FMap<Key, T>::FMap ()
    {
    }

    template <typename Key, typename T>
    FMap<Key, T>::~FMap ()
    {
    }

    template <typename Key, typename T>
    FMap<Key, T>::FMap (const FMap<Key, T> & aOther)
        : mVector(aOther.mVector)
    {
    }

    template <typename Key, typename T>
    FMap<Key, T>::FMap (FMap<Key, T> && aOther)
        : mVector(MoveCast(aOther.mVector))
    {
    }

    template <typename Key, typename T>
    FMap<Key, T>& FMap<Key, T>::operator= (const FMap<Key, T> & aOther)
    {
        mVector = aOther.mVector;
    }

    template <typename Key, typename T>
    FMap<Key, T>& FMap<Key, T>::operator= (FMap<Key, T> && aOther)
    {
        mVector = MoveCast(aOther.mVector);
    }

    //Iterator
    template <typename Key, typename T>
    typename FMap<Key, T>::TIterator FMap<Key, T>::begin ()
    {
        return mVector.begin();
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TConstIterator FMap<Key, T>::begin () const
    {
        return mVector.begin();
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TIterator FMap<Key, T>::end ()
    {
        return mVector.end();
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TConstIterator FMap<Key, T>::end () const
    {
        return mVector.end();
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TReference FMap<Key, T>::operator[] (Key const & aKey)
    {
        TIterator lIter = find(aKey);
        if(mVector.end() == lIter)
        {
            mVector.push_back(MakePair<Key, T>(aKey, T()));
            lIter = &mVector.back();
        }

        return lIter->second;
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TIterator FMap<Key, T>::find (Key const & aKey)
    {
        for(TPair< Key, T > & lIter : mVector)
        {
            if(lIter.first == aKey)
            {
                return &lIter;
            }
        }

        return mVector.end();
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TConstIterator FMap<Key, T>::find (Key const & aKey) const
    {
        for(TPair< Key, T > const & lIter : mVector)
        {
            if(lIter.first == aKey)
            {
                return &lIter;
            }
        }

        return mVector.end();
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TIterator FMap<Key, T>::insert (typename FMap<Key, T>::TIterator aIterator, TPair< Key, T > const & aPair)
    {
        if(aIterator == mVector.end())
        {
            mVector.push_back(aPair);
            aIterator = &mVector.back();
        }
        else if(mVector.data() <= aIterator && mVector.end() > aIterator)
        {
            *aIterator = aPair;
        }
        else
        {
            aIterator = nullptr;
            SBX_ERROR("[FMap::insert] Invalid iterator");
        }

        return aIterator;
    }

    template <typename Key, typename T>
    typename FMap<Key, T>::TIterator FMap<Key, T>::insert (typename FMap<Key, T>::TIterator aIterator, TPair< Key, T > && aPair)
    {
        if(aIterator == mVector.end())
        {
            mVector.push_back(MoveCast(aPair));
            aIterator = &mVector.back();
        }
        else if(mVector.data() <= aIterator && mVector.end() > aIterator)
        {
            aIterator = nullptr;
            *aIterator = MoveCast(aPair);
        }
        else
        {
            SBX_ERROR("[FMap::insert] Invalid iterator");
        }

        return aIterator;
    }
    template <typename Key, typename T>
    typename FMap<Key, T>::TIterator FMap<Key, T>::insert_or_assign (Key const & aKey, T const & aValue)
    {
        TIterator lIter = find(aKey);
        if(mVector.end() == lIter)
        {
            mVector.push_back(MakePair<Key, T>(aKey, aValue));
            lIter = &mVector.back();
        }
        else
        {
            lIter->second = aValue;
        }

        return lIter;
    }

    template <typename Key, typename T>
    void FMap<Key, T>::clear ()
    {
        mVector.clear();
    }

} // sbx

#endif // __SBX_LIBRARY_MAP_H__