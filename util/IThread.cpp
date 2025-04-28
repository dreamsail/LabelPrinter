#include "IThread.h"

DWORD WINAPI ThreadMain(LPVOID lpParameter) {
    ds::IThread* thread = (ds::IThread*)lpParameter;
    thread->Run();
    return 0;
}
HANDLE WINAPI DSThread_Execute(ds::IThread* thread)
{
    HANDLE handle = CreateThread(NULL, 0, ThreadMain, thread, 0, 0);
    thread->SetHandle(handle);
    return handle;
}
