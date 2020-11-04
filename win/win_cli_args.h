#pragma once

#include "windows_includer.h"
#include "dbj_start_stop.h"

namespace dbj::win {

    extern "C" {

    struct app_args_ final {
        int argc;
        wchar_t** argv;
    };

     inline __declspec(thread) struct app_args_ app_cli_args;

    } // "C"

    namespace {
        
        extern "C" {

        static inline void* harvest_app_args_ (void*) noexcept { 
            app_cli_args.argv =
                ::CommandLineToArgvW(
                    ::GetCommandLineW(), &app_cli_args.argc
                );
            return 0; 
        }
        static inline void* free_app_args_(void*) noexcept 
        {
            if (app_cli_args.argc > 0) {
                ::LocalFree(app_cli_args.argv);
                app_cli_args.argc = 0;
            }
            return 0; 
        }

    } // "C"

        //	In anonymous namespace hide the auto-initializer instance
        inline const dbj::start_stop< harvest_app_args_, free_app_args_> 
            dbj_app_cli_harvester_autoinit__{};
    } // anonspace


    // wide char is default
    // C++17 or better is required
    // this is simpler but each instance will make a copy of cli args
    struct cli_wargs final
    {
        int argc{};
        wchar_t** argv{};

        ~cli_wargs() { if ( argv != 0) { ::LocalFree(argv); } }

        // NOTE: rezult of GetCommandLineW() must NOT be free
        // NOTE: rezult of CommandLineToArgvW() must be freed
        cli_wargs() : argv (
            ::CommandLineToArgvW(
                ::GetCommandLineW(), &argc
            )
        ) 
        {
        }
    };

    // http://alter.org.ua/docs/win/args/
    inline PCHAR*
        CommandLineToArgvA(
            PCHAR CmdLine,
            int* _argc
        );

    struct cli_args final
    {
        int argc{};
        char ** argv{};

        ~cli_args() { if ( argv != 0) { ::LocalFree(argv); } }

        // NOTE: rezult of GetCommandLineW() must NOT be free
        // NOTE: rezult of CommandLineToArgvW() must be freed
        cli_args() : argv (
            dbj::win::CommandLineToArgvA(
                ::GetCommandLineA(), &argc
            )
        ) 
        {
        }
    };

    /*
    * for some reason it is not on MS DOCS any more 
    * and not in Win32
    */

    // http://alter.org.ua/docs/win/args/
    inline PCHAR*
        CommandLineToArgvA(
            PCHAR CmdLine,
            int* _argc
        )
    {
        PCHAR* argv;
        PCHAR  _argv;
        ULONG   len;
        ULONG   argc;
        CHAR   a;
        ULONG   i, j;

        BOOLEAN  in_QM;
        BOOLEAN  in_TEXT;
        BOOLEAN  in_SPACE;

        len = strlen(CmdLine);
        i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

        argv = (PCHAR*)LocalAlloc(GMEM_FIXED,
            i + (len + 2) * sizeof(CHAR));

        _argv = (PCHAR)(((PUCHAR)argv) + i);

        argc = 0;
        argv[argc] = _argv;
        in_QM = FALSE;
        in_TEXT = FALSE;
        in_SPACE = TRUE;
        i = 0;
        j = 0;

        while (a = CmdLine[i]) {
            if (in_QM) {
                if (a == '\"') {
                    in_QM = FALSE;
                }
                else {
                    _argv[j] = a;
                    j++;
                }
            }
            else {
                switch (a) {
                case '\"':
                    in_QM = TRUE;
                    in_TEXT = TRUE;
                    if (in_SPACE) {
                        argv[argc] = _argv + j;
                        argc++;
                    }
                    in_SPACE = FALSE;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if (in_TEXT) {
                        _argv[j] = '\0';
                        j++;
                    }
                    in_TEXT = FALSE;
                    in_SPACE = TRUE;
                    break;
                default:
                    in_TEXT = TRUE;
                    if (in_SPACE) {
                        argv[argc] = _argv + j;
                        argc++;
                    }
                    _argv[j] = a;
                    j++;
                    in_SPACE = FALSE;
                    break;
                }
            }
            i++;
        }
        _argv[j] = '\0';
        argv[argc] = NULL;

        (*_argc) = argc;
        return argv;
    }

} // dbj::win
