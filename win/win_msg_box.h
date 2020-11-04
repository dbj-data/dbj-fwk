#pragma once

#include "win32_strsafe.h"
#include "windows_includer.h"
// #include "dbj_win_buffer.h"

#include <stdbool.h>
#include <strsafe.h>
#include <crtdbg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	//  msg box no exit
	static inline void win32_msg_boxA(const char * fmt_ , ...)
	{
		DBJ_ASSERT(fmt_);
		char buff[1024] = { 0 };
		va_list argptr;
		va_start(argptr, fmt_ );
		HRESULT hr_ = win_vsprintfa(buff, 1024, fmt_, argptr) ;
		DBJ_ASSERT( S_OK == hr_ ) ;
		va_end(argptr);
		MessageBoxA(NULL, (LPCSTR)buff, this_base_namea(), MB_OK);
	}

#undef DBJ_MSGA
#ifdef _DEBUG
#define DBJ_MSGA(m_) win32_msg_boxA("%s\n\nFile : %s\nLine # %d", m_, __FILE__, __LINE__)
#else // ! _DEBUG
#define DBJ_MSGA(m_) 
#endif // ! _DEBUG

	static inline void win32_msg_boxW(const wchar_t * fmt_ , ... )
	{
		DBJ_ASSERT(fmt_);
		WCHAR buff[1024] = { (WCHAR)0 };
		va_list argptr;
		va_start(argptr, fmt_);
		HRESULT hr_ = win_vsprintfw(buff, 1024, fmt_, argptr);
		DBJ_ASSERT(S_OK == hr_);
		va_end(argptr);
		MessageBoxW(NULL, (LPCWSTR)buff, this_base_namew(), MB_OK);
	}

#undef DBJ_MSGW
#ifdef _DEBUG
#define DBJ_MSGW(m_) win32_msg_boxW(L"%s\n\nFile: %s\nLine # %d", m_, __FILEW__, __LINE__ )
#else // ! _DEBUG
#define DBJ_MSGW(m_) 
#endif // ! _DEBUG

	// SX stands for Show eXpression
#undef SXW
#define SXW(f_, x_ ) win32_msg_boxW(L"Expression:\n%s\n\nResult:\n" f_ , #x_, (x_))

#undef SXA
#define SXA(f_, x_ ) win32_msg_boxA("Expression:\n%s\n\nResult:\n" f_ , #x_, (x_))

#if 0
#undef  WIN32_ERROR_MESSAGE_SIZE
#define WIN32_ERROR_MESSAGE_SIZE 0xFF

	typedef struct win32_error_msg_box_arg
	{
		bool display_error_message;
		bool do_process_exit;
		/*DWORD*/ int last_error /* from GetLastError()*/;
		const char* error_source;
	}
	win32_error_msg_box_arg;

	typedef struct win32_error_msg {
		char data[WIN32_ERROR_MESSAGE_SIZE];
	} win32_error_msg;

	win32_error_msg
		win32_error_msg_box_exit(win32_error_msg_box_arg);

	/*
	*/
	static inline win32_error_msg
		win32_error_msg_box_exit(
			win32_error_msg_box_arg  arg_ret_val
		)
	{   // def value is empty string
		win32_error_msg result_err_msg = { (char)'\0' };

		// if last error not given ask for it
		if (arg_ret_val.last_error < 1) arg_ret_val.last_error = GetLastError();

		arg_ret_val.error_source =
			(arg_ret_val.error_source == NULL ? "null" : arg_ret_val.error_source);

		// Retrieve the system error message for the last-error code
		LPVOID lpMsgBuf = { 0 };
		LPVOID lpDisplayBuf = { 0 };

#ifndef  NDEBUG
		int format_rezult =
#endif // ! NDEBUG
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				arg_ret_val.last_error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&lpMsgBuf,
				0, NULL);

#ifndef  NDEBUG
		DBJ_ASSERT(format_rezult > 0);
		if (format_rezult < 1) {
			LocalFree(lpMsgBuf);
			return result_err_msg; // empty msg
		}
#endif // ! NDEBUG

		// Display the error message
		if (arg_ret_val.display_error_message) {

			lpDisplayBuf =
				(LPVOID)LocalAlloc(LMEM_ZEROINIT,
					(strlen((LPCSTR)lpMsgBuf) + strlen((LPCSTR)arg_ret_val.error_source) + 40) * sizeof(char)
				);

			StringCchPrintfA(
				(LPSTR)lpDisplayBuf,
				LocalSize(lpDisplayBuf) / sizeof(char),
				"%s failed\n\nerror code %d\nmessage: %s\n\n%s",
				arg_ret_val.error_source, 
				arg_ret_val.last_error, 
				(LPSTR)lpMsgBuf,
				(arg_ret_val.do_process_exit ? " Module will exit!" : "" )
			);

			MessageBoxA(NULL, (LPCSTR)lpDisplayBuf, "Error", MB_OK);
		}

		strncpy_s(result_err_msg.data, WIN32_ERROR_MESSAGE_SIZE, (LPCSTR)lpMsgBuf, WIN32_ERROR_MESSAGE_SIZE);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
		SetLastError(0); // yes this helps sometimes

		// caller asked us to exit the process
		if (arg_ret_val.do_process_exit) {
			ExitProcess(arg_ret_val.last_error);
		}
		return result_err_msg;
	} // win32_error_msg
#endif // 0
#ifdef __cplusplus
} // "C"
#endif // __cplusplus
