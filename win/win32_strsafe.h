#pragma once
/*
 (c) 2020 by dbj@dbj.org -- LICENSE_DBJ
 "normalization" of "crazy camel" strsafe.h function names
*/
#include <strsafe.h>

// https://docs.microsoft.com/en-us/windows/win32/api/strsafe/nf-strsafe-stringcchprintfa
/*

STRSAFEAPI StringCchPrintfA(  STRSAFE_LPSTR  pszDest,  size_t cchDest,  STRSAFE_LPCSTR pszFormat,  ...);

*/
#undef win_sprintfa
#define win_sprintfa StringCchPrintfA

#undef win_sprintfw
#define win_sprintfw StringCchPrintfW


// https://docs.microsoft.com/en-us/windows/win32/api/strsafe/nf-strsafe-stringcchvprintfa
/*

STRSAFEAPI StringCchVPrintfA(
  STRSAFE_LPSTR  pszDest,
  size_t         cchDest,
  STRSAFE_LPCSTR pszFormat,
  va_list        argList
);

*/
#undef win_vsprintfa
#define win_vsprintfa StringCchVPrintfA 

#undef win_vsprintfw
#define win_vsprintfw StringCchVPrintfW

