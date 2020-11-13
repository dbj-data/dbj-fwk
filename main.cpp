// #include "../ubench.h/ubench.h"
#include "dbj_main.h"
#include "win/win_cli_args.h"
// above we do #include "win/windows_includer.h"

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
/// in here we solve the SE catching (if SE raised) and minidump generation
static int seh_main(int argc, char** argv)
{
	__try
	{
		__try {

			dbj_main(argc, argv);

		} // inner __try
		__finally {
			DBJ_DBG("%s",":  __finally block visited");
				app_args_callback_(DBJ_CL_ARG_SHOW_BUILD_ENV, display_build_env );
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
	// "switch on" VT100 for WIN10 cmd.exe
	// an awfull hack
#if defined(_WIN32_WINNT_WIN10) 
// and this is it ... really
// ps: make sure it is not empty string!
	system(" ");
#else
 // no op
#endif
	dbj::win::cli_args args_;
	return seh_main(args_.argc, args_.argv);
}

//#else // ! _UNICODE

extern "C" int main(int argc, char** argv)
{
	// "switch on" VT100 for WIN10 cmd.exe
	// an awfull hack
#if defined(_WIN32_WINNT_WIN10) 
// and this is it ... really
// ps: make sure it is not empty string!
	system(" ");
#else
 // no op
#endif
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

	dbj::win::cli_args args_;
	return seh_main(args_.argc, args_.argv);
}
#endif // ! _UNICODE
