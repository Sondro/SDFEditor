/*
 * @file    CPropertySet.cpp
 * @author  David Gallardo Moreno
 */


#include <sbx/Common.h>
#include "CPropertySetDescriptor.h"

namespace
{
    static const uint32_t sPropertyTypesSizes[] =
    {
#       define PROPERTY_TYPE(_name, _type, _size) _size,
#       include <sbx/DataStructures/PropertyTypesRegister.def>
#       undef PROPERTY_TYPE
    };

    static const sbx::TUniqueId sPropertyTypesNames[] =
    {
#       define PROPERTY_TYPE(_name, _type, _size) sbx::TUniqueId( #_name ),
#       include <sbx/DataStructures/PropertyTypesRegister.def>
#       undef PROPERTY_TYPE
    };
}

namespace sbx
{
    EPropertyType TPropertyInfo::TypeStrToEnum(const char* aStr)
    {
        const TUniqueId lTypeUID(aStr);
        const sbx::TUniqueId* lPropTypeNameIt = sPropertyTypesNames;
        for(int32_t i = 0, l = int32_t(EPropertyType::ePT_Count); i < l; ++i)
        {
            if(sPropertyTypesNames[i] == lTypeUID)
            {
                return EPropertyType(i);
            }
        }

        return EPropertyType::ePT_Invalid;
    }

    CPropertySetDescriptor::CPropertySetDescriptor()
    {

    }

    CPropertySetDescriptor::~CPropertySetDescriptor()
    {

    }

    bool CPropertySetDescriptor::Init (TPropertyInfo* aPropertiesInfo, uint32_t aPropertiesCount)
    {
        //the magic, crate a buffer with the total properties size, and store 
        //each property info with it's mData member poiting to an offset of this buffer.

        mPropertiesInfoList.clear();


        uint32_t lTotalSize = 0;
        for(uint32_t i = 0; i < aPropertiesCount; ++i)
        {
            TPropertyInfo* lProperty = SBX_NEW(TPropertyInfo, aPropertiesInfo[i]);
            lProperty->mDataOffset = lTotalSize;
            lProperty->mSize = sPropertyTypesSizes[int32_t(lProperty->mType)];
            lProperty->mIndex = aPropertiesInfo[i].mIndex;
            lTotalSize += sPropertyTypesSizes[int32_t(lProperty->mType)];
            mPropertiesInfoList.push_back(lProperty);
        }

        return true;
    }

    uint32_t CPropertySetDescriptor::GetPropertyInfoCount () const
    {
        return static_cast< uint32_t >(mPropertiesInfoList.size());
    }

    const TPropertyInfo* CPropertySetDescriptor::GetPropertyInfoAt (uint32_t aIndex) const
    {
        //TODO: checks / asserts
        return mPropertiesInfoList[aIndex];
    }

    uint32_t CPropertySetDescriptor::GetPropertyIndex (TUniqueIdCS const & aUniqueId) const
    {
        for(uint32_t i = 0, l = (uint32_t)mPropertiesInfoList.size(); i < l; ++i)
        {
            if(aUniqueId == mPropertiesInfoList[i]->mName)
            {
                return i;
            }
        }

        return UINT32_MAX;
    }

} // sbx