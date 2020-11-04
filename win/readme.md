
# dbj++sharedwin&trade;

&copy; 2020 by dbj@dbj.org

>Licence: LICENSE_DBJ -- https://dbj.org/license_dbj

WIn32 depending/covering parts are here. This is presumably C code as WIN32 is an C API. As it is "undefined" by CL we do not know which C. Some subset of C99.

This is "not a lib", unless there are *.c files, which if found in here you need to include in your builds too. Why do we use *.c files? Well, mainly because that saves us from a pollution of #including <windows.h> everywhere. Which might be futile.