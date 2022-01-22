/*
 * @file CChannel.inl
 * @author David Gallardo Moreno
 */

#ifndef SBX_CCHANNEL_IMPL
#define SBX_CCHANNEL_IMPL

namespace sbx
{
    namespace _impl
    {
        //-----------------------------------------------------------------------
        // TBroadcaster
        //-----------------------------------------------------------------------
        template <typename T> struct TBroadcaster
        {
            enum class EBindingType
            {
                eFunction,
                eMemberFunction,
                eCount,
                eInvalid = 1,
            };

            struct TFunctionBindingBase
            {
                            TFunctionBindingBase(){;}
                virtual    ~TFunctionBindingBase(){;}
                virtual void operator()(T const * aMsg) = 0;
                virtual EBindingType GetType() const = 0;
            };

            struct TFunctionBinding : public TFunctionBindingBase
            {
                typedef void(*TListener)(T const *);

                TListener mCallback;

                TFunctionBinding(TListener const & aCallback)
                    : mCallback(aCallback)
                {;}

                void operator()(T const * aMsg) override
                {
                    mCallback(aMsg);
                }

                EBindingType GetType() const override
                {
                    return EBindingType::eFunction;
                }
            };

            template <typename U> struct TMemberBinding : public TFunctionBindingBase
            {
                typedef void(U::*TMemberListener)(T const *);

                TMemberListener mCallback;
                U* mObject;

                TMemberBinding(TMemberListener const & aCallback, U* aObject)
                    : mCallback(aCallback)
                    , mObject(aObject)
                {;}

                void operator()(T const * aMsg) override
                {
                    (mObject->*mCallback)(aMsg);
                }

                EBindingType GetType() const override
                {
                    return EBindingType::eMemberFunction;
                }
            };

        public:

            ~TBroadcaster()
            {
                for(auto lIt = mCallbacks.begin(); lIt != mCallbacks.end(); lIt++)
                {
                    SBX_DELETE_SILENT(*lIt);
                }

                mCallbacks.clear();
            }

            bool HaveCallback( void(*aCallback)(T const *) ) const
            {
                for(TFunctionBindingBase const * lCallbackIt : mCallbacks)
                {
                    if (EBindingType::eFunction == lCallbackIt->GetType())
                    {
                        TFunctionBinding const * lBinding = reinterpret_cast<TFunctionBinding const*>(lCallbackIt);
                        if(lBinding->mCallback == aCallback)
                        {
                            return true;
                        }
                    }
                }

                return false;
            }

            template <typename U> bool HaveCallback(U* aObject, void(U::*aCallback)(T const *)) const
            {
                for(TFunctionBindingBase const * lCallbackIt : mCallbacks)
                {
                    if (EBindingType::eMemberFunction == lCallbackIt->GetType())
                    {
                        TMemberBinding<U> const * lBinding = reinterpret_cast<TMemberBinding<U> const*>(lCallbackIt);
                        if( (lBinding->mCallback == aCallback) &&
                            (lBinding->mObject == aObject))
                        {
                            return true;
                        }
                    }
                }

                return false;
            }

            void RegisterCallback( void(*aCallback)(T const *) )
            {
                SBX_ASSERT(HaveCallback(aCallback) == false, "[TBroadcaster::RegisterCallback] Registering already existing callback!");
                mCallbacks.push_back(SBX_NEW_SILENT(TFunctionBinding, aCallback));
            }

            template <typename U> void RegisterCallback(U* aObject, void(U::*aCallback)(T const *))
            {
                SBX_ASSERT(HaveCallback(aObject, aCallback) == false, "[TBroadcaster::RegisterCallback] Registering already existing callback!");
                mCallbacks.push_back(SBX_NEW_SILENT(TMemberBinding<U>, aCallback, aObject));
            }

            void UnregisterCallback( void(*aCallback)(T const *) )
            {
                for(auto lIt = mCallbacks.begin(); lIt != mCallbacks.end(); lIt++)
                {
                    TFunctionBinding* lBinding = reinterpret_cast< TFunctionBinding* >(*lIt);
                    if( (EBindingType::eFunction == (*lIt)->GetType()) && 
                        (lBinding->mCallback == aCallback))
                    {
                        /*if(mCallbacks.back() != *lIt)
                        {
                            std::swap(mCallbacks.back(), *lIt);
                        }
                        mCallbacks.pop_back();*/
                        //mCallbacks.erase(lIt);
                        
                        SBX_DELETE_SILENT(lBinding);
                        mCallbacks.erase(lIt);
                        return;
                    }
                }

                SBX_ERROR("[TBroadcaster::UnregisterCallback] Trying to unregister callback not registered!");
            }

            template <typename U> void UnregisterCallback(U* aObject, void(U::*aCallback)(T const *))
            {
                for(auto lIt = mCallbacks.begin(); lIt != mCallbacks.end(); lIt++)
                {
                    TMemberBinding<U>* lBinding = reinterpret_cast< TMemberBinding<U>* >(*lIt);
                    if( (EBindingType::eMemberFunction == (*lIt)->GetType()) &&
                        (lBinding->mCallback == aCallback) &&
                        (lBinding->mObject == aObject))
                    {
                        /*if(mCallbacks.back() != *lIt)
                        {
                            std::swap(mCallbacks.back(), *lIt);
                        }
                        mCallbacks.pop_back();*/
                        SBX_DELETE_SILENT(lBinding);
                        mCallbacks.erase(lIt);
                        return;
                    }
                }

                SBX_ERROR("[TBroadcaster::UnregisterCallback] Trying to unregister callback not registered!");
            }

            void BroadcastMessage(const T* aMsg) const
            {
                for(TFunctionBindingBase* lBind : mCallbacks)
                {
                    (*lBind)(aMsg);
                }
            }

        private:
            FList< TFunctionBindingBase* > mCallbacks;
        };
    } // _impl


    template <typename T> CChannel::TBroadcasterIt CChannel::GetOrCreateBroadcaster()
    {
        const uint32_t lTypeId = GetTypeId< T > ();
        auto lIterator = mBroadcasters.find(lTypeId); 
        if(lIterator == mBroadcasters.end())
        {
            lIterator = mBroadcasters.insert
            (
                lIterator, 
                MakePair
                (
                    lTypeId, 
                    _impl::TStoredBroadcaster(SBX_NEW(_impl::TBroadcaster<T>), &_impl::BroadcasterDeleter< _impl::TBroadcaster<T> >)
                )
            );
        }

        return lIterator;
    }

    template <typename T> void CChannel::RegisterCallback( void(*aCallback)(T const *) )
    {
        auto lIterator = GetOrCreateBroadcaster<T>();
        reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->RegisterCallback(aCallback);
    }

    template <typename T, typename U> void CChannel::RegisterCallback(U* aObject, void(U::*aCallback)(T const *) )
    {
        auto lIterator = GetOrCreateBroadcaster<T>();
        reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->RegisterCallback(aObject, aCallback);
    }

    template <typename T> void CChannel::UnregisterCallback(  void(*aCallback)(T const *)  )
    {
        const uint32_t lTypeId = GetTypeId< T > ();
        auto lIterator = mBroadcasters.find(lTypeId);
        if(lIterator != mBroadcasters.end())
        {
            reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->UnregisterCallback(aCallback);   
        }
    }

    template <typename T, typename U> void CChannel::UnregisterCallback(U* aObject, void(U::*aCallback)(T const *) )
    {
        const uint32_t lTypeId = GetTypeId< T > ();
        auto lIterator = mBroadcasters.find(lTypeId);
        if(lIterator != mBroadcasters.end())
        {
            reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->UnregisterCallback(aObject, aCallback);   
        }
    }

    template <typename T> bool CChannel::HaveCallback( void(*aCallback)(T const *) ) const
    {
        const uint32_t lTypeId = GetTypeId< T > ();
        auto lIterator = mBroadcasters.find(lTypeId);
        if(lIterator != mBroadcasters.end())
        {
            return reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->HaveCallback(aObject, aCallback);   
        }

        return false;
    }

    template <typename T, typename U> bool CChannel::HaveCallback(U* aObject, void(U::*aCallback)(T const *) ) const
    {
        const uint32_t lTypeId = GetTypeId< T > ();
        auto lIterator = mBroadcasters.find(lTypeId);
        if(lIterator != mBroadcasters.end())
        {
            return reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->HaveCallback(aObject, aCallback);   
        }

        return false;
    }

    template <typename T> void CChannel::BroadcastMessage(T const & aMsg) const
    {
        uint32_t const lTypeId = GetTypeId< T > ();
        auto lIterator = mBroadcasters.find(lTypeId);
        if(lIterator != mBroadcasters.end())
        {
            reinterpret_cast< _impl::TBroadcaster<T>* >(lIterator->second.mBroadcaster)->BroadcastMessage(&aMsg);   
        }
    }

    template <typename A> const char* GetTypeName(_impl::TBroadcaster<A>* aObj)
    {
        return "struct sbx::_impl::TBroadcaster<Msg>";
    }

    template <typename A, typename B> const char* GetTypeName(typename _impl::TBroadcaster<A>::TMemberBinding<B>* aObj)
    {
        return "struct sbx::_impl::TBroadcaster<Msg>::TMemberBinding<Listener>";
    }
} // sbx

#endif //SBX_CCHANNEL_IMPL