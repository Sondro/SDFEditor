/*
 * @file    ErrorHandling.cpp
 * @author  David Gallardo Moreno
 */

#include <sbx/Common.h>

#if SBX_ERRORS_ENABLED
#include <stdarg.h>

namespace __sbx_assert
{
    /*void EvalAssert(bool const & aTest, char* aTestStr)
    {
        EvalAssert(aTest, aTestStr, "");
    }*/

    bool EvalAssert(bool const & aTest, char* aTestStr, char* aFile, int32_t aLine, char* aFormat, ...)
    {
        if(!aTest)
        { 
            va_list  lArgsList;
            va_start(lArgsList, aFormat);
            ::vsnprintf(__sbx_assert::GetAssertBuff<1024>(), 1024, aFormat, lArgsList);
            va_end(lArgsList);
            SBX_LOG("[Assert in %s:%d] (%s) - %s", aFile, aLine, aTestStr, __sbx_assert::GetAssertBuff<1024>()); 

            return true;//TODO: modal to ask if stop, no stop, ignore rest
        }

        return false;
    }
}

#endif