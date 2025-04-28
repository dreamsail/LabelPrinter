#include "StrConv.h"
#include <strsafe.h>
namespace ds {
	StrConv::StrConv(){
		this->u8 = (U8*)malloc(sizeof(U8));
		this->u16 = (U16*)malloc(sizeof(U16));
		this->ascii = (Ascii*)malloc(sizeof(Ascii));
	}
	StrConv::~StrConv()
	{
		free(this->u8);
		free(this->u16);
		free(this->ascii);
	}

	StrConv* StrConv::SetAscii(const Ascii* t)
	{
		size_t size = lstrlenA(t);
		this->ascii = (Ascii*)realloc(this->ascii, sizeof(Ascii) * (size + 1));
		lstrcpyA(this->ascii, t);

		//Ascii to utf16
		size = MultiByteToWideChar(CP_ACP, 0, this->ascii, -1, this->u16, 0);
		this->u16 = (U16*)realloc(this->u16, sizeof(U16) * (size + 1));
		MultiByteToWideChar(CP_ACP, 0, this->ascii, -1, this->u16, size);

		//utf16 to utf8
		size = WideCharToMultiByte(CP_UTF8, 0, this->u16, -1, this->u8, 0, 0, 0);
		this->u8 = (U8*)realloc(this->u8, sizeof(U8) * (size + 1));
		WideCharToMultiByte(CP_UTF8, 0, this->u16, -1, this->u8, size, 0, 0);

		return this;
	}
	StrConv* StrConv::SetU8(const U8* t)
	{

		size_t size = lstrlenA(t);
		this->u8 = (U8*)realloc(this->u8, sizeof(U8) * (size + 1));
		lstrcpyA(this->u8, t);


		//utf8 to utf16
		size = MultiByteToWideChar(CP_UTF8, 0, this->u8, -1, this->u16, 0);
		this->u16 = (U16*)realloc(this->u16, sizeof(U16) * (size + 1));
		MultiByteToWideChar(CP_UTF8, 0, this->u8, -1, this->u16, size);

		//utf16 to ascii
		size = WideCharToMultiByte(CP_ACP, 0, this->u16, -1, this->ascii, 0, 0, 0);
		this->ascii = (Ascii*)realloc(this->ascii, sizeof(Ascii) * (size + 1));
		WideCharToMultiByte(CP_ACP, 0, this->u16, -1, this->ascii, size, 0, 0);

		return this;
	}
	StrConv* StrConv::SetU16(const U16* t)
	{
		size_t size = lstrlenW(t);
		this->u16 = (U16*)realloc(this->u8, sizeof(U16) * (size + 1));
		lstrcpyW(this->u16, t);

		//utf16 to ascii
		size = WideCharToMultiByte(CP_ACP, 0, this->u16, -1, this->ascii, 0, 0, 0);
		this->ascii = (Ascii*)realloc(this->ascii, sizeof(Ascii) * (size + 1));
		WideCharToMultiByte(CP_ACP, 0, this->u16, -1, this->ascii, size, 0, 0);

		//utf16 to utf8
		size = WideCharToMultiByte(CP_UTF8, 0, this->u16, -1, this->u8, 0, 0, 0);
		this->u8 = (U8*)realloc(this->u8, sizeof(U8) * (size + 1));
		WideCharToMultiByte(CP_UTF8, 0, this->u16, -1, this->u8, size, 0, 0);

		return this;
	}

}
