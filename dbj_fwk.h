#pragma once
/*
(c) 2020 by dbj.org -- https://dbj.org/license-dbj

users of dbj-fwk need to include this header
*/

#ifndef _WIN32

#define NOMINMAX
#define STRICT 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif // ! _WIN32

// https://devblogs.microsoft.com/oldnewthing/20041025-00/?p=37483
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#undef  HINST_THISCOMPONENT 
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#ifdef _UNICODE

#ifndef _WIN64
#pragma comment(linker, "/include:_wWinMain@16")
#else
#pragma comment(linker, "/include:wWinMain")
#endif

#else // ! _UNICODE

#ifndef _WIN64
#pragma comment(linker, "/include:_WinMain@16")
#else
#pragma comment(linker, "/include:WinMain")
#endif

#endif