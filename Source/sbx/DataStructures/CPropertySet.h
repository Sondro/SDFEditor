/*
 * @file    CPropertySet.h
 * @author  David Gallardo Moreno
 */

#ifndef SBX_PROPERTY_SET_H
#define SBX_PROPERTY_SET_H

#include <sbx/DataStructures/CPropertySetDescriptor.h>
#include <sbx/Library/FRefCounted.h>

namespace sbx
{
    struct TPropertyInfo;

    class CPropertySet : public FRefCounted
    {
        SBX_REFCOUNTED_MAKER(CPropertySet)
        SBX_DISALLOW_COPY(CPropertySet)
    public:
        

                                    CPropertySet            ();
                                   ~CPropertySet            ();
        bool                        Init                    (CPropertySetDescriptor const *aDescriptor);
        bool                        Init                    (CPropertySet const & aOther);
        uint32_t                    GetPropertyInfoCount    () const;
        const TPropertyInfo*        GetPropertyInfoAt       (uint32_t aIndex) const;
        uint8_t*                    GetPropertyRawDataPtrAt (uint32_t aIndex);
        const uint8_t*              GetPropertyRawDataPtrAt (uint32_t aIndex) const;
        uint32_t                    GetPropertyIndex        (TUniqueIdCS const & aUniqueId) const;

        template <typename T> T*    GetPropertyDataPtrAt    (uint32_t aIndex);
        template <typename T> T     GetPropertyValue        (TUniqueIdCS const & aUniqueId) const;
        template <typename T> T     GetPropertyValue        (uint32_t aIndex) const;
        template <typename T> void  SetPropertyValue        (TUniqueIdCS const & aUniqueId, T aValue);
        template <typename T> void  SetPropertyValue        (uint32_t aIndex, T aValue);
    private:
        const CPropertySetDescriptor*   mDescriptor;
        uint8_t*                        mPropertiesData;
        size_t                          mBufferSize;
    };

    template <typename T> T* CPropertySet::GetPropertyDataPtrAt(uint32_t aIndex)
    {
        return reinterpret_cast< T* >(GetPropertyRawDataPtrAt(aIndex));
    }

    template <typename T> T CPropertySet::GetPropertyValue(TUniqueIdCS const & aUniqueId) const
    {
        return GetPropertyValue< T >(GetPropertyIndex(aUniqueId));
    }

    template <typename T> T CPropertySet::GetPropertyValue(uint32_t aIndex) const
    {
        return *reinterpret_cast< const T* >(this->GetPropertyRawDataPtrAt(aIndex));
    }

    template <typename T> void CPropertySet::SetPropertyValue(TUniqueIdCS const & aUniqueId, T aValue)
    {
        *reinterpret_cast< T* >(this->GetPropertyRawDataPtrAt(GetPropertyIndex(aUniqueId))) = aValue;
    }

    template <typename T> void CPropertySet::SetPropertyValue(uint32_t aIndex, T aValue)
    {
        *reinterpret_cast< T* >(this->GetPropertyRawDataPtrAt(aIndex)) = aValue;
    }

    using CPropertySetRef = CPropertySet::Ref;
} // sbx




#endif