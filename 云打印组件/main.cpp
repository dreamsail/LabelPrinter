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
    icc.dwICC = ICC_LISTVIEW_CLASSES;  // ��ȷ��ʼ���б���ͼ�� 
    InitCommonControlsEx(&icc);
}

std::wstring GetFileVersion() {
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH); // ��ȡ��ǰ��ִ���ļ�·��

    DWORD dwHandle = 0;
    DWORD dwSize = GetFileVersionInfoSize(szPath, &dwHandle);
    if (dwSize == 0) return TEXT("0.0.0.0"); // ʧ�ܴ��� 

    std::vector<TCHAR> buffer(dwSize);
    if (!GetFileVersionInfo(szPath, dwHandle, dwSize, buffer.data())) {
        return TEXT("0.0.0.0");
    }

    VS_FIXEDFILEINFO* pFileInfo = nullptr;
    UINT uLen = 0;
    if (!VerQueryValue(buffer.data(), TEXT("\\"), (LPVOID*)&pFileInfo, &uLen)) {
        return TEXT("0.0.0.0");
    }

    // �����汾�ţ��Ķ�ʽ�����汾.�ΰ汾.�޶���.�����ţ�
    std::wstring szVersion = std::format(TEXT("{}.{}.{}.{}"),
        HIWORD(pFileInfo->dwFileVersionMS), // ���汾 
        LOWORD(pFileInfo->dwFileVersionMS), // �ΰ汾
        HIWORD(pFileInfo->dwFileVersionLS), // �޶��� 
        LOWORD(pFileInfo->dwFileVersionLS)  // ������
    );
    return szVersion;
}

//ȷ������Ψһ����
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

//ע������
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


// ������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    HWND hwnd = uniqueRun();
    if (hwnd) {
        //���ѳ���ǰ̨���ڲ��˳�
        ShowWindow(hwnd, SW_SHOW);
        return 0;
    }
    ERROR_NOACCESS;
    DSUsb_Init();
    DSUI_Init(hInstance);
    initCommCtrl();

    auto version= GetFileVersion();
	auto windowTitle = std::format(TEXT("��ǩ�ƴ�ӡ��� V{}"), version);

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