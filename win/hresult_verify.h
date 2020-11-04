#pragma once
/*
NOTE: this code depends on dbj_simplelog
NOTE: dbj_simplelog does not support wchar_t 
*/

// #include "win_char_transformations.h"

//#include "win_msg_box.h"
//#include <crtdbg.h>
#include <comdef.h>

#ifndef _UNICODE
#error Unicode build is required 
#endif // _UNICODE

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	/* NOTE: this is C++ code inside extern "C" { } block */
	/* NOTE: process exits on anything but S_OK */
	static inline bool verify_hresult_(HRESULT hr_, const wchar_t* file_ = L"", const long line = 0)
	{
		if (S_OK == hr_) return true;

		_com_error  comerr(hr_);

		//dbj_w_string err_msg_w_ = dbj_w_string_make( comerr.ErrorMessage() ) ;
		//dbj_a_string err_msg_a = dbj_wide_to_narrow( err_msg_w_ );
		
		if (line > 0)
			dbj_log_fatal("HRESULT != S_OK !! IMMEDIATE EXIT !! '%S' Location:%S(# %d)", comerr.ErrorMessage(), file_, line);
		else
			dbj_log_fatal("HRESULT != S_OK !! IMMEDIATE EXIT !! '%S'", comerr.ErrorMessage());

#ifdef _CONSOLE
#error This should not happen?
		ExitProcess((UINT)hr_);
#endif
		dbj_log_info("Stopping in an orderly fashion. WM_QUIT passed.");
		PostThreadMessage((DWORD)GetCurrentThreadId(), WM_QUIT, 0, 0);

		return false; // will be not reached
	}

#ifdef __cplusplus
} // extern "C" 
#endif // __cplusplus

#undef VERIFY_S_OK

#ifdef _DEBUG
#define VERIFY_S_OK(HR_) ( verify_hresult_( HR_, __FILEW__, __LINE__) )
#else
#define VERIFY_S_OK(HR_) ( verify_hresult_( HR_) )
#endif
