#pragma once

#include "windows_includer.h"
#include "win32_strsafe.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	enum { dbj_a_w_string_max_size = 1024 } ;

	typedef struct dbj_a_string {
		char data[dbj_a_w_string_max_size];
	} dbj_a_string ;

	typedef struct dbj_w_string {
		wchar_t data[dbj_a_w_string_max_size];
	} dbj_w_string;

	static inline dbj_a_string dbj_a_string_make(const char * str_a_)
	{
		size_t size_ = lstrlenA(str_a_);
		DBJ_VERIFYX(size_ < dbj_a_w_string_max_size );
		dbj_a_string dbj_a_string = { L'\0' };
		HRESULT hr_ = win_sprintfa(dbj_a_string.data, dbj_a_w_string_max_size, "%s", str_a_);
		DBJ_VERIFYX(hr_ == S_OK);
		return dbj_a_string;
	}

	static inline dbj_w_string dbj_w_string_make( const wchar_t * str_w_ )
	{
		size_t size_ = lstrlenW(str_w_);
		DBJ_VERIFYX(size_  < dbj_a_w_string_max_size);
		dbj_w_string wstring_ = { L'\0' };
		HRESULT hr_ = win_sprintfw(wstring_.data, dbj_a_w_string_max_size, L"%s", str_w_ );
		DBJ_VERIFYX( hr_ == S_OK );
		return wstring_;
	}

	/*
	CP_ACP == ANSI
	CP_UTF8
	*/
	typedef enum { dbj_default_code_page = CP_UTF8	} dbj_win_string_transformations_enum ;

	static inline dbj_w_string dbj_narrow_to_wide(dbj_a_string a_string_)
	{
		const int slength = (int)strnlen_s(a_string_.data, dbj_a_w_string_max_size);

		const int required_len = MultiByteToWideChar(
			dbj_default_code_page, 0, a_string_.data , slength, 0, 0
		);

		DBJ_VERIFYX( required_len < (dbj_a_w_string_max_size - 1) );

		dbj_w_string w_string_ = { L'\0' };

		MultiByteToWideChar(
			dbj_default_code_page, 0, a_string_.data, slength, w_string_.data , required_len
		);
		return w_string_;
	}

	static inline dbj_a_string dbj_wide_to_narrow(dbj_w_string w_string_ )
	{
		const int slength =  (int)wcsnlen_s( w_string_.data, dbj_a_w_string_max_size) ;
		dbj_a_string a_string_ = { '\0' };

		const int required_len = WideCharToMultiByte(
			dbj_default_code_page, 0, w_string_.data, slength, a_string_.data, slength, NULL, NULL
		);

		DBJ_VERIFYX(required_len < (dbj_a_w_string_max_size - 1));

		WideCharToMultiByte(
			dbj_default_code_page, 0, w_string_.data, slength, a_string_.data, required_len, NULL, NULL
		);
		return a_string_;
	}

#ifdef __cplusplus
} // extern "C" 
#endif // __cplusplus
