/*
 * @file    TUniqueId.inl
 * @author  David Gallardo Moreno
 */

//#include <Common.h>
#include <sbx/Utils/CRC32.h>
namespace sbx
{
    //----------------------------------------------------------------
    // TUniqueIdBase
    //----------------------------------------------------------------
    template <bool CS>
    TUniqueIdBase<CS>::TUniqueIdBase(const char * aStr)
        : mData(nullptr)
    {
        uint32_t lLen = 0;
        const uint32_t lCRC32 = sbx::CRC32::str2crc32(aStr, lLen, CS);
        mData = GetRegistry()->RegisterUniqueId(aStr, lLen, lCRC32);
        GetRegistry()->AddReference(mData);
    }

    template <bool CS>
    TUniqueIdBase<CS>::TUniqueIdBase(TUniqueIdBase<CS> const & aOther)
        : mData(nullptr)
    {
        mData = aOther.mData;
        GetRegistry()->AddReference(mData);
    }

    template <bool CS>
    TUniqueIdBase<CS>::~TUniqueIdBase()
    {
        if(GetRegistry()->RemoveReference(mData))
        {
            SBX_DELETE(sUniqueIdRegistry);
            sUniqueIdRegistry = nullptr;
        }
    }

    template <bool CS>
    TUniqueIdBase<CS> const & TUniqueIdBase<CS>::operator=(TUniqueIdBase<CS> const & aOther)
    {
        if(nullptr != mData)
        {
            GetRegistry()->RemoveReference(mData);
        }
        mData = aOther.mData;
        GetRegistry()->AddReference(mData);
        return *this;
    }
 
    template <bool CS>
    bool TUniqueIdBase<CS>::operator==(TUniqueIdBase<CS> const & aOther) const
    {
        return mData == aOther.mData;
    }

    template <bool CS>
    bool TUniqueIdBase<CS>::operator!=(TUniqueIdBase<CS> const & aOther) const
    {
        return mData != aOther.mData;
    }

    template <bool CS>
    bool TUniqueIdBase<CS>::operator<(TUniqueIdBase const & aOther) const
    {
        return mData < aOther.mData;
    }

    template <bool CS>
    uint32_t TUniqueIdBase<CS>::GetCRC32() const
    {
        if(mData)
        {
            return mData->mCRC32;
        }

        return 0;
    }

    template <bool CS>
    const char* TUniqueIdBase<CS>::GetStr() const
    {
        if(mData)
        {
            return mData->mString;
        }

        return 0;
    }

    template <bool CS>
    size_t TUniqueIdBase<CS>::GetLength() const
    {
        if(mData)
        {
            return mData->mLength;
        }

        return 0;
    }

    template <bool CS>
    bool TUniqueIdBase<CS>::IsEmpty () const
    {
        return (!mData || (mData->mLength == 0));
    }

    template <bool CS>
    CUniqueIdRegistry* TUniqueIdBase<CS>::GetRegistry()
    {
        if(sUniqueIdRegistry == nullptr)
        {
            sUniqueIdRegistry = SBX_NEW(CUniqueIdRegistry);
        }
        return sUniqueIdRegistry;
    }
} //namespace sbx
