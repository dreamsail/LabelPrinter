#include "resource.h"
#include "CustomWM.h"
#include "MainWindow.h"
#include <format>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib") 
#pragma comment(lib, "Version.lib") 


void initCommCtrl() {
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES;  // 明确初始化列表视图类 
    InitCommonControlsEx(&icc);
}

std::wstring GetFileVersion() {
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH); // 获取当前可执行文件路径

    DWORD dwHandle = 0;
    DWORD dwSize = GetFileVersionInfoSize(szPath, &dwHandle);
    if (dwSize == 0) return TEXT("0.0.0.0"); // 失败处理 

    std::vector<TCHAR> buffer(dwSize);
    if (!GetFileVersionInfo(szPath, dwHandle, dwSize, buffer.data())) {
        return TEXT("0.0.0.0");
    }

    VS_FIXEDFILEINFO* pFileInfo = nullptr;
    UINT uLen = 0;
    if (!VerQueryValue(buffer.data(), TEXT("\\"), (LPVOID*)&pFileInfo, &uLen)) {
        return TEXT("0.0.0.0");
    }

    // 解析版本号（四段式：主版本.次版本.修订号.构建号）
    std::wstring szVersion = std::format(TEXT("{}.{}.{}.{}"),
        HIWORD(pFileInfo->dwFileVersionMS), // 主版本 
        LOWORD(pFileInfo->dwFileVersionMS), // 次版本
        HIWORD(pFileInfo->dwFileVersionLS), // 修订号 
        LOWORD(pFileInfo->dwFileVersionLS)  // 构建号
    );
    return szVersion;
}

//确保程序唯一运行
HWND uniqueRun() {
    BOOL result= TRUE;
    DWORD disposition;

    HKEY printlKey = NULL;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software/DreamSail/Print"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &printlKey, &disposition)!= ERROR_SUCCESS) {
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
    HKEY printlKey = NULL;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software/DreamSail/Print"), 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, &printlKey, &disposition) != ERROR_SUCCESS) {
        result = FALSE;
    }
    if (result && RegSetValueEx(printlKey, TEXT("RunHwnd"),NULL, REG_QWORD, (BYTE*)&hwnd, sizeof(hwnd)) != ERROR_SUCCESS) {
        result = FALSE;
    } 

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

    auto version= GetFileVersion();
	auto windowTitle = std::format(TEXT("标签云打印组件 V{}"), version);

    MainWindow* mainWindow = new MainWindow();
    mainWindow->Register(TEXT("MainWindow"), DSUI_MainWindowProc);
    mainWindow->Create(NULL, TEXT("MainWindow"), windowTitle.data());
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