/*
 * @file    ErrorHandling.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_ERROR_HANDLING_H__
#define __SBX_ERROR_HANDLING_H__

#include <cstdint>
#include <sbx/Core/Platform.h>
#include <sbx/Core/Log.h>

#ifndef _SBX_FINAL
#   define SBX_ERRORS_ENABLED 1
#else
#   define SBX_ERRORS_ENABLED 0
#endif

#if SBX_ERRORS_ENABLED
#   ifdef DEBUG
#       if SBX_COMPILER_MSVC
#           include <intrin.h>
#           define SBX_DEBUG_BREAK()        __debugbreak() //__asm{ int 3 }
#       elif SBX_COMPILER_GCC
#           if SBX_OS_MACOSX || SBX_OS_IOS
#               define SBX_DEBUG_BREAK()    __builtin_trap()
#           else
#               define SBX_DEBUG_BREAK()    abort()
#           endif
#       endif   // COMPILER
#   else
#       define SBX_DEBUG_BREAK()
#   endif // DEBUG

namespace __sbx_assert
{
    template <uint32_t SIZE> char* GetAssertBuff()
    {
        static char sAssertBuff[SIZE];
        return sAssertBuff;
    }

    bool EvalAssert(bool const & aTest, char* aTestStr, char* aFile, int32_t aLine, char* aFormat = "", ...);
};

#   define SBX_ERROR(...) { SBX_LOG(__VA_ARGS__); SBX_DEBUG_BREAK(); }
#   define SBX_ASSERT(_TEST, ...) if(__sbx_assert::EvalAssert(_TEST, #_TEST, __FILE__, __LINE__, ##__VA_ARGS__)){ SBX_DEBUG_BREAK(); }else{}
#else
#   define SBX_ERROR(...) 
#   define SBX_ASSERT(_TEST, ...) 
#endif

#endif //__SBX_ERROR_HANDLING_H__