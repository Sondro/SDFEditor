/*
 * @file    Alignment.h
 * @author  David Gallardo Moreno
 */

#ifndef __SBX_ALILGNMENT_H__
#define __SBX_ALILGNMENT_H__

#include <sbx/Core/Platform.h>
#include <sbx/Core/ErrorHandling.h>

namespace sbx
{
    template<typename T> inline T  SBX_PTR_ROUND_DOWN  (T  x, uintptr_t a) { return x & -(int32_t)a; }
    template<typename T> inline T  SBX_PTR_ROUND_UP    (T  x, uintptr_t a) { return (x + (a-1)) & -(int32_t)a; }
    template<typename T> inline T* SBX_PTR_ALIGN_DOWN  (T* p, uintptr_t a) { return (T*)((uintptr_t)p & -(int32_t)a); }
    template<typename T> inline T* SBX_PTR_ALIGN_UP    (T* p, uintptr_t a) { return (T*)(((uintptr_t)p + (a-1)) & -(int32_t)a); }

    #define SBX_NEXT_MULTIPLE_OF_2(__size) ((__size + 1UL) & ~1UL)
    #define SBX_PREVIOUS_MULTIPLE_OF_2(__size) (__size & ~1UL)

    #define SBX_NEXT_MULTIPLE_OF_8(__size) ((__size + 7UL) & ~7UL)
    #define SBX_PREVIOUS_MULTIPLE_OF_8(__size) (__size & ~7UL)

    #define SBX_NEXT_MULTIPLE_OF_128(__size) ((__size + 127UL) & ~127UL)
    #define SBX_PREVIOUS_MULTIPLE_OF_128(__size) (__size & ~127UL)

    #define SBX_NEXT_MULTIPLE_OF_256(__size) ((__size + 255UL) & ~255UL)
    #define SBX_PREVIOUS_MULTIPLE_OF_256(__size) (__size & ~255UL)
    
    #define SBX_NEXT_MULTIPLE_POT2(__size, __num) ((__size + (__num - 1)) & ~(__num - 1))
    #define SBX_PREVIOUS_MULTIPLE_POT2(__size, __num) (__size & ~(__num - 1))
    
    //power of two alignment size
    struct aligned_uintptr_t
    {
    public:
        aligned_uintptr_t(uintptr_t const & aAlignment)
            : mAlignment(aAlignment)
        {
            SBX_ASSERT( !(0 == aAlignment) && !(aAlignment & (aAlignment - 1)) , "The input alignment value must be a power of two");   
        }

        operator uintptr_t() const
        {
            return mAlignment;
        }
    private:
        uintptr_t mAlignment;
    };
}

#endif //__SBX_ALILGNMENT_H__