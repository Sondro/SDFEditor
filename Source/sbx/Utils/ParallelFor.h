/*
 * @file    ParallelFor.h
 * @author  David Gallardo Moreno
 */


#ifndef __SBX_PARALLEL_FOR_H__
#define __SBX_PARALLEL_FOR_H__

#include <functional>

namespace sbx
{
    typedef std::function< void(uint32_t) > TParallelFnc;

    uint32_t    GetNumLogicCores    ();
    void        ParallelFor         (uint32_t aNumJobs, TParallelFnc const & aKernel);
};

#endif //__SBX_PARALLEL_FOR_H__