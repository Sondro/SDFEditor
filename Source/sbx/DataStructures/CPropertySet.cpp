/*
 * @file    CPropertySet.cpp
 * @author  David Gallardo Moreno
 */


#include <sbx/Common.h>
#include "CPropertySet.h"
#include "CPropertySetDescriptor.h"

namespace
{
    const static size_t kRoundUpBufferSize = 32; //bytes

    size_t RoundUpBufferSize(size_t aSize)
    {
        return (size_t(aSize / kRoundUpBufferSize) + 1) * kRoundUpBufferSize;
    }
}

namespace sbx
{
    CPropertySet::CPropertySet()
        : mPropertiesData(nullptr)
        , mBufferSize(0)
    {

    }

    CPropertySet::~CPropertySet()
    {
        if(mPropertiesData)
        {
            SBX_FREE(mPropertiesData);
            mPropertiesData = 0;
        }
    }
    
    //the magic, crate a buffer with the total properties size, and store 
    //each property info with it's mData member poiting to an offset of this buffer.

    bool CPropertySet::Init (CPropertySetDescriptor const *aDescriptor)
    {
        mDescriptor = aDescriptor;

        size_t lBufferSize = 0;
        for(uint32_t i = 0; i < GetPropertyInfoCount(); ++i)
        {
            const TPropertyInfo* lProperty = GetPropertyInfoAt(i);
            lBufferSize += lProperty->mSize;
        }

        lBufferSize = RoundUpBufferSize(lBufferSize);

        if(!mPropertiesData || (lBufferSize != mBufferSize))
        {
            mBufferSize = lBufferSize;
            if(mPropertiesData)
            {
                SBX_FREE(mPropertiesData);
            }
            mPropertiesData = (uint8_t*)SBX_MALLOC(mBufferSize);
        }
        
        ::memset(mPropertiesData, 0, mBufferSize);

        for(uint32_t i = 0; i < GetPropertyInfoCount(); ++i)
        {
            const TPropertyInfo* lProperty = GetPropertyInfoAt(i);
            ::memcpy( &mPropertiesData[lProperty->mDataOffset], 
                      &lProperty->mDefaultValue, 
                      lProperty->mSize);
        }
        return true;
    }

    bool CPropertySet::Init(CPropertySet const & aOther)
    {
        Init(aOther.mDescriptor);
        ::memcpy(mPropertiesData, aOther.mPropertiesData, mBufferSize);
        return true;
    }

    uint32_t CPropertySet::GetPropertyInfoCount () const
    {
        return mDescriptor->GetPropertyInfoCount();
    }

    const TPropertyInfo* CPropertySet::GetPropertyInfoAt (uint32_t aIndex) const
    {
        //TODO: checks / asserts
        return mDescriptor->GetPropertyInfoAt(aIndex);
    }

    uint8_t* CPropertySet::GetPropertyRawDataPtrAt (uint32_t aIndex)
    {
        return &mPropertiesData[mDescriptor->GetPropertyInfoAt(aIndex)->mDataOffset];
    }

    const uint8_t* CPropertySet::GetPropertyRawDataPtrAt (uint32_t aIndex) const
    {
        return &mPropertiesData[mDescriptor->GetPropertyInfoAt(aIndex)->mDataOffset];
    }

    uint32_t CPropertySet::GetPropertyIndex (TUniqueIdCS const & aUniqueId) const
    {
        return mDescriptor->GetPropertyIndex(aUniqueId);    
    }

} // sbx