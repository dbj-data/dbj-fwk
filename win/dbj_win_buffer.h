#pragma once

#ifndef DBJ_WIN_BUFFER_INC
#define DBJ_WIN_BUFFER_INC

/*
-----------------------------------------------------------------------------------------
 (c) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj/

 NOTE: this is C++ ... but with C++ things "mellowed down" to be C things wherver possible
 for "no prisoners taken" C++17 version see 

 https://github.com/dbj-systems/dbj--nanolib

 This use MS STL. And that works in /kernel builds. But! It uses SEH in that case.
-----------------------------------------------------------------------------------------
*/

#define DBJ_USES_STD_LIB
#include "windows_includer.h"
#include "win32_strsafe.h" // win safe sprintf familly
/*
 This use MS STL. And that works in /kernel builds. But! It uses SEH in that case.
 In any case we hash define key things from std lib. In future we might use some 
 other implementation(s)
*/
#include <vector>
#undef DBJ_VECTOR
#define DBJ_VECTOR std::vector

#include <array>
#undef DBJ_ARRAY
#define DBJ_ARRAY std::array

#ifdef DBJ_USES_STRING_VIEW
#include <string_view>
#endif

#ifdef __cpp_char8_t
// C++20 or better
#pragma message "WARNING: no support for char8_t"
#endif // __cpp_char8_t

#ifndef CP_UTF8
#define CP_UTF8   65001 // UTF-8 translation, from winnls.h
#endif 

#pragma region win buffer type and helper

namespace dbj{ namespace win {

	/*
	in case you need more change this
	by default it is 64KB aka 65535 bytes, which is quite a lot perhaps?
	*/
	typedef enum { DBJ_MAX_BUFER_SIZE = UINT16_MAX } dbj_win_buff_enums ;
	/*
	for runtime buffering the most comfortable and in the same time fast
	solution is vector<char_type>
	only unique_ptr<char[]> is faster than vector of  chars, by a margin
	UNICODE does not mean 'char' is forbiden. We deliver 'char' based buffering
	only.
	Bellow is a helper, with function most frequently used to make buffer aka vector<char>
	*/
	template<typename CHAR>
	struct buffer final
	{
		using type = buffer;

		using char_type = CHAR;

		using buffer_type = DBJ_VECTOR <char_type>;

		/*
		there is a lot of vector moving/copying in here
		*/
		static buffer_type make(size_t count_)
		{
			DBJ_ASSERT(count_ < DBJ_MAX_BUFER_SIZE);
			return buffer_type((buffer_type::size_type)count_ /*+ 1*/, char_type(0));
		}
#ifdef DBJ_USES_STRING_VIEW
		static buffer_type make(std::basic_string_view<char_type> sview_)
		{
			DBJ_ASSERT(sview_.size() > 0);
			DBJ_ASSERT(DBJ_MAX_BUFER_SIZE > sview_.size());
			buffer_type retval_(sview_.data(), sview_.data() + sview_.size());
			// zero terminate?
			retval_.push_back(char(0));
			return retval_;
		}
#endif // DBJ_USES_STRING_VIEW

		static buffer_type make(char_type sview_ [DBJ_MAX_BUFER_SIZE] )
		{
			// do we need strlen?
			size_t size = DBJ_COUNT_OF(sview_);
			DBJ_ASSERT( size > 0);
			DBJ_ASSERT(DBJ_MAX_BUFER_SIZE > size );
			buffer_type retval_(sview_ , sview_ + size );
			// zero terminate?
			retval_.push_back(char(0));
			return retval_;
		}

		// char format
		template <typename... Args, size_t max_arguments = 255>
		static buffer_type
			format(char const* format_, Args... args) noexcept
		{
			static_assert(sizeof...(args) < max_arguments, "\n\nmax 255 arguments allowed\n");
			DBJ_ASSERT(format_);
			// 1: what is the size required
			size_t size = 1 + size_t(
				_snprintf_s(nullptr, 0, DBJ_MAX_BUFER_SIZE, format_, args...));
			DBJ_ASSERT(size > 0);
			// 2: use it at runtime
			buffer_type buf = make(size);
			//
			size = _snprintf_s(buf.data(), size, DBJ_MAX_BUFER_SIZE, format_, args...);
			DBJ_ASSERT(size > 0);

			return buf;
		}
		// wchar_t format
		template <typename... Args, size_t max_arguments = 255>
		static buffer_type
			format(wchar_t const* format_, Args... args) noexcept
		{
			static_assert(sizeof...(args) < max_arguments, "\n\nmax 255 arguments allowed\n");
			DBJ_ASSERT(format_);
			// 1: what is the size required
			size_t size = 1 + size_t(
				_snwprintf_s(nullptr, 0, DBJ_MAX_BUFER_SIZE, format_, args...));
			DBJ_ASSERT(size > 0);
			// 2: use it at runtime
			buffer_type buf = make(size);
			//
			size = _snwprintf_s(buf.data(), size, DBJ_MAX_BUFER_SIZE, format_, args...);
			DBJ_ASSERT(size > 0);

			return buf;
		}

		// replace char with another char
		static buffer_type replace(DBJ_VECTOR<char>  buff_, char find, char replace)
		{
			char* str = buff_.data();
			while (true)
			{
				if (char* current_pos = strchr(str, find); current_pos)
				{
					*current_pos = replace;
					// shorten next search
					str = current_pos;
				}
				else
				{
					break;
				}
			}
			return buff_;
		}
		// replace wchar with another wchar
		static buffer_type replace(DBJ_VECTOR<wchar_t> buff_, wchar_t find, wchar_t replace)
		{
			wchar_t* str = buff_.data();
			while (true)
			{
				if (wchar_t* current_pos = wcschr(str, find); current_pos)
				{
					*current_pos = replace;
					// shorten next search
					str = current_pos;
				}
				else
				{
					break;
				}
			}
			return buff_;
		}

		/*
		CP_ACP == ANSI
		CP_UTF8
		*/
		template <auto CODE_PAGE_T_P_ = CP_UTF8>
		static DBJ_VECTOR<wchar_t> n2w(std::string_view s)
		{
			const int slength = (int)s.size() + 1;
			int len = MultiByteToWideChar(CODE_PAGE_T_P_, 0, s.data(), slength, 0, 0);
			DBJ_VECTOR<wchar_t> rez(len, L'\0');
			MultiByteToWideChar(CODE_PAGE_T_P_, 0, s.data(), slength, rez.data(), len);
			return rez;
		}

		template <auto CODE_PAGE_T_P_ = CP_UTF8>
		static DBJ_VECTOR<char> w2n(std::wstring_view s)
		{
			const int slength = (int)s.size() + 1;
			int len = WideCharToMultiByte(CODE_PAGE_T_P_, 0, s.data(), slength, 0, 0, 0, 0);
			DBJ_VECTOR<char> rez(len, '\0');
			WideCharToMultiByte(CODE_PAGE_T_P_, 0, s.data(), slength, rez.data(), len, 0, 0);
			return rez;
		}
	};	 // buffer

	/*
	make compile time string literal buffer into array container. 
	
	Simple compile time usage:

	constexpr auto arr_ = make_arr_buffer("string literal") ;
	*/
	template<typename T, size_t N >
	constexpr inline auto
		make_arr_buffer(const T(&string_)[N]) noexcept
	{
		static_assert(N > 1);

		DBJ_ARRAY<T, N> buffy_{};
		size_t k{};
		for (auto CH : string_)
			buffy_[k++] = CH;
		return buffy_;
	}

} // namespace win 
} // namespace dbj

#pragma endregion

#endif // DBJ_WIN_BUFFER_INC
