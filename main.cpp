/* (c) 2019-2021 by dbj.org   -- https://dbj.org/license_dbj/ */

#include "printing_macros.h"
#include "win/win_cli_args.h"
#include "dbj_main.h"
#include "generate_dump.h"

static void  os_check(void) {
	// if the Windows version is equal to or
	// greater than 10.0.14393 then ENABLE_VIRTUAL_TERMINAL_PROCESSING is
	// supported.
	// That is: cmd is capable of handling VT100 colour codes
	if (false == is_win_ver_or_greater(10, 0, 14393))
	{
		puts("");
		puts("" DBJ_APP_NAME " -- ERROR ");
		puts("");
		puts("Minimum Windows version required is 10.0.14393");
		perror(" Exiting ...");

		__fastfail(FAST_FAIL_FATAL_APP_EXIT);
	}
}

// command line args understood by DBJ+FWK
static dbj::app_args_result cli_usage(const char* /*cli_arg_*/ = nullptr) noexcept {

	DBJ_INFO(": ");
	DBJ_INFO(": " DBJ_APP_NAME " " DBJ_APP_VERSION);
	DBJ_INFO(": " DBJ_APP_COPYRIGHT);
	DBJ_INFO(": ");
	DBJ_INFO(": " DBJ_APP_NAME ", Host Command LIne arguments ");
	DBJ_INFO(": If these are given, they are processed and " DBJ_APP_NAME " exits");
	DBJ_INFO(": ");
	DBJ_INFO(": " DBJ_CL_ARG_SHOW_BUILD_ENV " -- display the build environment key parameters");
	DBJ_INFO(": " DBJ_CL_ARG_LOG_TEST " -- quick dbj--simpelog test");
	DBJ_INFO(": " DBJ_CL_ARG_IGNORE_UBENCH " -- do not execute benchmarks");
	DBJ_INFO(": " DBJ_CL_ARG_IGNORE_UTEST " -- do not execute tests");
	DBJ_INFO(": " DBJ_CL_ARG_HELP " -- this short help");
	DBJ_INFO(": ");

	//  display the help and stop
	return dbj::app_args_result::stop;
}

static dbj::app_args_result display_build_env(const char* /*cli_arg_*/)
{
	DBJ_INFO(":");

	osinfo_struct os_ = get_win_version();
	DBJ_INFO(": %s: %ld.%ld.%ld [%ld]", "WINDOWS", os_.major, os_.minor, os_.build_num, os_.platform_id);

#ifdef _DEBUG
	DBJ_INFO(": DEBUG build");
#else
	DBJ_INFO(": RELEASE build");
#endif
	DBJ_INFO(":");
#ifdef __clang__
	DBJ_INFO(": %s ", __VERSION__);
#endif // __clang__               

	// _MSC_FULL_VER always exists
	DBJ_INFO(": %s: %d ", "_MSC_FULL_VER", _MSC_FULL_VER);

	DBJ_INFO(": __cplusplus  : %ld", __cplusplus);

#ifdef __STDC_VERSION__ 
	DBJ_INFO(": __STDC_VERSION__ : %d", __STDC_VERSION__);
#else // ! __STDC_VERSION__ 
	DBJ_INFO(": __STDC_VERSION__ is NOT defined");
#endif // !__STDC_VERSION__ 

	DBJ_INFO(":");

#ifdef _KERNEL_MODE
	DBJ_INFO(": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
	DBJ_INFO(": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE

	// _HAS_EXCEPTIONS always exists and is 0 or 1
	DBJ_INFO(": _HAS_EXCEPTIONS  == %d ", (_HAS_EXCEPTIONS == 1));

	// _CPPRTTI doe not always exists and is 0 or 1
#ifdef _CPPRTTI
	DBJ_INFO(": _CPPRTTI  == %d ", (_CPPRTTI == 1));
#else
	DBJ_INFO(": _CPPRTTI  is not defined ");
#endif 

	// _CPPUNWIND Defined as 1 if one or more of the / GX(Enable Exception Handling), 
	//  clr(Common Language Runtime Compilation), or /EH(Exception Handling Model) 
	//	compiler options are set.
	//	Otherwise, undefined.
#ifdef _CPPUNWIND
	DBJ_INFO(": _CPPUNWIND == %d", (_CPPUNWIND == 1));
#else
	DBJ_INFO(": _CPPUNWIND is undefined");
#endif

	DBJ_INFO(":");
	DBJ_INFO(": " DBJ_APP_NAME " " DBJ_APP_VERSION);
	DBJ_INFO(":");
	DBJ_INFO(":...Leaving... ");
	DBJ_INFO(":");

	return dbj::app_args_result::stop;

}
/// --------------------------------------------------------------------------------
static dbj::app_args_result call_simple_log_test(const char*)
{
#if defined(DBJ_FWK_USES_SIMPLELOG)
	dbj_simple_log_test("");
#else 
	DBJ_INFO(" ");
	DBJ_INFO("BEGIN Internal Test");
	DBJ_INFO(" ");
	DBJ_TRACE("Log  TRACE");
	DBJ_DEBUG("Log  DEBUG");
	DBJ_INFO("Log  INFO");
	DBJ_WARN("Log  WARN");
	DBJ_ERROR("Log  ERROR");
	DBJ_FATAL("Log  FATAL");
	DBJ_INFO(" ");
	DBJ_INFO("END Internal Test");
	DBJ_INFO(" ");
#endif // ! DBJ_FWK_USES_SIMPLELOG
	return dbj::app_args_result::stop;
}
/// --------------------------------------------------------------------------------
/// in here we solve the SE catching (if SE is raised) and minidump generation
static int seh_main(kind_of_app where_am_i_)
{				//  exit if bellow WIN 10.0.14393 
	os_check();
	// int argc; char** argv ;
	// technicaly argc,argv are not necessary as 
	// dbj::app_cli_args are already made at this point
					// made before main
	// or are they?
	DBJ_VERIFY(dbj::app_cli_args.argc > 0);
	//char app_full_path[1024] = { 0 };
	//// Q: is __argv available for windows desktop apps?
	//// A: no it is not
	//// win32 required here
	//int rez = GetModuleFileNameA(
	//	NULL, app_full_path, 1024
	//);
	//DBJ_ASSERT(rez != 0);

	__try
	{
		// setup the log v.s where are we 
		switch (where_am_i_) {
		case kind_of_app::console:
		case kind_of_app::unicode_console:
		{
			int rez = dbj_simple_log_startup(DBJ_LOG_DEFAULT_WITH_CONSOLE, dbj::app_cli_args.argv[0]);
			DBJ_ASSERT(EXIT_SUCCESS == rez);
			// "switch on" VT100 for WIN10 cmd.exe
			// an awfull hack, and really works
			// ps: make sure it is not empty string!
			system(" ");
		}
			break;
		case kind_of_app::windows:
		case kind_of_app::unicode_windows:
		{
#ifndef DBJ_FWK_USES_SIMPLELOG
#error DBJ_FWK_USES_SIMPLELOG must be used for building dbj_fwk Windows applications
#endif

			int rez = dbj_simple_log_startup(DBJ_LOG_DEFAULT_SETUP, dbj::app_cli_args.argv[0]);
			DBJ_ASSERT(EXIT_SUCCESS == rez);
		}
			break;
		default:
		{
			DBJ_INFO(" ");
			DBJ_ERROR("================================================================");
			DBJ_ERROR("Impossible kind of application?");
			DBJ_ERROR("================================================================");
			DBJ_INFO(" ");
			__leave;
		}
			break;
		} // switch


		// no args given?
		//if (dbj::app_cli_args.argc < 2) {
		//	cli_usage();
		//	__leave;
		//}
		// FWK cli help was requested
		if (dbj::app_args_result::stop == app_args_callback_(DBJ_CL_ARG_HELP, cli_usage)) __leave;
		// FWK log test was requested
		if (dbj::app_args_result::stop == app_args_callback_(DBJ_CL_ARG_LOG_TEST, call_simple_log_test)) __leave;
		// if this cli arg is defined use that callback
		if (dbj::app_args_result::stop == app_args_callback_(DBJ_CL_ARG_SHOW_BUILD_ENV, display_build_env)) __leave;

		dbj_fwk_main(dbj::app_cli_args.argc, dbj::app_cli_args.argv);

		DBJ_INFO(" ");
		DBJ_INFO("================================================================");
		DBJ_INFO("%s %s DONE", DBJ_APP_NAME, DBJ_APP_VERSION);
		DBJ_INFO("================================================================");
		DBJ_INFO(" ");

		// } __finally {	} // dbj: too fiddly when __leave is used: inner __try + __finally
	} // outer __try
	__except (
		GenerateDump(GetExceptionInformation())
		/* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
		)
	{
		// MS STL "throws" are raised SE's under "no exceptions" builds
		// they are best caught here and nowhere else in the app
		DBJ_ERROR(": Structured Exception caught");

		if (dump_last_run.finished_ok) {
			DBJ_INFO(": Minidump creation succeeded");
			DBJ_INFO(": Dump folder: %s", dump_last_run.dump_folder_name);
			DBJ_INFO(": Dump file  : %s", dump_last_run.dump_file_name);
		}
		else {
			DBJ_WARN("Minidump creation failed");
		}
	} // __except

	return EXIT_SUCCESS;

} // seh_main

/// --------------------------------------------------------------------------------
/*
WCHAR policy : ignore it :)

all the main() versions bellow capture argv
as char **
and send it into the FWK

end user is providing implementation for:
extern "C" int program (int argc, char ** argv) ;

as far as user is concetned, that is the entry point
*/

#if defined( __clang__ ) && (defined( UNICODE ) || defined( _UNICODE ))

extern "C" int wmain(int argc, wchar_t** argv)
{
	return seh_main(kind_of_app::unicode_console);
}

#else // ! clang and _UNICODE

extern "C" int main(int argc, char** argv)
{
	return seh_main(kind_of_app::console);
}

#endif // ! clang and _UNICODE

/// --------------------------------------------------------------------------------
#ifdef _UNICODE
extern "C" int WINAPI  wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	return seh_main(kind_of_app::unicode_windows);
}
/// --------------------------------------------------------------------------------
#else // ! _UNICODE
extern "C" int WINAPI  WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	return seh_main(kind_of_app::windows);
}
#endif // ! _UNICODE


