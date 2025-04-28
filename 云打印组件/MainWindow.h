#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "HttpServerThread.h"
#include "LabelPrintThread.h"

#include <dsui/IWindow.h>
#pragma comment(lib,"dsui.lib")
#include <libdsusb/usb.h>
#pragma comment(lib,"libdsusb.lib")
#include <Label/Label.h>
#pragma comment(lib,"Label.lib")
#include <util/StrConv.h>
#pragma comment(lib,"util.lib")
#include <Windows.h>




#define MAX_USB_DEVICE_NUM 12

class MainWindow :
    public Window
{
public:
    MainWindow();
    void Release() override;
    void UpdateUsbDeviceList();
    void UpdateUsbDeviceMenuList();

    LRESULT OnEvent(UINT msg, WPARAM wParam, LPARAM lParam) override;
    void OnCreate(WPARAM wParam, LPARAM lParam);
    void OnSize(WPARAM wParam, LPARAM lParam);
    void OnCommand(WPARAM wParam, LPARAM lParam);

    void OnAddLabel(WPARAM wParam, LPARAM lParam);
    void OnPrintDeviceState(WPARAM wParam, LPARAM lParam);
    void OnSystemNotify(WPARAM wParam, LPARAM lParam);

    static void WINAPI DeviceArrivalRemovalCallback(
        HCMNOTIFICATION       hNotify,
        PVOID                 Context,
        CM_NOTIFY_ACTION      Action,
        PCM_NOTIFY_EVENT_DATA EventData,
        DWORD                 EventDataSize
    );

private:
    HMENU topRootMenu, printMenu,notifyMenu;
    HWND hTable;
    NOTIFYICONDATA nid;

    DS_USB_DEVICE usbDeviceList[MAX_USB_DEVICE_NUM];
    TCHAR selectUsbDeviceInstanceId[MAX_PATH];
    size_t usbDeviceNum;

    HttpServerThread httpServerThread;
    LabelPrintThread labelPrintThread;

    HANDLE openUsbDeviceHandle;

    ds::StrConv strConv;
};

#endif // ! MAIN_WINDOW_H
