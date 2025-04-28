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
            unsigned char a : 1;//����
            unsigned char b : 1;//��ֽ
            unsigned char c : 1;//ȱֽ
            unsigned char d : 1;//��̼��
            unsigned char e : 1;//��ͣ��ӡ
            unsigned char f : 1;//���ڴ�ӡ
            unsigned char g : 1;//��Ǵ�
            unsigned char h : 1;//����

            unsigned char i : 1;//����Ӧ
        }b;
        unsigned short c;
    };
}PrintDeviceState;
class LabelPrintThread :
    public ds::IThread
{
public:
    // ͨ�� IThread �̳�
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