
#include <sbx/Core/Log.h>
#include <sbx/Core/Platform.h>

#if SBX_OS_WINDOWS
#   include <Windows.h>
#endif

#include <mutex>
#include <cstdarg>

void _sbx_write_log_va_list(const char* aFormat, va_list aArgsList);

void _sbx_write_log(const char* aFormat, ...)
{
    va_list  lArgsList;
    va_start(lArgsList, aFormat);
    _sbx_write_log_va_list(aFormat, lArgsList);
    va_end(lArgsList);
}

void _sbx_write_log_va_list(const char* aFormat, va_list aArgsList)
{
    static std::mutex sUniqueRegistryMutex; 
    std::lock_guard< std::mutex > lScopedMutex(sUniqueRegistryMutex);
    static char lMessageBuffer[49152];
    vsprintf_s(lMessageBuffer, sizeof(lMessageBuffer), aFormat, aArgsList);
    strcat_s(lMessageBuffer, sizeof(lMessageBuffer), "\n");

#if SBX_OS_WINDOWS
    OutputDebugString(lMessageBuffer);
#elif SBX_OS_MACOSX
    NSLog(@"%s", lMessageBuffer);
#else
    ::printf("%s", lMessageBuffer);
#endif
    
}

