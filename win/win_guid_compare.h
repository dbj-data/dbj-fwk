#pragma once
/*
(c) 2020 by dbj@dbj.org -- LICENSE_DBJ

https://stackoverflow.com/a/64065120/10870835

NOTE: "self sorting" STL Containers need an "less than" operator on things to be stored in them. 
Looking into the code bellow one can imagine such a "thing" being done for GUID's. Very unnatural but possible.

( std::priority_queue or std::multiset, etc)

You have (large) set of non-trivial structures stored persistently. You need to "put" into self sorting container for some processing.
You will have handlers in your run time not the full instances taken in and out of the storage.

And GUID is an ideal handle.

You will read just handles (aka GUIDS's) and use them all the way to the moment you actually 
have to look into the structure. To be able to use these GUID's you need to have an "less than" operator. 

Still not logicaly very sensical because GUID is an highly random value. But someone might need 

     bool less_than_guid( const GUID& rguid1, const GUID& rguid2  );

NOTE: there is UuidCompare function (rpcdce.h) but it is not compile time C++. Neither it is OS agnostic.
*/

#include <guiddef.h>

namespace dbj {
	namespace win {
		constexpr inline bool
			equal_guids (const GUID& rguid1, const GUID& rguid2)
			noexcept
		{
			return
				rguid1.Data1 == rguid2.Data1 &&
				rguid1.Data2 == rguid2.Data2 &&
				rguid1.Data3 == rguid2.Data3 &&
				rguid1.Data4[0] == rguid2.Data4[0] &&
				rguid1.Data4[1] == rguid2.Data4[1] &&
				rguid1.Data4[2] == rguid2.Data4[2] &&
				rguid1.Data4[3] == rguid2.Data4[3] &&
				rguid1.Data4[4] == rguid2.Data4[4] &&
				rguid1.Data4[5] == rguid2.Data4[5] &&
				rguid1.Data4[6] == rguid2.Data4[6] &&
				rguid1.Data4[7] == rguid2.Data4[7];
		}

		constexpr inline bool operator == (const GUID & guidOne, const GUID & guidOther)
		{
			return equal_guids(guidOne, guidOther);
		}
	} // namespace win 
} // namespace dbj 
