#pragma once
#ifndef STR_CONV_H_
#define STR_CONV_H_
#include <Windows.h>
#ifdef __cplusplus
extern "C" {
#endif


	namespace ds {
		typedef char Ascii;
		typedef char U8;
		typedef wchar_t U16;

		class StrConv
		{
		public:
			StrConv();
			~StrConv();

			StrConv* SetAscii(const Ascii* t);
			StrConv* SetU8(const U8* t);
			StrConv* SetU16(const U16* t);
			Ascii* ascii;
			U8* u8;
			U16* u16;
		};
	}

#ifdef __cplusplus
}
#endif
#endif //STR_CONV_H_
