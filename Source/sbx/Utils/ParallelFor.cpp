/*
 * @file    ParallelFor.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>
#include "ParallelFor.h"
#include <sbx/Library/FVector.h>
#include <thread>

namespace sbx
{
    uint32_t GetNumLogicCores()
    {
        return std::thread::hardware_concurrency();
    }

    void ParallelFor(uint32_t aNumJobs, TParallelFnc const & aKernel)
    {
        FVector< std::thread > lJobsList;
        lJobsList.reserve(aNumJobs);

        for(uint32_t lIndex = 0; lIndex < aNumJobs; ++lIndex)
        {
            lJobsList.push_back(std::thread(aKernel, lIndex));
        }

        for(uint32_t lIndex = 0; lIndex < aNumJobs; ++lIndex)
        {
            lJobsList[lIndex].join();
        }
    }
} // sbx