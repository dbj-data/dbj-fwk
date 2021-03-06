#pragma once
#ifndef DBJ_WINDOWS_INCLUDE_INC_
#define DBJ_WINDOWS_INCLUDE_INC_

#include "../wallofmacros.h"
/*
(c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/

https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=vs-2019

for building we require WIN10 as minimum
*/
#include <winsdkver.h>

#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
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

#ifdef NOMINMAX
#undef  min
#define min(x, y) ((x) < (y) ? (x) : (y))

#undef  max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif // NOMINIMAX

// https://devblogs.microsoft.com/oldnewthing/20041025-00/?p=37483
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#undef  HINST_THISCOMPONENT
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

/*
NOTE: the following is deliberately decoupled,
from what is in win_cli_args.h
*/
DBJ_UNUSED_F
static inline const wchar_t* this_app_full_path_w()
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

DBJ_UNUSED_F
static inline const char* this_app_full_path_a()
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

/// --------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------
///  what is bellow was in a separate header
///  moved it in here as it is always required wherever Windows is required
/// --------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------

#ifndef DBJ_WIN_LIB_INC_
#define DBJ_WIN_LIB_INC_
// (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj

//#ifdef __STDC_ALLOC_LIB__
//#define __STDC_WANT_LIB_EXT2__ 1
//#else
//#define _POSIX_C_SOURCE 200809L
//#endif

#ifndef _WIN32
#error This is WIN32 header, treat it as such
#endif // ! _WIN32

#include <io.h> // _isatty

/// --------------------------------------------------------------------------------
/* bool might require <stdbool.h> */
#ifdef __cplusplus
extern "C" {
#endif
	/* isatty and _fileno are in io.h */
	inline bool is_stdout_redirected() {
		return ((_isatty(_fileno(stdout))) ? true : false);
	}

	inline bool is_stderr_redirected() {
		return ((_isatty(_fileno(stderr))) ? true : false);
	}

	inline bool is_stdin_redirected() {
		return ((_isatty(_fileno(stdin))) ? true : false);
	}
	/// --------------------------------------------------------------------------------
	//
	// we emulate _OSVERSIONINFOA and return osinfo_struct to the callers
	// reason is mainly, we do not like the *agrarian* notation
	//
	typedef struct osinfo_struct {
		long size_info_;
		long major;
		long minor;
		long build_num;
		long platform_id;
		char csd_version[128]; // Maintenance string for PSS usage
	} osinfo, * osinfo_ptr;

#define DBJ_OSINFO_EMPTY {0,0,0,0,0,{0}}

	// actually the sure-fire way to obtain windows version
	inline osinfo get_win_version(void)
	{
		typedef LONG NTSTATUS; // , * PNTSTATUS{};
		typedef NTSTATUS(WINAPI* get_version_fp)(osinfo_ptr);

		static HMODULE hMod = ::GetModuleHandleA("ntdll.dll");
		if (hMod) {
			static get_version_fp fxPtr =
				(get_version_fp)::GetProcAddress(hMod, "RtlGetVersion");

			_ASSERTE(fxPtr != nullptr);

			if (fxPtr != nullptr) {
				osinfo osinfo_var_ = DBJ_OSINFO_EMPTY;
				osinfo_var_.size_info_ = sizeof(osinfo_var_);
				if ((0x00000000) == fxPtr(&osinfo_var_)) {
					return osinfo_var_;
				}
			}
		}
		static osinfo osinfo_empty_ = DBJ_OSINFO_EMPTY; //{ 0, 0, 0, 0, 0, {0} };
		return osinfo_empty_;
	} // get_win_version

#define DBJ_IS_OS_INFO_EMPTY(info_) (0 == info_.major)

/*
return true if Windows version is greater than or equal to the specified number

If the Windows version is equal to or greater than 10.0.14393
then ENABLE_VIRTUAL_TERMINAL_PROCESSING is supported.

*/
	inline bool is_win_ver_or_greater(WORD major_ver, WORD minor_ver,
		WORD build_ver) {
		osinfo ver_info_ = get_win_version();

		if (DBJ_IS_OS_INFO_EMPTY(ver_info_))
			return false;

		if (ver_info_.major > major_ver)
			return true;
		else if (ver_info_.major < major_ver)
			return false;

		if (ver_info_.minor > minor_ver)
			return true;
		else if (ver_info_.minor < minor_ver)
			return false;

		//  we are here because both major and minor are equal
		if (ver_info_.build_num >= build_ver)
			return true;
		return false;
	}

#undef DBJ_IS_OS_INFO_EMPTY

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#error ENABLE_VIRTUAL_TERMINAL_PROCESSING not found? Try Visual Studio solution re-targeting to the latest SDK.
#endif

	/*
	current machine may or may not  be on WIN10 where VT100 ESC codes are
	*supposed to be* on by default, Sigh ...

	You can use the run-time method bellow. if it returns false no VT100 colours.
	WARNING: this will not exit the app *only* if app is started in WIN32 CONSOLE
	Example: if running from git bash on win this will exit the app
	if app output is redirected to file, this will also fail.
	at runtime.

	If the Windows version is equal to or greater than: 10.0.14393
	then ENABLE_VIRTUAL_TERMINAL_PROCESSING is supported.

	If your win10 terminal is not showing colours use this
	it will not exit the app *only* if app is started in WIN32 CONSOLE
	Example: if running from git bash on win this will exit the app
	if app output is redirected to file, this will also fail.
	*/
	inline bool win_enable_vt_100_and_unicode() {
		static bool visited = false;
		if (visited)
			return true;

		int rez = SetConsoleOutputCP(CP_UTF8 /*65001*/);
		{
			if (rez == 0) {
				DBJ_PERROR;
				return false;
			}
		}
		// Set output mode to handle virtual terminal sequences
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE) {
			DBJ_PERROR;
			return false;
		}

		DWORD dwMode;
		if (!GetConsoleMode(hOut, &dwMode)) {
			fprintf(stderr, "\nFile: %s\nLine: %ul\nWhy: %s\n", __FILE__, __LINE__,
				", GetConsoleMode() failed");
			fprintf(stderr, "\nPlease re-run in either WIN console %s",
				" or powershell console\n");
			DBJ_PERROR;
			return false;
		}

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hOut, dwMode)) {
			DBJ_PERROR;
			return false;
		}
		visited = true;
		return true;
	}

	/*
	Where is the font_name coming from?

	vs win32 immensely complex method for enumerating fonts, font names are easy to
	find, just open your cmd.exe and go to properties; or if you like to be exact
	find them here:
	https://docs.microsoft.com/en-us/typography/fonts/windows_10_font_list

	font_height_ if 0 will not be changed, other than that it can be any number
	between 7 and 145. That is pixels.
	*/
	inline bool win_set_console_font(wchar_t* font_name,
		SHORT font_height_ /*= SHORT(0)*/) {
		CONSOLE_FONT_INFOEX font_info;
		font_info.cbSize = sizeof(CONSOLE_FONT_INFOEX);

		HANDLE con_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (con_out_handle == INVALID_HANDLE_VALUE)
			return false;

		BOOL rez_ = GetCurrentConsoleFontEx(con_out_handle, TRUE, &font_info);

		if (rez_ == 0) {
			DBJ_PERROR;
			exit(-1);
		}

		// set the new font name
		(void)memset(font_info.FaceName, 0, LF_FACESIZE);
		(void)memcpy(font_info.FaceName, font_name, lstrlenW(font_name));

		// if requested set the new font size
		if (font_height_ > 0) {
			// quietly discard the silly sizes
			if ((font_height_ > 7) && (font_height_ < 145)) {
				font_info.dwFontSize.Y = font_height_;
			}
		}

		rez_ = SetCurrentConsoleFontEx(con_out_handle,
			TRUE, /* for the max window size */
			&font_info);

		if (rez_ == 0) {
			DBJ_PERROR;
			exit(-1);
		}
		return true;
	}

#ifdef __cplusplus
} // extern "C" {
#endif

#endif // DBJ_WIN_LIB_INC_
