#include "resource.h"
#include "CustomWM.h"
#include "MainWindow.h"
#include<CommCtrl.h>
#pragma comment(lib, "comctl32.lib") 



void initCommCtrl() {
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES;  // 明确初始化列表视图类 
    InitCommonControlsEx(&icc);
}

//确保程序唯一运行
HWND uniqueRun() {
    BOOL result= TRUE;
    DWORD disposition;
    HKEY softwareKey=NULL;
    HKEY dreamsailKey = NULL;
    HKEY printlKey = NULL;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &softwareKey, &disposition)!= ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result&&RegCreateKeyEx(softwareKey, TEXT("DreamSail"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &dreamsailKey, &disposition) != ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result && RegCreateKeyEx(dreamsailKey, TEXT("Print"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &printlKey, &disposition) != ERROR_SUCCESS) {
        result = FALSE;
    }
    HWND hwnd=NULL;
    DWORD size=sizeof(hwnd);
    if (result && RegGetValue(printlKey, NULL, TEXT("RunHwnd"), RRF_RT_REG_QWORD, NULL, &hwnd, &size) != ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result && SendMessage(hwnd, WM_RUN_STATE, 0, 0)!= RUN_STATE__RUN){
        result = FALSE;
    }
    RegCloseKey(softwareKey);
    RegCloseKey(dreamsailKey);
    RegCloseKey(printlKey);
    if (result){
        return hwnd;
    }
    else {
        return NULL;
    } 
}

//注册运行
BOOL registRun(HWND hwnd) {
    BOOL result = TRUE;
    DWORD disposition;
    HKEY softwareKey = NULL;
    HKEY dreamsailKey = NULL;
    HKEY printlKey = NULL;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &softwareKey, &disposition) != ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result && RegCreateKeyEx(softwareKey, TEXT("DreamSail"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &dreamsailKey, &disposition) != ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result && RegCreateKeyEx(dreamsailKey, TEXT("Print"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &printlKey, &disposition) != ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result && RegSetValueEx(printlKey, TEXT("RunHwnd"),NULL, REG_QWORD, (BYTE*)&hwnd, sizeof(hwnd)) != ERROR_SUCCESS) {
        result = FALSE;
    } 
    RegCloseKey(softwareKey);
    RegCloseKey(dreamsailKey);
    RegCloseKey(printlKey);
    return result;
}


// 主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    HWND hwnd = uniqueRun();
    if (hwnd) {
        //唤醒程序前台窗口并退出
        ShowWindow(hwnd, SW_SHOW);
        return 0;
    }
    ERROR_NOACCESS;
    DSUsb_Init();
    DSUI_Init(hInstance);
    initCommCtrl();

    MainWindow* mainWindow = new MainWindow();
    mainWindow->Register(TEXT("MainWindow"), DSUI_MainWindowProc);
    mainWindow->Create(NULL, TEXT("MainWindow"), TEXT("标签云打印组件 V2.0.0.4"));
    mainWindow->Resize(700, 400);
    mainWindow->SetHIcon(LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)));
    mainWindow->Show(nCmdShow);
    mainWindow->Update();

    registRun(mainWindow->GetHwnd());

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    mainWindow->Release();

    return 0;
}