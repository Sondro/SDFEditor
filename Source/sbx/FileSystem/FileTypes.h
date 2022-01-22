/*
 * @file    FileTypes.h
 * @author  David Gallardo Moreno
 * @brief   Supported file types header
 */

#ifndef __SBX_FILE_TYPES_H__
#define __SBX_FILE_TYPES_H__

namespace sbx
{
    enum class EFileType
    {
#       define SBX_FILE_TYPE(_name, _ext, _filter) e##_name##File,
#       include <sbx/FileSystem/FileTypes.def>
#       undef SBX_FILE_TYPE
        eCount,
        eInvalid = -1
    };

    TUniqueIdCS GetFileTypeExtension    (EFileType aFileType);
    EFileType GetFileTypeEnumFromExtension (TUniqueIdCS const & aFileExtension);
    const char* GetFileTypeFilter  (EFileType aFileType);
}

#endif // __SBX_FILE_TYPES_H__