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
//�麯����Ĭ�ϰ󶨵ĺ������˺�����C++����ʱĬ�ϵ�������������ʱ�������Ҫ�Զ���ú���
extern "C" inline int __cdecl _purecall(void)
{
    return 0;
}
#endif // !DS_UTIL_NEW_H
