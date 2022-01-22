/*
 * @file    SystemUtils.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX__SYSTEMUTILS_H__
#define __SBX__SYSTEMUTILS_H__

#include <sbx/FileSystem/FileTypes.h>

namespace sbx { namespace sys
{
        bool            OpenFileDialog          (char* aOutFilePath, uint32_t aMaxLen, EFileType aType);
        bool            SaveFileDialog          (char* aOutFilePath, uint32_t aMaxLen, EFileType aType);
        bool            BrowsePathDialog        (char* aOutPath, uint32_t aMaxLen);
        void            GetCurrentDir           (char* aOutPath, size_t aMaxLen);
        void            GetDocumentsPath        (char* aOutPath, size_t aMaxLen);
        
}}; //sbx::sys

#endif // __SBX__SYSTEMUTILS_H__