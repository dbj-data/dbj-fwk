// dbj-fwk-lib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "../meta.h"

// TODO: This is an example of a library function
extern "C" const char * const DBJFWKLIB()
{
return 
( DBJ_APP_COPYRIGHT ", " DBJ_APP_NAME 
	"[" DBJ_APP_VERSION "] compiled by " DBJ_APP_OWNER  
	" on " __DATE__ " at " __TIME__  );
}

#ifdef __clang__
/*

clang + static lib
Link Time Code Generation aka /LTCG 
in release builds is a problem
 
https://developercommunity.visualstudio.com/content/problem/1110835/clangcl-ltcg-is-passed-to-llvm-lib.html
*/
#endif // __clang__
