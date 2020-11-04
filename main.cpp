// #include "../ubench.h/ubench.h"
#include "dbj_main.h"
#include "dbj_win_lib.h"
#include "win/win_cli_args.h"

/// --------------------------------------------------------------------------------
/// in here we solve the SE catching (if SE raised) and minidump generation
extern "C" static int seh_main (int argc, char ** argv)
{

      __try 
    { 
        __try {

    dbj_main(argc,argv);
    }
    __finally {
#ifdef DBJ_FWK_DISPLAY_INFO
    DBJ_INFO(  ":  __finally block visited");
#endif // DBJ_FWK_DISPLAY_INFO
        }
    } // outer __try
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
    // MS STL "throws" are raised SE's under "no exceptions" builds
    // they are best caught here and nowhere else in the app
    DBJ_ERROR(  ": Structured Exception caught");
        
        if ( dump_last_run.finished_ok ) {
            DBJ_INFO(  ": Minidump creation succeeded");
            DBJ_INFO(  ": Dump folder: %s", dump_last_run.dump_folder_name );
            DBJ_INFO(  ": Dump file  : %s", dump_last_run.dump_file_name);
        } else {
            DBJ_WARN(  "Minidump creation failed" );
        }
    }

#ifdef DBJ_FWK_DISPLAY_INFO

DBJ_INFO(  ":");
#ifdef _DEBUG
DBJ_INFO(  ": DEBUG build");
#else
DBJ_INFO(  ": RELEASE build");
#endif
DBJ_INFO(  ":");

    osinfo_struct os_ = get_win_version();
    DBJ_INFO(  "WINDOWS : %ld.%ld.%ld [%ld]\n", os_.major, os_.minor, os_.build_num, os_.platform_id );          

#ifdef __clang__
    DBJ_INFO(": %s ", __VERSION__  );          
#endif // __clang__               


#ifdef _KERNEL_MODE
DBJ_INFO(  ": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
DBJ_INFO(  ": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE


#if (_HAS_EXCEPTIONS == 1)
DBJ_INFO(  ": _HAS_EXCEPTIONS == 1");
#else
DBJ_INFO(  ": _HAS_EXCEPTIONS == 0");
#endif // _HAS_EXCEPTIONS

#if (_CPPRTTI == 1)
DBJ_INFO(  ": _CPPRTTI == 1");
#else
DBJ_INFO(  ": _CPPRTTI == 0");
#endif // ! _CPPRTTI

#if (_CPPUNWIND == 1)
DBJ_INFO(  ": _CPPUNWIND == 1");
#else // ! _CPPUNWIND 
DBJ_INFO(  ": _CPPUNWIND == 0");
#endif //! _CPPUNWIND 

DBJ_INFO( ": " DBJ_APP_NAME " " DBJ_APP_VERSION );
DBJ_INFO(  " ...Leaving... ");
DBJ_INFO(  ":");
#endif // DBJ_FWK_DISPLAY_INFO

return EXIT_SUCCESS ;

} // seh_main

/// --------------------------------------------------------------------------------
/*
WCHAR policy : ignore it :)

all the main version bellow caputre char ** argv
and send it intto the FWK

end user is providing implementation for:
extern "C" int program (int argc, char ** argv) ;

as far as user is concetned, that is the entry point
*/
#ifdef _UNICODE
extern "C" int wmain (int argc, wchar_t ** argv)
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
        int rezult_ = seh_main(args_.argc, args_.argv);
}
#else // ! _UNICODE
extern "C" int main (int argc, char ** argv)
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
    return seh_main( argc, argv ) ;
}
#endif // ! _UNICODE

/// --------------------------------------------------------------------------------
#ifdef _UNICODE
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    dbj::win::cli_args args_;

    int rezult_ = seh_main(args_.argc, args_.argv);

    return rezult_ ;
}
/// --------------------------------------------------------------------------------
#else // ! _UNICODE
int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    dbj::win::cli_args args_ ;

    int rezult_ = seh_main( args_.argc  , args_.argv);

    return rezult_ ;
}
#endif // ! _UNICODE
