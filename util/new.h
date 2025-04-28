#pragma once
#ifndef DS_UTIL_NEW_H
#define DS_UTIL_NEW_H
#include <iostream>
inline void* operator new(size_t size)
{
    return malloc(size);
}
inline void operator delete(void* ptr)
{
    free(ptr);
}
//虚函数表默认绑定的函数，此函数在C++运行时默认导出，剥离运行时的情况下要自定义该函数
extern "C" inline int __cdecl _purecall(void)
{
    return 0;
}
#endif // !DS_UTIL_NEW_H
