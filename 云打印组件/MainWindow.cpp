
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
    AppendMenu(this->topRootMenu, MF_POPUP, (UINT_PTR)this->printMenu, TEXT("��ӡ��"));

    DrawMenuBar(this->GetHwnd());        // ǿ���ػ�˵���
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
        pMMI->ptMinTrackSize.x = 800; // ��С��� 
        pMMI->ptMinTrackSize.y = 400; // ��С�߶� 
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
    //�˵�

    this->topRootMenu = CreateMenu();
    this->printMenu = CreatePopupMenu();
    AppendMenu(this->topRootMenu, MF_POPUP, (UINT_PTR)this->printMenu, TEXT("��ӡ��"));
    SetMenu(this->GetHwnd(), this->topRootMenu);     // �����˵����ӵ�����
    DrawMenuBar(this->GetHwnd());        // ǿ���ػ�˵���


    //��ͼ�б�
    this->hTable = CreateWindowEx(0,
        WC_LISTVIEW,            // ʹ��ϵͳ�б���ͼ�ؼ�
        TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        0, 0, 0, 0,       // ����ͳߴ�
        this->GetHwnd(),             // �����ھ��
        (HMENU)IDC_PRINT_TABLE,       // �ؼ�ID
        this->GetHInstance(),
        NULL);

    LVCOLUMN lvc = { 0 };
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("����");
    ListView_InsertColumn(this->hTable, 0, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("����");
    ListView_InsertColumn(this->hTable, 1, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("ʱ��");
    ListView_InsertColumn(this->hTable, 2, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 100;
    lvc.pszText = (TCHAR*)TEXT("״̬");
    ListView_InsertColumn(this->hTable, 3, &lvc);

    //ϵͳ����
      //ϵͳ��������
    this->nid.cbSize = sizeof(NOTIFYICONDATA);
    this->nid.hWnd = hwnd;
    this->nid.uID = 1;
    this->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    this->nid.uCallbackMessage = WM_SYSTEM_NOTIFY;
    this->nid.hIcon = LoadIcon(this->hInstance, MAKEINTRESOURCE(IDI_ICON1));
    lstrcpy(this->nid.szTip, TEXT("�κ��ƴ�ӡ���"));
    Shell_NotifyIcon(NIM_ADD, &this->nid);

    //���̲˵�
    this->notifyMenu = CreatePopupMenu();
    AppendMenu(this->notifyMenu, MF_STRING, IDC_NOTIFY_MENU1, TEXT("����"));
    AppendMenu(this->notifyMenu, MF_STRING, IDC_NOTIFY_MENU2, TEXT("�˳�"));

    //�����豸�б�
    UpdateUsbDeviceList();


    //������ӡ�����߳�
    this->labelPrintThread.SetHwnd(this->GetHwnd());
    DSThread_Execute(&this->labelPrintThread);

    //����������
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
    WORD menuId = LOWORD(wParam);  // ��ȡ�˵���ID 
    if (menuId >= IDC_MENU_PRINT && menuId < IDC_MENU_PRINT + this->usbDeviceNum)
    {
        size_t deviceIndex = menuId - IDC_MENU_PRINT;

        // 1. ����ѡ���豸��InstanceId 
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
    case IDC_NOTIFY_MENU1: // �ָ����� 
        ShowWindow(hwnd, SW_RESTORE);
        break;
    case IDC_NOTIFY_MENU2: // �˳����� 
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
        lvItem.pszText = (LPWSTR)TEXT("��ӡ����");
        break;
    case LabelPrintState::Print_Success:
        lvItem.pszText = (LPWSTR)TEXT("��ӡ�ɹ���");
        break;
    case LabelPrintState::Print_Wait:
        lvItem.pszText = (LPWSTR)TEXT("�ȴ���ӡ��");
        break;
    default:
        break;
    }
    SendMessage(this->hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);


    this->labelPrintThread.PushLabel(label);

    /*
    if (this->openUsbDeviceHandle==NULL) {
        MessageBox(NULL, TEXT("δѡ���ǩ��ӡ����"), TEXT("���棡"), MB_SYSTEMMODAL);
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
        lstrcat(stateStr, TEXT("��ӡ���,"));
    }
    else {
        if (state.b.a)
            lstrcat(stateStr, TEXT("����,"));
        if (state.b.b)
            lstrcat(stateStr, TEXT("��ֽ,"));
        if (state.b.c)
            lstrcat(stateStr, TEXT("ȱֽ,"));
        if (state.b.d)
            lstrcat(stateStr, TEXT("��̼��,"));
        if (state.b.e)
            lstrcat(stateStr, TEXT("��ͣ��ӡ,"));
        if (state.b.f)
            lstrcat(stateStr, TEXT("���ڴ�ӡ,"));
        if (state.b.g)
            lstrcat(stateStr, TEXT("��Ǵ�,"));
        if (state.b.h)
            lstrcat(stateStr, TEXT("����,"));
        if (state.b.i)
            lstrcat(stateStr, TEXT("����Ӧ,"));
    }
    //ȥβ�͵Ķ���
    if (lstrlen(stateStr)) {
        stateStr[lstrlen(stateStr) - 1] = TEXT('\0');
    }
    SendMessage(this->hTable, LVM_SETITEM, 0, (LPARAM)&lvItem);
}

void MainWindow::OnSystemNotify(WPARAM wParam, LPARAM lParam){
    switch (lParam) {
    case WM_RBUTTONUP: // �Ҽ���� 
    {
        POINT pt;
        GetCursorPos(&pt); // ��ȡ���λ�� 
        SetForegroundWindow(this->hwnd); // ȷ���˵���ǰ̨ 
        TrackPopupMenu(this->notifyMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->hwnd, NULL);
        PostMessage(this->hwnd, WM_NULL, 0, 0); // ȷ���˵���ȷ�ر� 
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
    //CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL ����
                //CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL �뿪
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
