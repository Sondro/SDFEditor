/*
 * @file CChannel.h
 * @author David Gallardo Moreno
 */

#ifndef SBX_CCHANNEL_HPP
#define SBX_CCHANNEL_HPP

#include <sbx/Utils/LangUtils.h>
#include <sbx/Library/FMap.h>
#include <sbx/Library/FList.h>

namespace sbx
{
    //template <typename T> using TListener = std::function< void(T const *) >;
    //#define SBX_LISTENER_BIND(_obj, _mf) std::bind(_mf, _obj, std::placeholders::_1)
 
    namespace _impl
    {
        //-----------------------------------------------------------------------
        // BroadcasterDeleter
        //-----------------------------------------------------------------------
        template <typename T> void BroadcasterDeleter(void* aObject)
        {
            SBX_DELETE(reinterpret_cast< T* >(aObject));
        }

        //-----------------------------------------------------------------------
        // TStoredBroadcaster
        //-----------------------------------------------------------------------
        struct TStoredBroadcaster
        {
            typedef void(*TComponentDeleterFnc)(void*);
            TComponentDeleterFnc mDeleter;
            void*                mBroadcaster;

            TStoredBroadcaster(void* aBroadcaster, TComponentDeleterFnc const & aDeleter)
                : mBroadcaster(aBroadcaster)
                , mDeleter(aDeleter)
            {}
        };
    } // _impl

    //-----------------------------------------------------------------------
    // CChannel
    //-----------------------------------------------------------------------

    class CChannel
    {
    public:
                                                CChannel                ();
                                               ~CChannel                ();
        void                                    UnregisterAllCallbacks  ();

        template <typename T> void              RegisterCallback        ( void(*aCallback)(T const *) );
        template <typename T, typename U> void  RegisterCallback        (U* aObject, void(U::*aCallback)(T const *) );
        
        template <typename T> void              UnregisterCallback      ( void(*aCallback)(T const *) );
        template <typename T, typename U> void  UnregisterCallback      (U* aObject, void(U::*aCallback)(T const *) );

        template <typename T> bool              HaveCallback            ( void(*aCallback)(T const *) ) const;
        template <typename T, typename U> bool  HaveCallback            (U* aObject, void(U::*aCallback)(T const *) ) const;

        template <typename T> void              BroadcastMessage        (T const & aMsg) const;

    private:
        typedef FMap< uint32_t, _impl::TStoredBroadcaster >::TIterator TBroadcasterIt;
        template <typename T> TBroadcasterIt    GetOrCreateBroadcaster  ();

    private:
        FMap< uint32_t, _impl::TStoredBroadcaster > mBroadcasters;
    };
} // sbx

#include <sbx/Messaging/CChannel.inl>

#endif //SBX_CHANNEL_HPP
