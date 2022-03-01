// Copyright (c) 2022 David Gallardo and SDFEditor Project

#pragma once

void _sbx_write_log(const char* aFormat, ...);

#ifndef _SBX_FINAL
#define SBX_LOG(...) _sbx_write_log(__VA_ARGS__)
#define SBX_CONSOLE_LOG(...) _sbx_write_log(__VA_ARGS__)
#else
#define SBX_LOG(...) 
#define SBX_CONSOLE_LOG(...) _sbx_write_log(__VA_ARGS__)
#endif
