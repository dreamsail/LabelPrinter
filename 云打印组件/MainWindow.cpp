
#include "resource.h"
#include "MainWindow.h"
#include "CustomWM.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <CommCtrl.h>
#include <tchar.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif 


std::vector<DS_USB_VID_PID>enableDeviceList = {
    {0x0471,0x0055}
};

MainWindow::MainWindow()
{
    this->usbDeviceNum = 0;
    this->selectUsbDeviceInstanceId[0] = '\0';
    this->openUsbDeviceHandle = NULL;

    DSUsb_RegisterDeviceArrivalRemoval((PCM_NOTIFY_CALLBACK)MainWindow::DeviceArrivalRemovalCallback, this);
}

void MainWindow::Release()
{
    this->Destroy();
    CloseHandle(openUsbDeviceHandle);
    delete this;
}

void MainWindow::UpdateUsbDeviceList()
{
    this->usbDeviceNum = DSUsb_GetUsbDeviceList(this->usbDeviceList, MAX_USB_DEVICE_NUM, enableDeviceList.data(), enableDeviceList.size());
    for (size_t i = 0; i < this->usbDeviceNum; i++)
    {
        DSUsb_GetDevicePropertys(&this->usbDeviceList[i]);
    }

    this->UpdateUsbDeviceMenuList();
}

void MainWindow::UpdateUsbDeviceMenuList()
{
    RemoveMenu(this->topRootMenu, (UINT)this->printMenu, MF_BYCOMMAND);
    DestroyMenu(this->printMenu);

    this->printMenu = CreatePopupMenu();
    for (UINT i = 0; i < this->usbDeviceNum; i++)
    {
        BOOL isChecked = lstrcmp(this->usbDeviceList[i].InstanceId, this->selectUsbDeviceInstanceId) == 0;

        InsertMenu(this->printMenu, i,
            (isChecked ? MF_CHECKED : 0) | MF_STRING,
            IDC_MENU_PRINT + i, this->usbDeviceList[i].FriendlyName);

    }
    AppendMenu(this->topRootMenu, MF_POPUP, (UINT_PTR)this->printMenu, TEXT("打印机"));

    DrawMenuBar(this->GetHwnd());        // 强制重绘菜单栏
}

LRESULT MainWindow::OnEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    case WM_CREATE: {
        this->OnCreate(wParam, lParam);
        break;
    }
    case WM_CLOSE: {
        ShowWindow(this->hwnd, SW_HIDE);
        return 0;
        break;
    }
    case WM_SIZE: {
        this->OnSize(wParam, lParam);
        break;
    }
    case WM_GETMINMAXINFO: {
        MINMAXINFO* pMMI = (MINMAXINFO*)lParam;
        pMMI->ptMinTrackSize.x = 800; // 最小宽度 
        pMMI->ptMinTrackSize.y = 400; // 最小高度 
        break;
    }
    case WM_COMMAND: {
        this->OnCommand(wParam, lParam);
        break;
    }
    case WM_USB_DEVICE_NOTIFY: {
        this->UpdateUsbDeviceList();
        break;
    }
    case WM_ADD_LABEL: {
        this->OnAddLabel(wParam, lParam);
        break;
    }
    case WM_PRINT_DEVICE_STATE: {
        this->OnPrintDeviceState(wParam, lParam);
        break;
    } 
    case WM_SYSTEM_NOTIFY: {
        this->OnSystemNotify(wParam, lParam);
        break;
    }
    case WM_RUN_STATE: {
        return RUN_STATE__RUN;
        break;
    }
    default:
        break;
    }
    return DefWindowProc(this->GetHwnd(), msg, wParam, lParam);
}

void MainWindow::OnCreate(WPARAM wParam, LPARAM lParam)
{
    //菜单

    this->topRootMenu = CreateMenu();
    this->printMenu = CreatePopupMenu();
    AppendMenu(this->topRootMenu, MF_POPUP, (UINT_PTR)this->printMenu, TEXT("打印机"));
    SetMenu(this->GetHwnd(), this->topRootMenu);     // 将主菜单附加到窗口
    DrawMenuBar(this->GetHwnd());        // 强制重绘菜单栏


    //视图列表
    this->hTable = CreateWindowEx(0,
        WC_LISTVIEW,            // 使用系统列表视图控件
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        0, 0, 0, 0,       // 坐标和尺寸
        this->GetHwnd(),             // 父窗口句柄
        (HMENU)IDC_PRINT_TABLE,       // 控件ID
        this->GetHInstance(),
        NULL);

    LVCOLUMN lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("标题");
    ListView_InsertColumn(this->hTable, 0, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("数量");
    ListView_InsertColumn(this->hTable, 1, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("时间");
    ListView_InsertColumn(this->hTable, 2, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("状态");
    ListView_InsertColumn(this->hTable, 3, &lvc);

    //系统托盘
      //系统托盘设置
    this->nid.cbSize = sizeof(NOTIFYICONDATA);
    this->nid.hWnd = hwnd;
    this->nid.uID = 1;
    this->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    this->nid.uCallbackMessage = WM_SYSTEM_NOTIFY;
    this->nid.hIcon = LoadIcon(this->hInstance, MAKEINTRESOURCE(IDI_ICON1));
    lstrcpy(this->nid.szTip, TEXT("梦航云打印组件"));
    Shell_NotifyIcon(NIM_ADD, &this->nid);

    //托盘菜单
    this->notifyMenu = CreatePopupMenu();
    AppendMenu(this->notifyMenu, MF_STRING, IDC_NOTIFY_MENU1, TEXT("设置"));
    AppendMenu(this->notifyMenu, MF_STRING, IDC_NOTIFY_MENU2, TEXT("退出"));

    //更新设备列表
    UpdateUsbDeviceList();


    //启动打印任务线程
    this->labelPrintThread.SetHwnd(this->GetHwnd());
    DSThread_Execute(&this->labelPrintThread);

    //启动服务器
    this->httpServerThread.SetConfig("127.0.0.1", 16500, this->GetHwnd());
    DSThread_Execute(&this->httpServerThread);
   
}

void MainWindow::OnSize(WPARAM wParam, LPARAM lParam)
{

    RECT clientRect;
    GetClientRect(this->GetHwnd(), &clientRect);
    SetWindowPos(this->hTable, NULL, 4, 4, clientRect.right - 8, clientRect.bottom - 8, SWP_NOZORDER);

    ListView_SetColumnWidth(this->hTable, 0, max((clientRect.right - 8) / 4, 150));
    ListView_SetColumnWidth(this->hTable, 1, max((clientRect.right - 8) / 4, 100));
    ListView_SetColumnWidth(this->hTable, 2, max((clientRect.right - 8) / 4, 100));
    ListView_SetColumnWidth(this->hTable, 3, max((clientRect.right - 8) / 4, 100));
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    WORD menuId = LOWORD(wParam);  // 获取菜单项ID 
    if (menuId >= IDC_MENU_PRINT && menuId < IDC_MENU_PRINT + this->usbDeviceNum)
    {
        size_t deviceIndex = menuId - IDC_MENU_PRINT;

        // 1. 更新选中设备的InstanceId 
        lstrcpy(this->selectUsbDeviceInstanceId,
            this->usbDeviceList[deviceIndex].InstanceId);
        CloseHandle(this->openUsbDeviceHandle);
        this->openUsbDeviceHandle = CreateFile(
            this->usbDeviceList[deviceIndex].Interface,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL
        );
        this->labelPrintThread.SetDeviceHandle(this->openUsbDeviceHandle);
        this->UpdateUsbDeviceMenuList();
    }

    switch (menuId)
    {
    case IDC_NOTIFY_MENU1: // 恢复窗口 
        ShowWindow(hwnd, SW_RESTORE);
        break;
    case IDC_NOTIFY_MENU2: // 退出程序 
        PostQuitMessage(0);
        break;
    }
}

void MainWindow::OnAddLabel(WPARAM wParam, LPARAM lParam)
{
    TCHAR tmpStr[128];
    ILabel* label = (ILabel*)lParam;

    LVITEM lvItem = { 0 };
    lvItem.mask = LVIF_TEXT | LVIF_PARAM;
    lvItem.lParam = lParam;

    int newRowIndex = SendMessage(this->hTable, LVM_GETITEMCOUNT, 0, 0);
    lvItem.iItem = newRowIndex;

    lvItem.iSubItem = 0;
    this->strConv.SetAscii(label->GetTitle());
    lvItem.pszText = this->strConv.u16;
    SendMessage(this->hTable, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

    lvItem.mask = LVIF_TEXT;
    lvItem.iSubItem = 1;
    lvItem.pszText = tmpStr;
    _stprintf(lvItem.pszText, TEXT("%d"), label->GetNum());
    SendMessage(this->hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);

    lvItem.iSubItem = 2;
    this->strConv.SetAscii(label->GetTime());
    lvItem.pszText = this->strConv.u16;
    SendMessage(this->hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);

    lvItem.iSubItem = 3;
    switch (label->GetState())
    {
    case LabelPrintState::Print_Error:
        lvItem.pszText = (LPWSTR)TEXT("打印错误！");
        break;
    case LabelPrintState::Print_Success:
        lvItem.pszText = (LPWSTR)TEXT("打印成功！");
        break;
    case LabelPrintState::Print_Wait:
        lvItem.pszText = (LPWSTR)TEXT("等待打印！");
        break;
    default:
        break;
    }
    SendMessage(this->hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);


    this->labelPrintThread.PushLabel(label);

    /*
    if (this->openUsbDeviceHandle==NULL) {
        MessageBox(NULL, TEXT("未选择标签打印机！"), TEXT("警告！"), MB_SYSTEMMODAL);
    }*/
}

void MainWindow::OnPrintDeviceState(WPARAM wParam, LPARAM lParam) {
    TCHAR stateStr[128] = { 0 };
    ILabel* label = (ILabel*)lParam;
    PrintDeviceState state;
    state.c = (unsigned short)wParam;

    LVFINDINFO lvFindInfo = { 0 };
    lvFindInfo.flags = LVFI_PARAM;
    lvFindInfo.lParam = lParam;
    int rowIndex = SendMessage(this->hTable, LVM_FINDITEM, -1, (LPARAM)&lvFindInfo);

    LVITEM lvItem = { 0 };
    lvItem.iItem = rowIndex;
    lvItem.iSubItem = 3;
    lvItem.pszText = stateStr;
    lvItem.mask = LVIF_TEXT;

    if (state.c == 0) {
        lstrcat(stateStr, TEXT("打印完成,"));
    }
    else {
        if (state.b.a)
            lstrcat(stateStr, TEXT("开盖,"));
        if (state.b.b)
            lstrcat(stateStr, TEXT("卡纸,"));
        if (state.b.c)
            lstrcat(stateStr, TEXT("缺纸,"));
        if (state.b.d)
            lstrcat(stateStr, TEXT("无碳带,"));
        if (state.b.e)
            lstrcat(stateStr, TEXT("暂停打印,"));
        if (state.b.f)
            lstrcat(stateStr, TEXT("正在打印,"));
        if (state.b.g)
            lstrcat(stateStr, TEXT("外盖打开,"));
        if (state.b.h)
            lstrcat(stateStr, TEXT("过热,"));
        if (state.b.i)
            lstrcat(stateStr, TEXT("无响应,"));
    }
    //去尾巴的逗号
    if (lstrlen(stateStr)) {
        stateStr[lstrlen(stateStr) - 1] = TEXT('\0');
    }
    SendMessage(this->hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);
}

void MainWindow::OnSystemNotify(WPARAM wParam, LPARAM lParam){
    switch (lParam) {
    case WM_RBUTTONUP: // 右键点击 
    {
        POINT pt;
        GetCursorPos(&pt); // 获取鼠标位置 
        SetForegroundWindow(this->hwnd); // 确保菜单在前台 
        TrackPopupMenu(this->notifyMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->hwnd, NULL);
        PostMessage(this->hwnd, WM_NULL, 0, 0); // 确保菜单正确关闭 
    }
    break;
    case WM_LBUTTONDBLCLK:
        ShowWindow(this->hwnd, SW_RESTORE);
        break;
    }
}

void WINAPI MainWindow::DeviceArrivalRemovalCallback(HCMNOTIFICATION hNotify, PVOID Context, CM_NOTIFY_ACTION Action, PCM_NOTIFY_EVENT_DATA EventData, DWORD EventDataSize)
{
    MainWindow* _this = (MainWindow*)Context;
    DS_USB_VID_PID vidpid;
    //CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL 到达
                //CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL 离开
    if (
        Action == CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL ||
        Action == CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL
        )
    {
        if (DSUsb_DeviceInterfaceToVidPid(EventData->u.DeviceInterface.SymbolicLink, &vidpid))
        {
            if (DSUsb_VidPidIncludes(enableDeviceList.data(), enableDeviceList.size(), enableDeviceList[0]))
            {
                PostMessage(_this->GetHwnd(), WM_USB_DEVICE_NOTIFY, 0, 0);
            }
        }
    }
}
