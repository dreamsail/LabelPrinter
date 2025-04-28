#pragma once
#ifndef DS_DLL_h
#define DS_DLL_h
#ifdef _WINDLL 
#define DLL_API __declspec(dllexport)
#else 
#ifdef _DLL
#define DLL_API __declspec(dllimport)
#else
#define DLL_API
#endif  //_DLL
#endif //_WINDLL
#endif // ! DS_DLL_h