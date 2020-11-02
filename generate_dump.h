#pragma once

#include "meta.h"

/*

Taken from: https://docs.microsoft.com/en-us/windows/win32/dxtecharts/crash-dump-analysis

Extension (c) 2020 by dbj.org -- LICENSE_DBJ

NOTE: SEH routines are intrinsic to the cl.exe
Usage: 

inline void SomeFunction()
{
    __try
    {
        int *pBadPtr = NULL;
        *pBadPtr = 0;
    }
    __except(GenerateDump(GetExceptionInformation()))
    {
    }
}
*/

#include <windows.h>

#include <assert.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <strsafe.h>

//    StringCchPrintfA( szFileName, MAX_PATH, "%s%s", szPath, szAppName );
#undef  win32_sprintf
#define win32_sprintf StringCchPrintfA

#pragma comment(lib, "Dbghelp.lib")

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define GENERATE_DUMP_MAX_PATH 1024

typedef struct generate_dump_last_run {
    char  dump_folder_name[GENERATE_DUMP_MAX_PATH] ; 
    char  dump_file_name[GENERATE_DUMP_MAX_PATH] ; 
    BOOL finished_ok ;
} generate_dump_last_run;

static generate_dump_last_run dump_last_run ;
/*
DBJ: warning, currently this is a blocking call to an expensive function
*/
inline int GenerateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
static generate_dump_last_run dump_last_run_clean = { {0}, {0}, FALSE } ;

    dump_last_run = dump_last_run_clean  ;

    char  szPath[GENERATE_DUMP_MAX_PATH] = {0}; 
    char  szFileName[GENERATE_DUMP_MAX_PATH] = {0}; 
    const char * szAppName = DBJ_APP_NAME;
    const char * szVersion = DBJ_APP_VERSION;
    DWORD dwBufferSize = GENERATE_DUMP_MAX_PATH;
    SYSTEMTIME stLocalTime;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;
     HANDLE hDumpFile = NULL ;
     BOOL mini_dump_rezult = FALSE ;

    GetLocalTime( &stLocalTime );
    GetTempPathA( dwBufferSize, szPath );

    win32_sprintf( szFileName, MAX_PATH, "%s%s", szPath, szAppName );
    /*BOOL isDirCreated = */CreateDirectoryA( szFileName, NULL );

    win32_sprintf( dump_last_run.dump_folder_name,  MAX_PATH, "%s", szFileName  );

    win32_sprintf( szFileName, MAX_PATH, "%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
               szPath, szAppName, szVersion, 
               stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
               stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
               GetCurrentProcessId(), GetCurrentThreadId());

    hDumpFile = CreateFileA(szFileName, GENERIC_READ|GENERIC_WRITE, 
                FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

    if (INVALID_HANDLE_VALUE == hDumpFile)
    {
    fprintf( stderr, "CreateFileA(%s) has failed with error code: %lu", szFileName, GetLastError());
    goto exit;
    }                

    win32_sprintf( dump_last_run.dump_file_name ,  MAX_PATH, "%s", szFileName  );

    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = pExceptionPointers;
    ExpParam.ClientPointers = TRUE;

    mini_dump_rezult = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
                    hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);
#ifdef _DEBUG
    if (mini_dump_rezult == 0)
    {
    fprintf( stderr, "MiniDumpWriteDump() has failed with error code: %lu", GetLastError());
    }
#endif

    dump_last_run.finished_ok = mini_dump_rezult;
exit:
    return EXCEPTION_EXECUTE_HANDLER;
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

