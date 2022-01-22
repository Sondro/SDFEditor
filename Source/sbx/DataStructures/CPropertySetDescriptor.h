/*
 * @file    CPropertySetDescriptor.h
 * @author  David Gallardo Moreno
 */

#ifndef SBX_PROPERTY_SET_TEMPLATE_H
#define SBX_PROPERTY_SET_TEMPLATE_H

#include <sbx/Utils/LangUtils.h>
#include <sbx/TextureCore/Color.h>
#include <sbx/Library/FVector.h>
#include <sbx/TextureCore/TTransform2D.h>

namespace sbx
{
    enum class EPropertyType : int32_t
    {
#       define PROPERTY_TYPE(_name, _type, _size) ePT_##_name,
#       include <sbx/DataStructures/PropertyTypesRegister.def>
#       undef PROPERTY_TYPE
        ePT_Count,
        ePT_Invalid = -1
    };

    struct TPropertyInfo
    {
        TUniqueIdCS     mName;
        TUniqueIdCS     mDisplayName;
        EPropertyType   mType;
        uint32_t        mDataOffset;
        uint32_t        mSize;
        uint32_t        mIndex { UINT_MAX };
        
        struct TEnumPair
        {
            TUniqueIdCS     mTag;
            int32_t         mValue;
        };

        FVector<TEnumPair> mEnumValues;

        union
        {
            uint8_t mDefaultValue;
#           define PROPERTY_TYPE(_name, _type, _size) _type mDefault##_name##Value;
#           include <sbx/DataStructures/PropertyTypesRegister.def>
#           undef PROPERTY_TYPE            
        };

        union
        {
            uint8_t mMinValue;
#           define PROPERTY_TYPE(_name, _type, _size) _type mMin##_name##Value;
#           include <sbx/DataStructures/PropertyTypesRegister.def>
#           undef PROPERTY_TYPE            
        };

        union
        {
            uint8_t mMaxValue;
#           define PROPERTY_TYPE(_name, _type, _size) _type mMax##_name##Value;
#           include <sbx/DataStructures/PropertyTypesRegister.def>
#           undef PROPERTY_TYPE            
        };

        union
        {
            uint8_t mValueStep;
#           define PROPERTY_TYPE(_name, _type, _size) _type mStep##_name##Value;
#           include <sbx/DataStructures/PropertyTypesRegister.def>
#           undef PROPERTY_TYPE            
        };

        TPropertyInfo(){}
        static EPropertyType TypeStrToEnum(const char* aStr);
    };

    class CPropertySetDescriptor
    {
    public:
                                    CPropertySetDescriptor();
                                   ~CPropertySetDescriptor();
        bool                        Init                    (TPropertyInfo* aPropertiesInfo, uint32_t aPropertiesCount);
        uint32_t                    GetPropertyInfoCount    () const;
        const TPropertyInfo*        GetPropertyInfoAt       (uint32_t aIndex) const;
        uint32_t                    GetPropertyIndex        (TUniqueIdCS const & aUniqueId) const;

    private:
        FVector< TPropertyInfo* >   mPropertiesInfoList;
    };


} // sbx

#endif // SBX_PROPERTY_SET_TEMPLATE_H
