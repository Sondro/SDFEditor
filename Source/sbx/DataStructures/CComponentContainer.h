/*
 * @file    CComponentContainer.h
 * @author  David Gallardo Moreno
 */

#ifndef SBX_COMPONENT_CONTAINER_HPP
#define SBX_COMPONENT_CONTAINER_HPP

#include <sbx/Utils/LangUtils.h>
#include <sbx/Library/FMap.h>
#include <functional>

namespace sbx
{
    namespace __component_deleter
    {
        template <typename T> void ComponentDeleter(void* aObject)
        {
            SBX_DELETE(reinterpret_cast< T* >(aObject));
        }
    }

    class CComponentContainer
    {
    private:
        struct TStoredComponent
        {
            typedef void(*TComponentDeleterFnc)(void*);

            void* mObject;
            TComponentDeleterFnc mDeleter;

            TStoredComponent(void* aObject, TComponentDeleterFnc const & aDeleter)
                : mObject(aObject)
                , mDeleter(aDeleter)
            {
            }
        };
    public:
                                    CComponentContainer ();
        virtual                    ~CComponentContainer ();
        template <typename T> T*    AddComponent        (T* aComponent);
        template <typename T> bool  RemoveComponent     ();
        template <typename T> T*    GetComponent        ();
    private:
        FMap<uint32_t, TStoredComponent> mComponents;
    };

    template <typename T> T* CComponentContainer::AddComponent(T* aComponent)
    {
        const uint32_t lTypeId = GetTypeId<T>();
        auto lIterator = mComponents.find(lTypeId);
        if(lIterator == mComponents.end())
        {
            mComponents.insert(lIterator, 
                               MakePair
                               (
                                   lTypeId, 
                                   TStoredComponent
                                   (
                                       aComponent, 
                                       &__component_deleter::ComponentDeleter<T>
                                   )
                               ));
            return aComponent;
        }
        
        SBX_ERROR("Can't add the same component type twice.");
        return nullptr;
    }

    template <typename T> T* CComponentContainer::GetComponent()
    {
        const uint32_t lTypeId = GetTypeId<T>();
        auto lIterator = mComponents.find(lTypeId);
        if(lIterator != mComponents.end())
        {
            return reinterpret_cast<T*>(lIterator->second.mObject);
        }

        SBX_ERROR("Trying to get an unexistent component.");
        return nullptr;
    }

    template <typename T> bool CComponentContainer::RemoveComponent()
    {
        const uint32_t lTypeId = GetTypeId<T>();
        auto lIterator = mComponents.find(lTypeId);
        if(lIterator != mComponents.end())
        {
            SBX_DELETE(reinterpret_cast<T*>(lIterator->second.mObject));
            lIterator->second.mObject = 0;
            mComponents.erase(lIterator);
            return true;
        }
        else
        {
            SBX_ERROR("Trying to remove an unexistent component.");
            return false;
        }
    }
} //sbx

#endif //SBX_COMPONENT_CONTAINER_HPP
