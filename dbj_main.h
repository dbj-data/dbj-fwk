#pragma once
/* (c) 2019-2021 by dbj.org   -- https://dbj.org/license_dbj/

This is where dbj-fwk users might use their own all exception catching
(or not)
*/
#include "meta.h"
// #include "generate_dump.h"
// #include "vt100.h"

// Depending on _HAS_EXCEPTIONS
// MS STL transforms to non standard  std lib
/*
<vcruntime.h> #100

#ifndef _HAS_EXCEPTIONS // Predefine as 0 to disable exceptions
	#ifdef _KERNEL_MODE
		#define _HAS_EXCEPTIONS 0
	#else
		#define _HAS_EXCEPTIONS 1
	#endif // _KERNEL_MODE
#endif // _HAS_EXCEPTIONS

*/
#ifdef DBJ_NEW_DEL_REPLACEMENTS
#include <new>
// --------------------------------------------------------------------------
// optional
void dbj_new_handler() noexcept
{
	perror(DBJ_ERR_PROMPT("Memory allocation failed, terminating"));
	exit(EXIT_FAILURE);
}

inline auto init_once = []() { std::set_new_handler(dbj_new_handler); }();

// --------------------------------------------------------------------------
// https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019

#ifdef __clang__
#define NEW_DEL_NOEXCEPT
#else // ! __clang__
#define NEW_DEL_NOEXCEPT noexcept
#endif // ! __clang__

//
#ifdef __cpp_aligned_new
namespace my
{
	enum class align_val_t : size_t
	{
	};
}
#endif // __cpp_aligned_new
void* __CRTDECL operator new(size_t count) NEW_DEL_NOEXCEPT
{
	return calloc(1, count);
}
#ifdef __cpp_aligned_new
void* __CRTDECL operator new(size_t count,
	my::align_val_t al) NEW_DEL_NOEXCEPT
{
	return calloc(1, count);
}
#endif
void __CRTDECL operator delete(void* ptr)NEW_DEL_NOEXCEPT
{
	free(ptr);
}
#ifdef __cpp_aligned_new
void __CRTDECL operator delete(void* ptr, my::align_val_t al)NEW_DEL_NOEXCEPT
{
	free(ptr);
}
#endif

#endif // DBJ_NEW_DEL_REPLACEMENTS

//
// user code start points
// user code needs to provide these two implementations
extern "C" int bench_program(int argc, char** argv);
extern "C" int test_program(int argc, char** argv);
//
/*
Although dbj-fwk works "always". With or without
cpp exceptions. SEH is intrinsic to Windows.
*/
extern "C" inline int dbj_fwk_main(int argc, char** argv)
{

#if _CPPUNWIND
	try
	{
#endif // _CPPUNWIND

		// logic is this
		// if DBJ_CL_ARG_IGNORE_UBENCH argument is NOT given result is proceed
		// if it is given callback provided will return stop and flow will pass around
		if (dbj::app_args_result::proceed == dbj::app_args_callback_(DBJ_CL_ARG_IGNORE_UBENCH, dbj::app_args_stop))
		{
			DBJ_INFO(" ");
			DBJ_INFO("================================================================");
			DBJ_INFO("BENCH MARKS");
			DBJ_INFO("================================================================");
			DBJ_INFO(" ");

			(void)bench_program(argc, argv);
		}

		if (dbj::app_args_result::proceed == dbj::app_args_callback_(DBJ_CL_ARG_IGNORE_UTEST, dbj::app_args_stop))
		{
			DBJ_INFO(" ");
			DBJ_INFO("================================================================");
			DBJ_INFO("TESTS");
			DBJ_INFO("================================================================");
			DBJ_INFO(" ");

			(void)test_program(argc, argv);
		}

#if _CPPUNWIND
	}
	catch (...)
	{
		// user defined measure here
#ifndef DBJ_FWK_ALL_CATCH
		DBJ_ERROR("Unknown Exception!");
#endif // DBJ_FWK_ALL_CATCH
	}
#endif // _CPPUNWIND

	return 0;
} // main
