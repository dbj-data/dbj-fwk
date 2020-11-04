#pragma once
/*
 2020 note: this is pre C++11 code
*/
/*
Copyright 2006 - 2018 dbj@dbj.org

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/* set to 1 if using com */
#define DBJCOM 1

#if DBJCOM		
#include <combaseapi.h>

namespace dbj {

	namespace win {
		namespace com {
			namespace {
				/*
				In anonymous namespace we hide the auto-initializer
				This ensures that COM is initialized “as soon as possible”
				This mechanism really works. Convince yourself once through the
				debugger, and then just forget about COM init/uninit.
				*/
				struct __declspec(novtable)
					COMAUTOINIT
				{
					unsigned int& counter()
					{
						static unsigned int counter_ = 0;
						return counter_;
					}
					/*
					If you call ::CoInitialize(NULL), after this method is used
					most likely the HRESULT will be :
					hRes = 0×80010106 — Cannot change thread mode after it is set.
					*/
					COMAUTOINIT()
					{
						const UINT& ctr = (counter())++;
						if (0 == ctr)
#if ( defined(_WIN32_DCOM)  || defined(_ATL_FREE_THREADED))
							HRESULT result = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
							HRESULT result = ::CoInitialize(NULL);
#endif
						/*TODO: log the result here*/
					}

					~COMAUTOINIT()
					{
						const UINT ctr = --(counter());
						if (ctr < 1)
							::CoUninitialize();
					}

				};

				// no C++ inline variable used here
				static const COMAUTOINIT dbj_com_autoinit__{};

			} // anonspace
		} // com
	} // win
} // dbj
#endif // DBJCOM
