
/// the mandatory and only initialization of the dbj simplelog
#include "../dbj--simplelog/dbj_simple_log_host.h"

// change in the main() linker will use
int dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_WITH_CONSOLE);


/// ------------------------------------------------------------------------
#include "dbj_main.h"
#include "win/win_cli_args.h"

static bool we_are_on_required_os(void) {
	// if the Windows version is equal to or
	// greater than 10.0.14393 then ENABLE_VIRTUAL_TERMINAL_PROCESSING is
	// supported.
	// That is: cmd is capable of handling VT100 colour codes
	return is_win_ver_or_greater(10, 0, 14393);
}

// command line args understood by DBJ+FWK
static void cli_usage(const char* /*cli_arg_*/) {

	DBJ_INFO(": ");
	DBJ_INFO(": " DBJ_APP_NAME " " DBJ_APP_VERSION);
	DBJ_INFO(": " DBJ_APP_COPYRIGHT);
	DBJ_INFO(": ");
	DBJ_INFO(": " DBJ_APP_NAME " only, Command LIne arguments ");
	DBJ_INFO(": ");
	DBJ_INFO(": " DBJ_CL_ARG_SHOW_BUILD_ENV " -- display the build environment key parameters");
	DBJ_INFO(": " DBJ_CL_ARG_LOG_TEST " -- quick dbj--simpelog test");
	DBJ_INFO(": " DBJ_CL_ARG_IGNORE_UBENCH " -- do not execute benchmarks");
	DBJ_INFO(": " DBJ_CL_ARG_IGNORE_UTEST " -- do not execute tests");
	DBJ_INFO(": " DBJ_CL_ARG_HELP " -- this short help");
	DBJ_INFO(": ");

}

static void display_build_env(const char* /*cli_arg_*/)
{
	DBJ_INFO(":");
#ifdef _DEBUG
	DBJ_INFO(": DEBUG build");
#else
	DBJ_INFO(": RELEASE build");
#endif
	DBJ_INFO(":");

	osinfo_struct os_ = get_win_version();
	DBJ_INFO(": %s: %ld.%ld.%ld [%ld]", "WINDOWS", os_.major, os_.minor, os_.build_num, os_.platform_id);

#ifdef __clang__
	DBJ_INFO(": %s ", __VERSION__);
#endif // __clang__               

#ifdef _MSC_FULL_VER 
	DBJ_INFO(": %s: %d ", "_MSC_FULL_VER", _MSC_FULL_VER);
#endif // _MSC_FULL_VER                

#ifdef __cplusplus  
	DBJ_INFO(": __cplusplus  : %d", __cplusplus);
#else // ! __cplusplus  
	DBJ_INFO(": __cplusplus  is NOT defined");
#endif // !__cplusplus  

#ifdef __STDC_VERSION__ 
	DBJ_INFO(": __STDC_VERSION__ : %d", __STDC_VERSION__);
#else // ! __STDC_VERSION__ 
	DBJ_INFO(": __STDC_VERSION__ is NOT defined");
#endif // !__STDC_VERSION__ 

#ifdef _KERNEL_MODE
	DBJ_INFO(": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
	DBJ_INFO(": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE

#if (_HAS_EXCEPTIONS == 1)
	DBJ_INFO(": _HAS_EXCEPTIONS == 1");
#else
	DBJ_INFO(": _HAS_EXCEPTIONS == 0");
#endif // _HAS_EXCEPTIONS

#if (_CPPRTTI == 1)
	DBJ_INFO(": _CPPRTTI == 1");
#else
	DBJ_INFO(": _CPPRTTI == 0");
#endif // ! _CPPRTTI

#ifdef _CPPUNWIND
	DBJ_INFO(": _CPPUNWIND is defined");
#else // ! _CPPUNWIND 
	DBJ_INFO(": _CPPUNWIND is not defined");
#endif //! _CPPUNWIND 

	DBJ_INFO(":");
	DBJ_INFO(": " DBJ_APP_NAME " " DBJ_APP_VERSION);
	DBJ_INFO(":");
	DBJ_INFO(":...Leaving... ");
	DBJ_INFO(":");

}
/// --------------------------------------------------------------------------------
/// in here we solve the SE catching (if SE is raised) and minidump generation
static int seh_main(int argc, char** argv)
{
	__try
	{
		__try {

			if (!we_are_on_required_os())
			{
				DBJ_ERROR("");
				DBJ_ERROR("" DBJ_APP_NAME " -- ERROR " );
				DBJ_ERROR("");
				DBJ_ERROR("Minimum Windows version required is 10.0.14393");
				DBJ_ERROR(" Exiting ...");
				DBJ_ERROR("");
			}
			else
			if (!app_args_callback_(DBJ_CL_ARG_HELP, cli_usage)) {
				// no FWK cli help was requested
				(void)app_args_callback_(DBJ_CL_ARG_LOG_TEST, dbj_simple_log_test);
				dbj_main(argc, argv);
			}

		} // inner __try
		__finally {
            // if this cli arg is defined use that callback
			(void)app_args_callback_(DBJ_CL_ARG_SHOW_BUILD_ENV, display_build_env);
		} // __finally
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

// #ifdef _UNICODE

extern "C" int wmain(int argc, wchar_t** argv)
{
	 dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_WITH_CONSOLE);

// "switch on" VT100 for WIN10 cmd.exe
// an awfull hack
// and this is it ... really
// ps: make sure it is not empty string!
	system(" ");

	dbj::win::cli_args args_;
	return seh_main(args_.argc, args_.argv);
}

//#else // ! _UNICODE

extern "C" int main(int argc, char** argv)
{
	// "switch on" VT100 for WIN10 cmd.exe
	// an awfull hack
	system(" ");

	return seh_main(argc, argv);
}

// #endif // ! _UNICODE

/// --------------------------------------------------------------------------------
#ifdef _UNICODE
extern "C" int WINAPI  wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_SETUP);

	dbj::win::cli_args args_;
	return seh_main(args_.argc, args_.argv);
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

	dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_SETUP);

	dbj::win::cli_args args_;
	return seh_main(args_.argc, args_.argv);
}
#endif // ! _UNICODE
