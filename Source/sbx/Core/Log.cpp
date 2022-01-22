
#include <sbx/Common.h>
#include <sbx/Core/Log.h>
#include <vector>
#include <string>

#include <stdio.h>
#include <Windows.h>

#include <mutex>

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
    OutputDebugString(lMessageBuffer);
}

