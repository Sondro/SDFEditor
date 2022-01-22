/*
 * @file    CResourceUtils.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_RES_UTILS_H__
#define __SBX_RES_UTILS_H__

#include <sbx/FileSystem/CResource.h>

namespace sbx
{
    void            sbxDumpRes      (CResource const & aRes);
    void            sbxResToJson    (CResource const & aInRes, std::string & aOutJson, bool aPrettyPrint);
    const char*     sbxJsonToRes    (const char* aInJson, CResource & aOutRes);
    void            sbxJsonFileToRes(TPath const & aPath, CResource & aOutRes);
    void            sbxResToJsonFile(TPath const & aPath, CResource const & aRes);

} // sbx


#endif //__SBX_RES_UTILS_H__