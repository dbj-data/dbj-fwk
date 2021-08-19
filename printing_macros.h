#ifndef PRTINTING_MACROS_INCLUDED_
#define PRTINTING_MACROS_INCLUDED_

#ifdef __clang__
#pragma clang system_header
#endif

// DBJ+FWK is stand alone if this is not defined at build time
#if defined(DBJ_FWK_USES_SIMPLELOG)

#define DBJ_SIMPLELOG_USER_DEFINED_MACRO_NAMES

#ifndef __clang__
#include "../simplelog/dbj_simple_log_msvc.h"
#else
#include "../simplelog/dbj_simple_log.h"
#endif // ! __clang__ 

#define DBJ_TRACE(...) dbj_log_trace(__VA_ARGS__)
#define DBJ_DEBUG(...) dbj_log_debug(__VA_ARGS__)
#define DBJ_INFO(...) dbj_log_info(__VA_ARGS__)
#define DBJ_WARN(...) dbj_log_warn(__VA_ARGS__)
#define DBJ_ERROR(...) dbj_log_error(__VA_ARGS__)
#define DBJ_FATAL(...) dbj_log_fatal(__VA_ARGS__)

// dbj_simple_log requires this in one place
// BEFORE it is used
// extern int dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_SETUP);

//#else // ! DBJ_FWK_USES_SIMPLELOG
//int dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_WITH_CONSOLE);
//#endif // DBJ_FWK_USES_SIMPLELOG

#else // ! defined(DBJ_FWK_USES_SIMPLELOG)
// -----------------------------------------------------------------------------
// stand alone console printing macros and colouring
#include "vt100.h"

// redirect stderr to a file if required
// vt100 coluring goes to stdout
#undef  DBJ_PRINT
#define DBJ_PRINT(...) fprintf(stderr, __VA_ARGS__ )

#undef  DBJ_TRACE
#define  DBJ_TRACE DBJ_PRINT

#undef  DBJ_INFO
#define DBJ_INFO(...)  fprintf(stdout, VT100_FG_CYAN_BOLD ); fprintf(stderr, "\n" __VA_ARGS__ ); fprintf(stdout, VT100_RESET ); 

#undef  DBJ_DEBUG
#define  DBJ_DEBUG DBJ_INFO

#undef  DBJ_WARN
#define DBJ_WARN(...)  fprintf(stdout, VT100_FG_YELLOW ); fprintf(stderr, "\n"  __VA_ARGS__ ); fprintf(stdout, VT100_RESET ); 

#undef  DBJ_ERROR
#define DBJ_ERROR(...)  fprintf(stdout, VT100_FG_RED_BOLD ); fprintf(stderr, "\n"  __VA_ARGS__ ); fprintf(stdout, VT100_RESET ); 

#undef  DBJ_FATAL
#define  DBJ_FATAL DBJ_ERROR

#endif // ! defined(DBJ_FWK_USES_SIMPLELOG)

/////////////////////////////////////////////////////////////////////////////////////
// #undef SX
#undef DBJ_DBG
// all four above do stay in the RELEASE builds
// but DBJ_DBG does not 
#ifdef _DEBUG
#define DBJ_DBG(F, ... ) DBJ_INFO("\n%s[%4d] %s : " F, __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__ )
// deprecated -- Show eXpression
// #define SX(F, ...) printf("\n%s[%4d] %12s : " F, __FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)
#else // ! _DEBUG
// just execute the expression do not try to print it
#define DBJ_DBG(F,X) ((void)X)
// #define SX(F,...)
#endif // ! _DEBUG

#endif // PRTINTING_MACROS_INCLUDED_