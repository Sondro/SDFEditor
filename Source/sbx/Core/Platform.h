/*
 * @file    Platform.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_PLATFORM_H__
#define __SBX_PLATFORM_H__

/*
 * Platform and Architecture detection
 */

#ifdef _WIN32
#   define SBX_OS_WINDOWS           1
#   ifndef _M_ARM_FP
#   ifdef _WIN64
#       define SBX_ARCH_X86_64      1
#       else
#           define SBX_ARCH_I386    1
#       endif
#   else
#       ifdef _WIN64
#           define SBX_ARCH_ARM64   1
#       else
#           define SBX_ARCH_ARM32   1
#       endif
#   endif
#elif defined(__ANDROID__)
#   define SBX_OS_ANDROID      1
#   ifdef __arm__
#       define SBX_ARCH_ARM32  1
#   elif defined(__i386__)
#       define SBX_ARCH_I386   1
#   else
#       define SBX_ARCH_ARM64  1
#   endif

#elif defined(__linux__)
#   define SBX_OS_LINUX        1
#   ifdef __i386
#       define SBX_ARCH_I386   1
#   elif defined(LINUX64)
#       define SBX_ARCH_X86_64 1
#   endif

#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#       define SBX_OS_IOS          1
#       if !__LP64__
#           define SBX_ARCH_ARM32  1
#       else
#           define SBX_ARCH_ARM64  1
#       endif
#   elif TARGET_OS_MAC
#       define SBX_OS_MACOSX       1
#       if !__LP64__
#           define SBX_ARCH_I386   1
#       else
#           define SBX_ARCH_X86_64 1
#       endif
#   else
#       error Unsupported platform
#   endif
#else
#   error Unsupported platform
#endif



/*
 * Operative Systems
 */
#ifndef SBX_OS_WINDOWS
#   define SBX_OS_WINDOWS  0
#endif
#ifndef SBX_OS_LINUX
#   define SBX_OS_LINUX    0
#endif
#ifndef SBX_OS_MACOSX
#   define SBX_OS_MACOSX   0
#endif
#ifndef SBX_OS_IOS
#   define SBX_OS_IOS      0
#endif
#ifndef SBX_OS_ANDROID
#   define SBX_OS_ANDROID  0
#endif

/*
 * Architectures
 */
#ifndef SBX_ARCH_I386
#   define SBX_ARCH_I386       0
#endif
#ifndef SBX_ARCH_X86_64
#   define SBX_ARCH_X86_64     0
#endif
#ifndef SBX_ARCH_ARM32
#   define SBX_ARCH_ARM32      0
#endif
#ifndef SBX_ARCH_ARM64
#   define SBX_ARCH_ARM64      0
#endif

/*
 * Architecture Bits
 */
#if SBX_ARCH_X86_64 || SBX_ARCH_ARM64
#   define SBX_ARCH_64BITS        1
#   define SBX_ARCH_32BITS        0
#   define SBX_DEFAULT_PTR_T      unsigned long
#elif SBX_ARCH_I386 || SBX_ARCH_ARM32
#   define SBX_ARCH_64BITS        0
#   define SBX_ARCH_32BITS        1
#   define SBX_DEFAULT_PTR_T      unsigned long
#else
#   error Unnable to determine memory address size
#endif


/*
 * Compiler detection and compiler-specific instructions
 */
#ifdef _MSC_VER
#   define SBX_COMPILER_MSVC   1
#   define SBX_MSVC_ALIGN(a)   __declspec(align(a))
#   define SBX_PLATFORM_ALIGNED_MALLOC(_ALIGNMENT, _SIZE)   ::_aligned_malloc(_SIZE, _ALIGNMENT)
#   define SBX_PLATFORM_ALIGNED_FREE(_PTR)                  ::_aligned_free(_PTR)
#elif __GNUC__
#   define SBX_COMPILER_GCC    1
#   define SBX_GCC_ALIGN(a)    __attribute__((aligned(a)))
#   define SBX_PLATFORM_ALIGNED_MALLOC(_ALIGNMENT, _SIZE)   ::aligned_malloc(_ALIGNMENT, _SIZE)
#   define SBX_PLATFORM_ALIGNED_FREE(_PTR)                  ::free(_PTR)
#else
#   error Unknown compiler.
#endif

#ifndef SBX_COMPILER_MSVC
#   define SBX_COMPILER_MSVC    0
#endif
#ifndef SBX_COMPILER_GCC
#   define SBX_COMPILER_GCC     0
#endif

#ifndef SBX_MSVC_ALIGN
#   define SBX_MSVC_ALIGN(a)
#endif

#ifndef SBX_GCC_ALIGN
#   define SBX_GCC_ALIGN(a)
#endif

/*
 * IDE Macros
 */
#ifndef __LINE__
#   define __LINE__    0
#endif
#ifndef __FILE__
#   define __FILE__    L"UNKNOWN_FILE"
#endif
#ifndef __DATE__
#   define __DATE__    0
#endif
#ifndef __TIME__
#   define __TIME__    0
#endif

#endif //__SBX_PLATFORM_H__
