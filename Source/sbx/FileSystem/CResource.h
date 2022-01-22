/*
 * @file    CResource.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_RES_H__
#define __SBX_RES_H__

#include <sbx/DataStructures/CRawBuffer.h>
#include <sbx/Library/FVector.h>
#include <sbx/Library/FMap.h>
#include <sbx/FileSystem/File.h>
#include <sbx/TextureCore/Color.h>
#include <sbx/TextureCore/TTransform2D.h>
#include <utility>

namespace sbx
{
    enum class EResourceType
    {
        eInt32 = 0,
        eInt64,
        eUInt32,
        eUInt64,
        eFloat,
        eDouble,
        eBoolean,
        eString,
        eArray,
        eDictionary,
        eCount,
        eInvalid = -1  
    };

    class CResource
    {
    public:
        //static
        static CResource     CreateArray        ();
        static CResource     CreateDictionary   ();

                                //CResource       ();
                                CResource       (EResourceType aType = EResourceType::eDictionary);
                                CResource       (int32_t aValue);
                                CResource       (uint32_t aValue);
                                CResource       (int64_t aValue);
                                CResource       (uint64_t aValue);
                                CResource       (float aValue);
                                CResource       (double aValue);
                                CResource       (bool aValue);
                                CResource       (TUniqueIdCS const & aValue);
                                CResource       (const char* aStr);
                                CResource       (const char* aStr, size_t aLen);
                                CResource       (TColor32F const & aValue);
                                CResource       (gfx::TTransform2D const & aValue);
                               ~CResource       ();
                                CResource       (CResource const & aOther);
        CResource &             operator=       (CResource const & aOther);  
        void                    RemoveRef       ();
        //void                    CopyFrom        (CResource const & aOther);
        EResourceType           GetType         () const;
        bool                    IsOk            () const;
        bool                    IsRefCounted    () const;
        
        bool                    IsReal          () const;
        bool                    IsInteger       () const;
        bool                    IsArray         () const;
        bool                    IsDictionary    () const;
        //Compare
        bool            operator==      (CResource const & aOther) const;

        //Getters
        uint8_t         AsUint8         (uint8_t    aDefault = 0u) const;
        int32_t         AsInt32         (int32_t    aDefault = 0) const;
        uint32_t        AsUInt32        (uint32_t   aDefault = 0u) const;
        int64_t         AsInt64         (int64_t    aDefault = 0ll) const;
        uint64_t        AsUInt64        (uint64_t   aDefault = 0ull) const;
        float           AsFloat         (float      aDefault = 0.f) const;
        double          AsDouble        (double     aDefault = 0.0) const;
        bool            AsBool          (bool       aDefault = false) const;
        TUniqueIdCS     AsUniqueIdCS    (TUniqueIdCS const & aDefault = TUniqueIdCS()) const;
        TUniqueId       AsUniqueId      (TUniqueId const & aDefault = TUniqueId()) const;
        const char*     AsConstStr      (const char* aDefault = "") const;
        TColor32F       AsColor         (TColor32F const & aDefault = TColor32F()) const;
        gfx::TTransform2D AsTransform2D (gfx::TTransform2D const & aDefault = gfx::TTransform2D()) const;

        uint32_t        Count           () const;
        CResource       ArrayAt         (uint32_t aIndex) const;
        TUniqueIdCS     DictKeyAt       (uint32_t aIndex) const;
        CResource       DictValueAt     (uint32_t aIndex) const;

        //CResource const & operator[]    (uint32_t aIndex) const;
        CResource const & operator[]    (const char* aKey) const;
        CResource const & operator[]    (TUniqueIdCS const & aKey) const;

        //CResource &     operator[]      (uint32_t aIndex);
        //CResource &     operator[]      (const char* aKey);
        //CResource &     operator[]      (TUniqueIdCS const & aKey);

        //Modifiers     
        void        PushBack    (CResource const & aRes);
        void        AddPair     (const char* aKey, CResource const & aRes);
        void        AddPair     (TUniqueIdCS const & aKey, CResource const & aRes);

        //Cast
        template <typename T> T Cast(T aDefualt) const;
    private:

        struct TResourceData;
        EResourceType   mType;
        union
        {
            TResourceData*  mData;
            int64_t         mInt64Value;
            uint64_t        mUInt64Value;
            int32_t         mInt32Value;
            uint32_t        mUInt32Value;
            double          mDoubleValue;
            float           mFloatValue;
            bool            mBoolValue;
        };
    };

    template <typename T> 
    T CResource::Cast(T aDefualt) const
    {
        switch ( mType )
        {
        case EResourceType::eUInt32:     return T(mUInt32Value); break;
        case EResourceType::eInt32:      return T(mInt32Value); break;
        case EResourceType::eFloat:      return T(mFloatValue); break;
        case EResourceType::eUInt64:     return T(mUInt64Value); break;
        case EResourceType::eInt64:      return T(mInt64Value); break;
        case EResourceType::eDouble:     return T(mDoubleValue); break;
        case EResourceType::eBoolean:    return T(mBoolValue); break;
        }

        return aDefualt;
    }
}

#endif //__SBX_RES_H__