/*
 * @file    LangUtils.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include <sbx/Utils/LangUtils.h>
#include <sbx/System/CMemoryProfiler.h>

namespace sbx
{
    uint32_t __type_id_counter::sTypeIdCounter = 0;

    //TODO: add more information about allocations
    void ManageObjectAllocationInfo(uint32_t aTypeId, const char* aTypeIdName, size_t aSize, void* aObj)
    {
        //SBX_LOG("Instancing %s at 0x%016llx", aTypeIdName, aObj);
        CMemoryProfiler::GetInstance()->AddAllocation(aTypeId, aTypeIdName, aSize);
    }

    void ManageObjectDeleteInfo(uint32_t aTypeId, const char* aTypeIdName, void* aObj)
    {
        //SBX_LOG("Deleting %s at 0x%016llx", aTypeIdName, aObj);
        CMemoryProfiler::GetInstance()->RemoveAllocation(aTypeId, aTypeIdName);
    }
} // sbx::__type_id_counter


