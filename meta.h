#pragma once
/*
no includes in here please

CAUTION! DBJ_APP_NAME is used as folder name at one point
, do not use chars that must not be used in file/folder names

*/
#define DBJ_APP_NAME "DBJ+FWK"
#define DBJ_APP_VERSION "1.0.0"
#define DBJ_APP_OWNER "dbj@dbj.org"
#define DBJ_APP_LICENSE "LICENSE_DBJ"
#define DBJ_APP_COPYRIGHT "(c) 2020 by dbj@dbj.org"

#pragma comment( user, DBJ_APP_COPYRIGHT ", " DBJ_APP_NAME "[" DBJ_APP_VERSION "] compiled by " DBJ_APP_OWNER  " on " __DATE__ " at " __TIME__ )

// command line args understood by DBJ+FWK
#define DBJ_CL_ARG_SHOW_BUILD_ENV  "--fwk-buildenv"
#define DBJ_CL_ARG_LOG_TEST  "--log-test"
#define DBJ_CL_ARG_IGNORE_UBENCH  "--fwk-ignore-ubench"
#define DBJ_CL_ARG_IGNORE_UTEST  "--fwk-ignore-utest"
#define DBJ_CL_ARG_HELP  "--fwk-cli"

// EOF