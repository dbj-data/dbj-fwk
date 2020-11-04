#pragma once
#ifndef DBJ_WINDOWS_INCLUDE_INC_
#define DBJ_WINDOWS_INCLUDE_INC_
/*
(c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/

https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=vs-2019

//
// _WIN32_WINNT version constants
//
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10

set the WINVER and _WIN32_WINNT macros to the oldest supported platform 
*/
#include <winsdkver.h>

#ifndef WINVER
#define WINVER _WIN32_WINNT_WIN10
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

//-------------------------------------------------------------------------------

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <crtdbg.h>
#include <errno.h>

#define NOMINMAX
#define STRICT 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <processenv.h>

#ifdef DBJ_MINIMAX
#undef  min
#define min(x, y) ((x) < (y) ? (x) : (y))

#undef  max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif // DBJ_MINIMAX

#undef DBJ_ASSERT
#define DBJ_ASSERT _ASSERTE


#undef DBJ_PERROR
#define DBJ_PERROR (perror(__FILE__ " # " _CRT_STRINGIZE(__LINE__)))

// ALWAYS WORKS!!! in release builds too, be carefull!
// NOTE: this is wide string version
#ifndef DBJ_VERIFY
#define DBJ_VERIFY(expr, msg) \
(void)(                                                                                     \
    (!!(expr)) ||                                                                           \
    (1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, L"%ls", msg)) || \
    (_CrtDbgBreak(), 0)                                                                     \
)
#endif

#ifndef DBJ_VERIFYX
#define DBJ_VERIFYX(x) DBJ_VERIFY((x), _CRT_WIDE(#x) )
#endif //! DBJ_VERIFYX

// Here's a better C version (from Google's Chromium project):
#define DBJ_COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

static inline const wchar_t* const this_app_full_path_w()
{
	static wchar_t full_path[1024] = { L'\0' };
	if (full_path[0] == L'\0') {
		int rez = GetModuleFileNameW(
			(HMODULE)NULL, full_path, 1024
		);

		DBJ_ASSERT(rez != 0);
	}
	return full_path;
}

static inline const char * const this_app_full_path_a()
{
	static char full_path[1024] = { '\0' };
	if (full_path[0] == '\0') {
		int rez = GetModuleFileNameA(
			(HMODULE)NULL, full_path, 1024
		);
		DBJ_ASSERT(rez != 0);
	}
	return full_path;
}

	// with no extension
static inline const wchar_t* this_base_namew()
{
	static wchar_t this_base_name[0xFF] = { L'\0' };

	if (this_base_name[0] == L'\0') {

		wchar_t this_file[1024] = { L'\0' };

		int rez = GetModuleFileNameW(
			(HMODULE)NULL, this_file, 1024
		);

		DBJ_ASSERT(rez != 0);

		wchar_t* last_slash = wcsrchr(this_file, L'\\');
		wchar_t* last_dot = wcsrchr(this_file, L'.');
		wchar_t* walker = last_slash + 1;

		DBJ_ASSERT(last_slash);
		DBJ_ASSERT(last_dot);
		DBJ_ASSERT(walker);

		int this_base_name_idx = 0;
		while (walker != last_dot) {
			this_base_name[this_base_name_idx++] = *walker;
			walker++;
		}
		this_base_name[this_base_name_idx] = L'\0';

		return this_base_name;
	}

	return this_base_name;
}

static inline const char* this_base_namea()
{
	static char this_base_name[0xFF] = { '\0' };

	if (this_base_name[0] == '\0') {

		char this_file[1024] = { '\0' };

		int rez = GetModuleFileNameA(
			(HMODULE)NULL, this_file, 1024
		);

		DBJ_ASSERT(rez != 0);

		char* last_slash = strchr(this_file, L'\\');
		char* last_dot = strchr(this_file, L'.');
		char* walker = last_slash + 1;

		DBJ_ASSERT(last_slash);
		DBJ_ASSERT(last_dot);
		DBJ_ASSERT(walker);

		int this_base_name_idx = 0;
		while (walker != last_dot) {
			this_base_name[this_base_name_idx++] = *walker;
			walker++;
		}
		this_base_name[this_base_name_idx] = '\0';

		return this_base_name;
	}

	return this_base_name;
}


#endif // DBJ_WINDOWS_INCLUDE_INC_
