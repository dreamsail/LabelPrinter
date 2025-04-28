
#include "IWindow.h"


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

