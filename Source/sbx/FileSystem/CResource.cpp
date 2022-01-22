/*
 * @file    CRes.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "CResource.h"
#include <sbx/Library/FObjectPool.h>

namespace sbx
{
    sbx::CResource sInvalidResource(EResourceType::eInvalid);

    typedef FMap< TUniqueIdCS, CResource > TResMap;
    typedef FVector< CResource > TResArray;

    struct CResource::TResourceData
    {
    private:
        int32_t        mRefCount;
        EResourceType  mType;
    public:

        union
        {
            TResMap     mMapValue;
            TResArray   mVectorValue;
            std::string mStringValue;

        };

        static FObjectPool< CResource::TResourceData > & GetObjectPool()
        {
            static FObjectPool< CResource::TResourceData > sResourceDataPool;
            if(!sResourceDataPool.IsOk())
            {
                sResourceDataPool.Init(1024);
            }
            return sResourceDataPool;
        }

        static CResource::TResourceData* Alloc(EResourceType aType)
        {
            return GetObjectPool().AllocOjbect(aType);
        }

        static void Free(CResource::TResourceData* aData)
        {
            GetObjectPool().FreeObject(aData);
        }

        TResourceData  (EResourceType aType)
            : mType(aType)
            , mRefCount(1)
        {
            
            switch ( mType )
            {
            case EResourceType::eString:      SBX_PLACEMENT_NEW(std::string, &mStringValue); break;
            case EResourceType::eArray:       SBX_PLACEMENT_NEW(TResArray, &mVectorValue); break;
            case EResourceType::eDictionary:  SBX_PLACEMENT_NEW(TResMap, &mMapValue); break;
            }
        }

        ~TResourceData()
        {
            switch ( mType )
            {
            case EResourceType::eString:      mStringValue.~basic_string(); break;
            case EResourceType::eArray:       mVectorValue.~FVector(); break;
            case EResourceType::eDictionary:  mMapValue.~FMap(); break;
            }
        }

        void AddRef()
        {
            ++mRefCount;
        }

        bool RemoveRef()
        {
            --mRefCount;
            SBX_ASSERT(mRefCount >= 0, "Invalid resource ref count!");
            if(mRefCount <= 0)
            {
                return true;
            }

            return false;
        }

        void CopyFrom (TResourceData const * aOther)
        {
            switch ( mType )
            {
            case EResourceType::eString:     mStringValue.~basic_string(); break;
            case EResourceType::eArray:      mVectorValue.~FVector(); break;
            case EResourceType::eDictionary: mMapValue.~FMap(); break;
            }

            mType = aOther->mType;
            switch ( mType )
            {
            case EResourceType::eString:     SBX_PLACEMENT_NEW(std::string, &mStringValue, aOther->mStringValue); break;
            case EResourceType::eArray:      SBX_PLACEMENT_NEW(FVector, &mVectorValue, aOther->mVectorValue); break;
            case EResourceType::eDictionary: SBX_PLACEMENT_NEW(FMap, &mMapValue, aOther->mMapValue); break;
            }
        }
    };

    

    //static
    CResource CResource::CreateArray ()
    {
        return CResource(EResourceType::eArray);
    }

    CResource CResource::CreateDictionary ()
    {
        return CResource(EResourceType::eDictionary);
    }

    /*CResource::CResource()
        : mType(EResourceType::eInvalid)
        , mUInt64Value(0)
    {
    };*/

    CResource::CResource (EResourceType aType)
        : mType(aType)
        , mUInt64Value(0)
    {
        if(EResourceType::eInvalid != aType)
        {
            switch ( mType )
            {
            case EResourceType::eString:    
            case EResourceType::eArray:     
            case EResourceType::eDictionary:
                mData = TResourceData::Alloc(aType);
                break;
            default:
                mUInt64Value = 0;
            }
        }
    }

    CResource::CResource (int32_t aValue)
        : mType(EResourceType::eInt32)
        , mUInt64Value(0)
    {
        mInt32Value = aValue;
    }

    CResource::CResource (uint32_t aValue)
        : mType(EResourceType::eUInt32)
        , mUInt64Value(0)
    {
        mUInt32Value = aValue;
    }

    CResource::CResource (int64_t aValue)
        : mType(EResourceType::eInt64)
        , mUInt64Value(0)
    {
        mInt64Value = aValue;
    }

    CResource::CResource (uint64_t aValue)
        : mType(EResourceType::eUInt64)
        , mUInt64Value(0)
    {
        mUInt64Value = aValue;
    }           

    CResource::CResource (float aValue)
        : mType(EResourceType::eFloat)
        , mUInt64Value(0)
    {
        mFloatValue = aValue;
    }

    CResource::CResource (double aValue)
        : mType(EResourceType::eDouble)
        , mUInt64Value(0)
    {
        mDoubleValue = aValue;
    }

    CResource::CResource (bool aValue)
        : mType(EResourceType::eBoolean)
        , mUInt64Value(0)
    {
        mBoolValue = aValue;
    }


    CResource::CResource (TUniqueIdCS const & aValue)
        : mType(EResourceType::eString)
        , mUInt64Value(0)
    {
        mData = TResourceData::Alloc(EResourceType::eString);
        SBX_PLACEMENT_NEW(std::string, &mData->mStringValue, aValue.GetStr(), aValue.GetLength());
    }
    
    CResource::CResource (const char* aValue)
        : mType(EResourceType::eString)
        , mUInt64Value(0)
    {
        mData = TResourceData::Alloc(EResourceType::eString);
        SBX_PLACEMENT_NEW(std::string, &mData->mStringValue, aValue);
    }

    CResource::CResource (const char* aValue, size_t aLen)
        : mType(EResourceType::eString)
        , mUInt64Value(0)
    {
        mData = TResourceData::Alloc(EResourceType::eString);
        SBX_PLACEMENT_NEW(std::string, &mData->mStringValue, aValue, aLen);
    }

    CResource::CResource (TColor32F const & aValue)
        : mType(EResourceType::eArray)
        , mUInt64Value(0)
    {
        mData = TResourceData::Alloc(EResourceType::eArray);
        mData->mVectorValue.push_back(uint8_t(aValue.r));
        mData->mVectorValue.push_back(uint8_t(aValue.g));
        mData->mVectorValue.push_back(uint8_t(aValue.b));
    }

    CResource::CResource (gfx::TTransform2D const & aValue)
        : mType(EResourceType::eArray)
        , mUInt64Value(0)
    {
        mData = TResourceData::Alloc(EResourceType::eArray);
        mData->mVectorValue.push_back(aValue.x);
        mData->mVectorValue.push_back(aValue.y);
        mData->mVectorValue.push_back(aValue.sx);
        mData->mVectorValue.push_back(aValue.sy);
        mData->mVectorValue.push_back(aValue.Rotation);
    }
    
    CResource::CResource (CResource const & aOther)
        : mType(EResourceType::eInvalid)
        , mUInt64Value(0)
    {
        if(aOther.IsRefCounted())
        {
            if(nullptr != aOther.mData)
            {
                mType = aOther.mType;
                mData = aOther.mData;
                mData->AddRef();
            }
        }
        else
        {
            mType = aOther.mType;
            mUInt64Value = aOther.mUInt64Value;
        }
    }

    CResource::~CResource ()
    {
        RemoveRef();
    }

    CResource & CResource::operator= (CResource const & aOther)
    {
        RemoveRef();

        if(aOther.IsRefCounted())
        {
            if(nullptr != aOther.mData)
            {
                mType = aOther.mType;
                mData = aOther.mData;
                mData->AddRef();
            }
        }
        else
        {
            mType = aOther.mType;
            mUInt64Value = aOther.mUInt64Value;
        }

        return *this;
    }

    
    /*void CResource::CopyFrom (CResource const & aOther)
    {
        if(nullptr == mData)
        {
            mData = TResourceData::Alloc(EResourceType::eInvalid);
            mUInt64Value = 0;
        }

        mData->CopyFrom(aOther.mData);
    }*/

    void CResource::RemoveRef()
    {
        if(IsRefCounted() && (nullptr != mData) && mData->RemoveRef())
        {
            TResourceData::Free(mData);
        }

        mData = nullptr;
    }

    //Comparator
    bool CResource::operator== (CResource const & aOther) const
    {
        return (mData == aOther.mData);
    }

    //Getters
    EResourceType CResource::GetType () const 
    { 
        //return (nullptr != mData) ? mData->mType : EResourceType::eInvalid; 
        return mType;
    }

    bool CResource::IsOk () const
    {
        return (mType != EResourceType::eInvalid) && (!IsRefCounted() || (nullptr != mData));
    }

    bool CResource::IsRefCounted () const
    {
        switch ( mType )
        {
        case EResourceType::eString:    
        case EResourceType::eArray:     
        case EResourceType::eDictionary:
            return true;
            break;
        default:
            return false;
        }

        return false;
    }

    bool CResource::IsReal () const
    {
        return  (mType == EResourceType::eFloat)  ||
                (mType == EResourceType::eDouble);
    }

    bool CResource::IsInteger () const
    {
        return  (mType == EResourceType::eInt32)  ||
                (mType == EResourceType::eUInt32) ||
                (mType == EResourceType::eInt64)  ||
                (mType == EResourceType::eUInt64);
    }

    bool CResource::IsArray () const
    {
        return IsOk() && (mType == EResourceType::eArray);
    }

    bool CResource::IsDictionary () const
    {
        return IsOk() && (mType == EResourceType::eDictionary);
    }
    
    uint8_t CResource::AsUint8 (uint8_t aDefault) const
    {
        return Cast<uint8_t>(aDefault);
    }

    int32_t CResource::AsInt32 (int32_t aDefault) const
    {
        return (mType == EResourceType::eInt32) ? mInt32Value : Cast<int32_t>(aDefault);
    }

    uint32_t CResource::AsUInt32 (uint32_t aDefault) const
    {
        return (mType == EResourceType::eUInt32) ? mUInt32Value : Cast<uint32_t>(aDefault);
    }

    int64_t CResource::AsInt64 (int64_t aDefault) const
    {
        return (mType == EResourceType::eInt64) ? mInt64Value : Cast<int64_t>(aDefault); 
    }

    uint64_t CResource::AsUInt64 (uint64_t aDefault) const
    {
        return (mType == EResourceType::eUInt64) ? mUInt64Value : Cast<uint64_t>(aDefault);
    }

    float CResource::AsFloat (float aDefault) const
    {
        return (mType == EResourceType::eFloat) ? mFloatValue : Cast<float>(aDefault);
    }

    double CResource::AsDouble (double aDefault) const
    {
        return (mType == EResourceType::eDouble) ? mDoubleValue : Cast<double>(aDefault);
    }

    bool CResource::AsBool (bool aDefault) const
    {
        return (mType == EResourceType::eBoolean) ? mBoolValue : Cast<bool>(aDefault);
    }
    
    TUniqueId CResource::AsUniqueId (TUniqueId const & aDefault) const
    {
        if(nullptr == mData) return aDefault;
        else return (mType == EResourceType::eString) ? TUniqueId(mData->mStringValue.c_str()) : aDefault;
    }

    TUniqueIdCS CResource::AsUniqueIdCS (TUniqueIdCS const & aDefault) const
    {
        if(nullptr == mData) return aDefault;
        else return (mType == EResourceType::eString) ? TUniqueIdCS(mData->mStringValue.c_str()) : aDefault;
    }

    const char* CResource::AsConstStr (const char* aDefault) const
    {
        if(nullptr == mData) return aDefault;
        else return (mType == EResourceType::eString) ? mData->mStringValue.c_str() : aDefault;
    }

    TColor32F CResource::AsColor (TColor32F const & aDefault) const
    {
        if(nullptr == mData) return aDefault;
        else return (mType == EResourceType::eArray) ? 
            TColor32F( mData->mVectorValue[0].AsFloat(),
                       mData->mVectorValue[1].AsFloat(), 
                       mData->mVectorValue[2].AsFloat()) : aDefault;
    }

    gfx::TTransform2D CResource::AsTransform2D (gfx::TTransform2D const & aDefault) const
    {
        if(nullptr == mData) return aDefault;
        else return (mType == EResourceType::eArray) ? 
            gfx::TTransform2D(  mData->mVectorValue[0].AsFloat(), 
                                mData->mVectorValue[1].AsFloat(), 
                                mData->mVectorValue[2].AsFloat(),
                                mData->mVectorValue[3].AsFloat(),
                                mData->mVectorValue[4].AsFloat()) : aDefault;
    }

    uint32_t CResource::Count () const
    {
        if(nullptr == mData) return 0u;
        else return ((mType == EResourceType::eArray) || (mType == EResourceType::eDictionary)) ? mData->mVectorValue.size32() : 0;
    }

    CResource CResource::ArrayAt (uint32_t aIndex) const
    {
        if(nullptr == mData) return sInvalidResource;
        else return ((mType == EResourceType::eArray) && (aIndex < mData->mVectorValue.size32())) ? mData->mVectorValue[aIndex] : DictValueAt(aIndex);
    }

    TUniqueIdCS CResource::DictKeyAt (uint32_t aIndex) const
    {
        if(nullptr == mData) return TUniqueIdCS();
        else if((mType == EResourceType::eDictionary))
        {
            auto & lVec = mData->mMapValue.GetVector();
            if(aIndex < lVec.size32())
            {
                return lVec[aIndex].first;
            }
        }

        return TUniqueIdCS();
    }

    CResource CResource::DictValueAt (uint32_t aIndex) const
    {
        if(nullptr == mData) return sInvalidResource;
        else if((mType == EResourceType::eDictionary))
        {
            auto & lVec = mData->mMapValue.GetVector();
            if(aIndex < lVec.size32())
            {
                return lVec[aIndex].second;
            }
        }

        return sInvalidResource;
    }
    
    /*CResource const & CResource::operator[] (uint32_t aIndex) const
    {
        return ArrayAt(aIndex);
    }*/

    CResource const & CResource::operator[] (const char* aKey) const
    {
        return this->operator[](TUniqueIdCS(aKey));
    }

    CResource const & CResource::operator[] (TUniqueIdCS const & aKey) const
    {
        if(nullptr == mData) return sInvalidResource;
        else if((mType == EResourceType::eDictionary))
        {
            auto lPair = mData->mMapValue.find(aKey);
            if(lPair != mData->mMapValue.end())
            {
                return lPair->second;
            }
        }

        return sInvalidResource;
    }
    
    /*CResource & CResource::operator[] (uint32_t aIndex)
    {
        if(mType != EResourceType::eArray) 
        {
            *this = CResource::CreateArray();
        }
        SBX_ASSERT(aIndex < mData->mVectorValue.size32());
        return mData->mVectorValue[aIndex];
    }*/

    /*CResource & CResource::operator[] (const char* aKey)
    {
        return this->operator[](TUniqueIdCS(aKey));
    }

    CResource & CResource::operator[] (TUniqueIdCS const & aKey)
    {
        if(mType != EResourceType::eDictionary) 
        {
            *this = CResource::CreateDictionary();
        }

        return mData->mMapValue[aKey];
    }*/
    
    //Modifiers     
    void CResource::PushBack (CResource const & aRes)
    {
        if(mType != EResourceType::eArray) 
        {
            *this = CResource::CreateArray();
        }
        
        mData->mVectorValue.push_back(aRes);
    }

    void CResource::AddPair (const char* aKey, CResource const & aRes)
    {
        AddPair(TUniqueIdCS(aKey), aRes);
    }

    void CResource::AddPair (TUniqueIdCS const & aKey, CResource const & aRes)
    {
        if(mType != EResourceType::eDictionary) 
        {
            *this = CResource::CreateDictionary();
        }

        mData->mMapValue.insert_or_assign(aKey, aRes);
    }

   
} // sbx