#pragma once
/*
(c) 2020 bu dbj@dbj.org 
*/

#ifdef _KERNEL_MODE
#pragma message( __FILE__ " has sense only in the NON KERNEL builds")
#endif // _KERNEL_MODE


namespace dbj {
	/*
	signatures of two callbacks given at compile time
	*/
	using start_stop_fp = void * (*) (void *);

	template < start_stop_fp ctor_f, start_stop_fp dtor_f >
	struct __declspec(novtable) start_stop
	{
		unsigned int& counter()
		{
			static unsigned int counter_ = 0;
			return counter_;
		}

		start_stop()
		{
			const unsigned & ctr = (counter())++;
			if (0 == ctr) {
				void* reserverd_ = ctor_f(0);
				(void)reserverd_; 
				/*TODO: log the result here*/
			}
		}

		~start_stop()
		{
			const unsigned ctr = --(counter());
			if (ctr < 1)
			{
				void* reserverd_ = dtor_f(0);
				(void)reserverd_;
				/*TODO: log the result here*/
			}
		}

	}; // start_stop

#if 0

	// how to use
	namespace {
		static inline void * enter (void*) { return 0; }
		static inline void * leave(void*) { return 0; }
		//	In anonymous namespace hide the auto-initializer instance
		static const dbj::start_stop< enter, leave> dbj_example_autoinit__{};
	} // anonspace
#endif // 0

} // dbj

