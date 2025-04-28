#pragma once
#ifndef DS_ITHREAD_H
#define DS_ITHREAD_H
#include <Windows.h>
#include "IBase.h"
#include "Dll.h"
#ifdef __cplusplus
extern "C" {
#endif

    namespace ds {
        class IThread :
            public IBase
        {
        public:
            // Í¨¹ý IBase ¼Ì³Ð
            virtual void Release() =0 ;
            virtual void Run() = 0;
            virtual void SetHandle(HANDLE handle) = 0;
            virtual HANDLE GetHandle() = 0;
        };
    }

    DLL_API HANDLE WINAPI DSThread_Execute(ds::IThread* thread);
#ifdef __cplusplus
}
#endif
#endif // ! DS_ITHREAD_H



