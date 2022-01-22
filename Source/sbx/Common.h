/*
 * @file    Common.h
 * @author  David Gallardo Moreno
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <typeinfo>
#include <sbx/Core/Platform.h>
#include <sbx/Core/Log.h>
#include <sbx/Core/Memory.h>

#ifndef _SBX_FINAL
#define SBX_NEW(_T, ...)    sbx::LogObjectAllocation(new (sbxMalloc(sizeof(_T))) _T(__VA_ARGS__))
#define SBX_DELETE(_PTR)    sbxFree(sbx::LogObjectDeletion(_PTR))
#define SBX_MALLOC(_SIZE)   sbxMalloc(_SIZE)
#define SBX_FREE(_PTR)      sbxFree(_PTR)
#define SBX_REALLOC(_PTR, _SIZE)    sbxRealloc(_PTR, _SIZE)
#define SBX_ALIGNED_MALLOC(_ALIGNMENT, _SIZE)   sbx::LogRawAllocation(_SIZE, SBX_PLATFORM_ALIGNED_MALLOC(_ALIGNMENT, _SIZE))
#define SBX_ALIGNED_FREE(_PTR)                  sbx::LogRawFree(_PTR); SBX_PLATFORM_ALIGNED_FREE(_PTR)
#else 
#define SBX_NEW(_T, ...)    new _T(__VA_ARGS__)
#define SBX_DELETE(_PTR)    delete _PTR
#define SBX_MALLOC(_SIZE)   ::malloc(_SIZE)
#define SBX_FREE(_PTR)      ::free(_PTR)
#define SBX_REALLOC(_PTR, _SIZE)    ::realloc(_PTR, _SIZE)
#define SBX_ALIGNED_MALLOC(_SIZE)   SBX_PLATFORM_ALIGNED_MALLOC(_SIZE)
#define SBX_ALIGNED_FREE(_PTR)      SBX_PLATFORM_ALIGNED_FREE(_PTR)
#endif
#define SBX_NEW_SILENT(_T, ...)     new _T(__VA_ARGS__)
#define SBX_DELETE_SILENT(_PTR)     delete _PTR
#define SBX_PLACEMENT_NEW(_T, _PTR, ...) new (_PTR) _T(__VA_ARGS__)
#define SBX_NEW_ARRAY(_T, _N)       new _T[_N]
#define SBX_DELETE_ARRAY(_PTR)      delete [] _PTR



#include <sbx/Core/ErrorHandling.h>
#include <sbx/DataStructures/TUniqueId.h>
#include <sbx/Utils/LangUtils.h>
//#include <FileSystem/CJsonRes.h>
//#include <functional>
