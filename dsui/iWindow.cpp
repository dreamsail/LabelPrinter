
#include "IWindow.h"
#include <dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")
HINSTANCE hInstance;
void WINAPI DSUI_Init(HINSTANCE _hInstance)
{
    hInstance = _hInstance;
}
// CS_HREDRAW | CS_VREDRAW
BOOL WINAPI DSUI_RegisterClass(const TCHAR* name, WNDPROC wndproc, UINT style)
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = hInstance;
    wc.lpszClassName = name;
    wc.cbWndExtra = sizeof(void*);
    wc.lpfnWndProc = wndproc;
    wc.style = style;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClassEx(&wc)) {
        return FALSE;
    }
    return TRUE;
}

// WS_TILEDWINDOW
HWND WINAPI DSUI_CreateWindow(IWindow* dWindow, HWND hWndParent, const TCHAR* className, const TCHAR* title, DWORD dwStyle, DWORD dwExStyle)
{
    
    dWindow->SetHInstance(hInstance);

    HWND hwnd = CreateWindowEx(
        dwExStyle,
        className,
        title,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hWndParent,
        NULL,
        hInstance,
        dWindow
    );
    return hwnd;
}

LRESULT WINAPI DSUI_MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    IWindow* _this;

    switch (msg)
    {
    case WM_CREATE: {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        _this = (IWindow*)cs->lpCreateParams;
        _this->SetHwnd(hwnd);
        SetWindowLongPtr(hwnd, 0, (LONG_PTR)_this);
        break;
    }
    default:
        break;
    }

    _this = (IWindow*)GetWindowLongPtr(hwnd, 0);
    if (_this)
    {
        return _this->OnEvent(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT WINAPI DSUI_NoFrameWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    IWindow* _this;
    
    switch (msg)
    {
    case WM_CREATE: {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        _this = (IWindow*)cs->lpCreateParams;
        _this->SetHwnd(hwnd);
        SetWindowLongPtr(hwnd, 0, (LONG_PTR)_this);
        break;
    }
    case WM_ACTIVATE: {
        HRESULT hr = S_OK;
        MARGINS margins;

        margins.cxLeftWidth = 0;      // 8
        margins.cxRightWidth = 0;    // 8
        margins.cyBottomHeight = 0; // 20
        margins.cyTopHeight =-50;       // 27

        hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
        if (SUCCEEDED(hr))
        {
            // ...
        }
        break;
    }
    case WM_NCHITTEST: {
        //RECT rc;
        //GetClientRect(hwnd, &rc);
        //POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        //ScreenToClient(hwnd, &pt);
        //// ¼ì²â±ßÔµÇøÓò£¨Èç8ÏñËØ¿í£©
        //const int border = 8;
        //if (pt.x < border && pt.y < border) return HTTOPLEFT;
        //else if (pt.x >= rc.right - border && pt.y < border) return HTTOPRIGHT;
        //else if (pt.x < border && pt.y >= rc.bottom - border) return HTBOTTOMLEFT;
        //else if (pt.x >= rc.right - border && pt.y >= rc.bottom - border) return HTBOTTOMRIGHT;
        //else if (pt.y < border) return HTTOP;
        //else if (pt.y >= rc.bottom - border) return HTBOTTOM;
        //else if (pt.x < border) return HTLEFT;
        //else if (pt.x >= rc.right - border) return HTRIGHT;
        //else return HTCAPTION;
    }
    default:
        break;
    }

    _this = (IWindow*)GetWindowLongPtr(hwnd, 0);
    if (_this)
    {
        return _this->OnEvent(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}