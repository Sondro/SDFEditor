/*
 * @file    FileTypes.h
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "FileTypes.h"

namespace sbx
{
    static TUniqueIdCS sFileTypesExtension[] =
    {
#       define SBX_FILE_TYPE(_name, _ext, _filter) TUniqueIdCS(_ext),
#       include <sbx/FileSystem/FileTypes.def>
        TUniqueIdCS(),
    };

    TUniqueIdCS GetFileTypeExtension (EFileType aFileType)
    {
        if(aFileType < EFileType::eCount && aFileType != EFileType::eInvalid)
        {
            return sFileTypesExtension[uint32_t(aFileType)];
        }

        return TUniqueIdCS();
    }

    EFileType GetFileTypeEnumFromExtension (TUniqueIdCS const & aFileExtension)
    {
        for(uint32_t i = 0; i < uint32_t(EFileType::eCount); ++i)
        {
            if(sFileTypesExtension[i] == aFileExtension)
            {
                return EFileType(i);
            }
        }
        
        return EFileType::eInvalid;
    }

    const char* GetFileTypeFilter (EFileType aFileType)
    {
        static const char* sFileTypesFilters[] =
        {
#           define SBX_FILE_TYPE(_name, _ext, _filter) _filter,
#           include <sbx/FileSystem/FileTypes.def>
#           undef SBX_FILE_TYPE
            "",
        };

        if(aFileType < EFileType::eCount && aFileType != EFileType::eInvalid)
        {
            return sFileTypesFilters[uint32_t(aFileType)];
        }

        return "";
    }
};