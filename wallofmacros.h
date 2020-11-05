#pragma once
#ifndef DBJ_NANOC_INCLUDED
#define DBJ_NANOC_INCLUDED
/*
   (c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/
*/
#ifdef __clang__
#pragma clang system_header
#endif

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else // ! __STDC_ALLOC_LIB__
#define _POSIX_C_SOURCE 200809L
#endif // ! __STDC_ALLOC_LIB__

#include <crtdbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "printing_macros.h"

// -------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
/*-------------------------------------------------------------------------------*/

#if defined(__clang__) 
#define DBJ_PURE_FUNCTION __attribute__((const))
#define DBJ_CTOR_FUNCTION __attribute__((constructor))
#define DBJ_DTOR_FUNCTION __attribute__((destructor))
#define DBJ_NRET_FUNCTION __attribute__((noreturn))
#else
#define DBJ_PURE_FUNCTION 
#define DBJ_CTOR_FUNCTION 
#define DBJ_DTOR_FUNCTION 
#define DBJ_NRET_FUNCTION
#endif

// Here's a better C version (from Google's Chromium project):

#undef   DBJ_COUNT_OF
#define DBJ_COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

/// https://stackoverflow.com/a/29253284/10870835

#if (! defined (_DEBUG)) &&  (! defined (NDEBUG))
#error  NDEBUG *is* standard macro and has to exist.
#endif

#undef DBJ_RELEASE_BUILD
#ifdef NDEBUG 
#define DBJ_RELEASE_BUILD
#endif

/// -------------------------------------------------------------------------------
#undef  DBJ_PERROR 
#ifndef NDEBUG
#define DBJ_PERROR (perror(__FILE__ " # " _CRT_STRINGIZE(__LINE__))) 
#else
#define DBJ_PERROR
#endif // NDEBUG

#undef DBJ_FERROR
#ifdef _DEBUG
#define DBJ_FERROR( FP_) \
do { \
if (ferror(FP_) != 0) {\
	DBJ_PERROR ;\
	clearerr_s(FP_);\
} \
} while(0)
#else
#define DBJ_FERROR( FP_ )
#endif // _DEBUG

/// -------------------------------------------------------------------------------
/// stolen from vcruntime.h 
#define _DBJ_STRINGIZE_(x) #x
#define _DBJ_STRINGIZE(x) _DBJ_STRINGIZE_(x)

#define _DBJ_WIDE_(s) L##s
#define _DBJ_WIDE(s) _DBJ_WIDE_(s)

#define _DBJ_CONCATENATE_(a, b) a##b
#define _DBJ_CONCATENATE(a, b) _DBJ_CONCATENATE_(a, b)

#define _DBJ_EXPAND_(s) s
#define _DBJ_EXPAND(s) _DBJ_EXPAND_(s)

#ifdef _MSVC_LANG
// https://developercommunity.visualstudio.com/content/problem/195665/-line-cannot-be-used-as-an-argument-for-constexpr.html
#define DBJ_CONSTEXPR_LINE long(_DBJ_CONCATENATE(__LINE__, U))
#else
#define DBJ_CONSTEXPR_LINE __LINE__
#endif

///-----------------------------------------------------------------------------------------
#ifdef _KERNEL_MODE
#define DBJ_NONPAGESECTION __declspec(code_seg("$dbj_fwk_kerneltext$"))
#else
#define DBJ_NONPAGESECTION
#endif // _KERNEL_MODE

// -----------------------------------------------------------------------------
#undef DBJ_ASSERT
#define DBJ_ASSERT _ASSERTE

//
// CAUTION! DBJ_VERIFY works in release builds too
#undef DBJ_VERIFY
#undef DBJ_VERIFY_

#define DBJ_VERIFY_(x, file, line) \
	if (false == x)                \
	dbj_nanoc_terror("Expression: " #x ", failed ", file, line)

#define DBJ_VERIFY(x) DBJ_VERIFY_(x, __FILE__, __LINE__)

// NOTE: this is wide string version
#undef DBJ_DEBUG_REPORTW
#define DBJ_DEBUG_REPORTW(expr, msg) \
(void)(                                                                                     \
    (!!(expr)) ||                                                                           \
    (1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, L"%ls", msg)) || \
    (_CrtDbgBreak(), 0)                                                                     \
)


// NOTE: notice the transformation to WCHAR and calling 
// DBJ_VERIFY which is WCHAR only
#undef DBJ_DEBUG_REPORT
#ifdef _DEBUG
#define DBJ_DEBUG_REPORT(x) DBJ_DEBUG_REPORTW((x), _CRT_WIDE(#x) )
#else
#define DBJ_DEBUG_REPORT(x) 
#endif


/*
we use the macros bellow to create ever needed location info always
associated with the offending expression
timestamp included
*/
#undef  DBJ_FILE_LINE 
#define DBJ_FILE_LINE __FILE__ "(" _CRT_STRINGIZE(__LINE__) ")"

#undef  DBJ_FILE_LINE_TSTAMP
#define DBJ_FILE_LINE_TSTAMP __FILE__ "(" _CRT_STRINGIZE(__LINE__) ")[" __TIMESTAMP__ "] "

#undef  DBJ_FLT_PROMPT
#define DBJ_FLT_PROMPT(x) DBJ_FILE_LINE_TSTAMP _CRT_STRINGIZE(x)

/* will not compile if MSG_ is not string literal */
#undef  DBJ_ERR_PROMPT
#define DBJ_ERR_PROMPT(MSG_) DBJ_FILE_LINE_TSTAMP MSG_

// -----------------------------------------------------------------------------
/*
dbj_nanoc_terror == terminating error
NOTE: std::exit *is* different to C API exit()
NOTE: all the bets are of so no point of using some logging
*/
	inline void dbj_terror(const char* msg_, const char* file_, const int line_)
	{
		/// DBJ_ASSERT(msg_ && file_ && line_);
		/// all the bets are of so no point of using some logging
		perror("\n\n" DBJ_ERR_PROMPT("\n\ndbj NANOCLIB Terminating error!"));
		exit(EXIT_FAILURE);
	}

	/*
	2020 SEP 03 -- it turns out (again) we need to initialize WIN10 terminal
	to show us VT100 colours
	*/
	inline void dbj_win_vt100_initor_() {
		// and ... this is it ... awfull really ;)
		// ps: make sure it is not empty string!
		system(" ");
	}

#ifdef __cplusplus
} // extern "C" 
#endif

#endif // DBJ_NANOC_INCLUDED
