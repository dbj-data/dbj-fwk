#pragma once

#include "windows_includer.h"
#include "dbj_start_stop.h"

#ifdef __cplusplus   
extern "C" {
#endif 
	/*
	*  http://alter.org.ua/docs/win/args/
	*
	* for some reason it is not on MS DOCS any more
	* and not in Win32 any more
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

		len = (ULONG)strlen(CmdLine);
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

		while ((a = CmdLine[i])) {
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

	//---------------------------------------------------------------------
	//
	struct app_args_ final {
		int argc;
		char** argv;
	};

	static __declspec(thread) struct app_args_ app_cli_args = { 0,0 };

#ifdef __clang__

	__attribute__((constructor))
		static inline void harvest_app_args_(void)
	{
		if (app_cli_args.argc < 1) {
			app_cli_args.argv =
				CommandLineToArgvA(
					GetCommandLineA(), &app_cli_args.argc
				);
		}
	}

	// CAUTION: it seems clang-cl can do this on Windows
	// only if linking with static runtime lib
	__attribute__((destructor))
		static inline void free_app_args_(void)
	{
		if (app_cli_args.argc > 0) {
			LocalFree(app_cli_args.argv);
			app_cli_args.argc = 0;
		}
	}

#endif // __clang__

	/*
	if arg found if it is given call a callback on it, return true
	otherwise return false
	*/
	inline bool app_args_callback_ ( const char arg_name[], void (*callb_)(const char*))
	{
		// DBJ TODO: MT lock here
		_ASSERTE( app_cli_args.argc > 0 );

		size_t arg_name_len = strnlen_s(arg_name, 0xFF);
		_ASSERTE(arg_name_len);

		for (int index = 0; index < app_cli_args.argc; ++index)
		{
			if (0 == strncmp(app_cli_args.argv[index], arg_name, arg_name_len)) {

				if (callb_) callb_(arg_name);
				return true;
			}
		}
		return false;
	}


#ifdef __cplusplus   
} // "C"
#endif // __cplusplus   

#ifdef __cplusplus 
namespace dbj {
	namespace win {

		//---------------------------------------------------------------------
		// this is simpler but each instance will make a copy of cli args
		struct cli_args final
		{
			int argc{};
			char** argv{};

			~cli_args() noexcept {
				if (argv != 0) {
					::LocalFree(argv);
					argc = 0;
					argv = nullptr;
					if (app_cli_args.argc < 1) {
						// populate the C struct above 
						app_cli_args.argc = 0;
						app_cli_args.argv = nullptr;
					}
				}
			}

			// NOTE: rezult of GetCommandLineA() must NOT be freed
			// NOTE: rezult of CommandLineToArgvA() must be freed
			cli_args() noexcept : argv(
				CommandLineToArgvA(
					::GetCommandLineA(), &argc
				)
			)
			{
				if (app_cli_args.argc < 1) {
					// populate the C struct above 
					app_cli_args.argc = this->argc;
					app_cli_args.argv = this->argv;
				}
			}
			// no copy
			cli_args(cli_args const&) = delete;
			cli_args& operator = (cli_args const&) = delete;
			// no move
			cli_args(cli_args&&) = delete;
			cli_args& operator = (cli_args&&) = delete;

		}; // cli_args

		inline cli_args command_line_{};
	}
} // dbj::win

#endif // __cplusplus 
