#pragma once

#if defined(DBJ_FWK_USES_SIMPLELOG)
// NOTE: host app includes dbj_simple_log_host.h

#define DBJ_SIMPLELOG_USER_DEFINED_MACRO_NAMES
#include "../dbj--simplelog/dbj_simple_log.h"

#define DBJ_TRACE(...) dbj_log_trace(__VA_ARGS__)
#define DBJ_DEBUG(...) dbj_log_debug(__VA_ARGS__)
#define DBJ_INFO(...) dbj_log_info(__VA_ARGS__)
#define DBJ_WARN(...) dbj_log_warn(__VA_ARGS__)
#define DBJ_ERROR(...) dbj_log_error(__VA_ARGS__)
#define DBJ_FATAL(...) dbj_log_fatal(__VA_ARGS__)

#else // ! defined(DBJ_FWK_USES_SIMPLELOG)
// -----------------------------------------------------------------------------

#include "vt100.h"

// redirect stderr to a file if required
// vt100 coluring goes to stdout
#undef  DBJ_PRINT
#define DBJ_PRINT(...) fprintf(stderr, __VA_ARGS__ )

#undef  DBJ_INFO
#define DBJ_INFO(...)  fprintf(stdout, VT100_FG_CYAN_BOLD ); fprintf(stderr, "\n" __VA_ARGS__ ); fprintf(stdout, VT100_RESET ); 

#undef  DBJ_WARN
#define DBJ_WARN(...)  fprintf(stdout, VT100_FG_YELLOW ); fprintf(stderr, "\n"  __VA_ARGS__ ); fprintf(stdout, VT100_RESET ); 

#undef  DBJ_ERROR
#define DBJ_ERROR(...)  fprintf(stdout, VT100_FG_RED_BOLD ); fprintf(stderr, "\n"  __VA_ARGS__ ); fprintf(stdout, VT100_RESET ); 

#endif // ! defined(DBJ_FWK_USES_SIMPLELOG)


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
#define DBJ_DBG(F,X) (X)
// #define SX(F,...)
#endif // ! _DEBUG
