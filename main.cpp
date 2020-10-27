// #include "../ubench.h/ubench.h"
#include "dbj_main.h"
/// --------------------------------------------------------------------------------
#include <winnt.h>

//typedef struct _OSVERSIONINFOA {
//    DWORD dwOSVersionInfoSize;
//    DWORD dwMajorVersion;
//    DWORD dwMinorVersion;
//    DWORD dwBuildNumber;
//    DWORD dwPlatformId;
//    CHAR   szCSDVersion[128];     // Maintenance string for PSS usage
//} OSVERSIONINFOA, * POSVERSIONINFOA, * LPOSVERSIONINFOA;
extern "C" {
    typedef struct osinfo_struct {
        long dwOSVersionInfoSize;
        long dwMajorVersion;
        long dwMinorVersion;
        long dwBuildNumber;
        long dwPlatformId;
        char szCSDVersion[128];     // Maintenance string for PSS usage
    } osinfo, * osinfo_ptr;

    osinfo get_os_version(void);
} // "C"

/// --------------------------------------------------------------------------------
extern "C" int main (int argc, char ** argv)
{
#if defined(_WIN32_WINNT_WIN10) && (_WIN32_WINNT >= _WIN32_WINNT_WIN10)		
        // and ... this is it ... really
        // ps: make sure it is not empty string!
        system(" "); 
#else
 // no op
#endif
      __try 
    { 
        __try {

               auto os_ver_struct = get_os_version();

                dbj_main(argc,argv);
        }
        __finally {
#ifdef _DEBUG
            DBJ_INFO(  ":  __finally block visited");
#endif // _DEBUG
        }
    }
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
        // MS STL "throws" are raised SE's under /kernel builds
        // caught here and nowhere else in the app
         DBJ_ERROR(  ": Structured Exception caught");
        
        DBJ_WARN(  ": %s", 
( dump_last_run.finished_ok == TRUE ? "minidump creation succeeded" : "minidump creation failed" ) 
        );

        if ( dump_last_run.finished_ok ) {
            DBJ_INFO(  ": Dump folder: %s", dump_last_run.dump_folder_name );
            DBJ_INFO(  ": Dump file  : %s", dump_last_run.dump_file_name);
        }
    }

#ifdef _DEBUG

#ifdef _KERNEL_MODE
DBJ_INFO(  ": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
DBJ_INFO(  ": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE


#if _HAS_EXCEPTIONS
DBJ_INFO(  ": _HAS_EXCEPTIONS == 1");
#else
DBJ_INFO(  ": _HAS_EXCEPTIONS == 0");
#endif // _HAS_EXCEPTIONS

#if _CPPRTTI 
DBJ_INFO(  ": _CPPRTTI == 1");
#else
DBJ_INFO(  ": _CPPRTTI == 0");
#endif // ! _CPPRTTI

#if _CPPUNWIND 
DBJ_INFO(  ": _CPPUNWIND == 1");
#else // ! _CPPUNWIND 
DBJ_INFO(  ": _CPPUNWIND == 0");
#endif //! _CPPUNWIND 

        DBJ_INFO( ": " DBJ_APP_NAME " " DBJ_APP_VERSION );
        DBJ_INFO(  " ...Leaving... ");
#endif // _DEBUG

return EXIT_SUCCESS ;

}
/// --------------------------------------------------------------------------------
extern "C" {

    // actually the only sure-fire way to
    // obtain windows version

    typedef LONG NTSTATUS; // , * PNTSTATUS{};
#define STATUS_SUCCESS (0x00000000)

    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(osinfo_ptr);

    osinfo get_os_version(void)
    {
        HMODULE hMod = ::GetModuleHandleA("ntdll.dll");
        if (hMod) {
            RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
            if (fxPtr != nullptr) {
                osinfo osinfo_var_ = { 0 };
                osinfo_var_.dwOSVersionInfoSize = sizeof(osinfo_var_);
                if (STATUS_SUCCESS == fxPtr(&osinfo_var_)) {
                    return osinfo_var_;
                }
            }
        }
        osinfo osinfo_var_ = { 0 };
        return osinfo_var_;
    }


} // extern "C" {
