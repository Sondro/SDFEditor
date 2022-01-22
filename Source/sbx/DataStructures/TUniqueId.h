/*
 * @file    TUniqueId.h
 * @author  David Gallardo Moreno
 */

#ifndef SBX_TUNIQUEID_H
#define SBX_TUNIQUEID_H

//#include "CRC32.h"

//#define SBX_UID(__str) (sbx::TUniqueId(sbx::CRC32::const_str2crc32(__str), __str))
//#define SBX_HASH(__str) (sbx::CRC32::const_str2crc32(__str))

                                   //exposing this constructor is not coherent
//#define UID(_UID) sbx::TUniqueId _uid_##_UID( #_UID, sbx::CRC32::const_str2crc32( #_UID ) ) 



//typedef BHashMap<U32, BString> BNameRegistry;

namespace sbx 
{
    //----------------------------------------------------------------
    // TUniqueIdData
    //----------------------------------------------------------------
    struct TUniqueIdData
    {
        char*       mString;
        size_t      mLength;
        uint32_t    mCRC32;
        size_t      mRefCount;

        TUniqueIdData(const char* aStr, uint32_t aLength, uint32_t aCRC32);
        ~TUniqueIdData();
    };

    //----------------------------------------------------------------
    // CUniqueIdRegistry
    //----------------------------------------------------------------
    class CUniqueIdRegistry
    {
    public:
                        CUniqueIdRegistry   ();
                       ~CUniqueIdRegistry   ();
        TUniqueIdData*  RegisterUniqueId    (const char * aNameStr, uint32_t aLength, uint32_t aCRC32);
        void            AddReference        (TUniqueIdData* aRegisteredId);
        bool            RemoveReference     (TUniqueIdData* aRegisteredId);
    private:
        struct TData;
        TData* mData;
    };

    //----------------------------------------------------------------
    // TUniqueIdBase
    //----------------------------------------------------------------
    template <bool CS> class TUniqueIdBase
    {
    
    public:
        explicit                TUniqueIdBase   (const char * aStr = "");
                                TUniqueIdBase   (TUniqueIdBase const & aOther);
                               ~TUniqueIdBase   ();

        TUniqueIdBase const &   operator=   (TUniqueIdBase const & aOther);   
        bool                    operator==  (TUniqueIdBase const & aOther) const;
        bool                    operator!=  (TUniqueIdBase const & aOther) const;
        bool                    operator<   (TUniqueIdBase const & aOther) const;

        uint32_t                GetCRC32    () const;
        const char*             GetStr      () const;
        size_t                  GetLength   () const;
        bool                    IsEmpty     () const;

    private:
        static CUniqueIdRegistry* sUniqueIdRegistry;
        static CUniqueIdRegistry* GetRegistry();
        TUniqueIdData* mData;
    };
    
    template <bool CS> CUniqueIdRegistry* TUniqueIdBase<CS>::sUniqueIdRegistry { nullptr };

    typedef TUniqueIdBase<false> TUniqueId;
    typedef TUniqueIdBase<true> TUniqueIdCS;

#   define UID(_UID) sbx::TUniqueId _uid_##_UID( #_UID );
#   define UID_TEXT(_UID, _TEXT) sbx::TUniqueId _uid_##_UID( _TEXT );
#   define uid(_UID) _uid_##_UID


#   define UIDCS(_UID) sbx::TUniqueIdCS _uidcs_##_UID( #_UID );
#   define UIDCS_TEXT(_UID, _TEXT) sbx::TUniqueIdCS _uidcs_##_UID( _TEXT );
#   define uidcs(_UID) _uidcs_##_UID


} //namespace sbx

#include <sbx/DataStructures/TUniqueId.inl>


#endif //SBX_TUNIQUEID_H