#pragma once
#ifndef LABEL_PRINT_THREAD_H
#define LABEL_PRINT_THREAD_H
#include <util/IThread.h>
#include <Label/Label.h>
#include <queue>
typedef struct PrintDeviceState {
    union {
        unsigned char a;
        struct {
            unsigned char a : 1;//开盖
            unsigned char b : 1;//卡纸
            unsigned char c : 1;//缺纸
            unsigned char d : 1;//无碳带
            unsigned char e : 1;//暂停打印
            unsigned char f : 1;//正在打印
            unsigned char g : 1;//外盖打开
            unsigned char h : 1;//过热

            unsigned char i : 1;//无响应
        }b;
        unsigned short c;
    };
}PrintDeviceState;
class LabelPrintThread :
    public ds::IThread
{
public:
    // 通过 IThread 继承
    void Release() override;
    void Run() override;
    void SetHandle(HANDLE handle) override;
    HANDLE GetHandle() override;
private:
    HWND hwnd;
    HANDLE handle,deviceHandle;

    CRITICAL_SECTION criticalSection;
    CONDITION_VARIABLE conditionVariable;
    OVERLAPPED ov;

    std::queue<ILabel*> labelList;
private:
    PrintDeviceState GetDeviceState();
public:
    LabelPrintThread();
    ~LabelPrintThread();

    void SetDeviceHandle(HANDLE handle);
    HANDLE GetDeviceHandle();

    void SetHwnd(HWND hwnd);
    HWND GetHwnd();

    void PushLabel(ILabel* label);
};

#endif // !LABEL_PRINT_THREAD_H