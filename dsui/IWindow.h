#pragma once
#ifndef DS_UI_H
#define DS_UI_H

#include <Windows.h>
#include <util/Dll.h>
#include <util/IBase.h>

#ifdef __cplusplus
extern "C" {
#endif




    class IWindow;
    class Window;

    DLL_API void WINAPI DSUI_Init(HINSTANCE _hInstance);
    DLL_API BOOL WINAPI DSUI_RegisterClass(const TCHAR* name, WNDPROC wndproc, UINT style);
    DLL_API HWND WINAPI DSUI_CreateWindow(IWindow* dWindow, HWND hWndParent, const TCHAR* className, const TCHAR* title, DWORD dwStyle, DWORD dwExStyle);
    DLL_API LRESULT WINAPI DSUI_MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    DLL_API LRESULT WINAPI DSUI_NoFrameWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class IWindow:public::ds::IBase
    {
    public:
        virtual void Release() = 0;
        virtual void SetHwnd(HWND hwnd) = 0;
        virtual HWND GetHwnd() = 0;
        virtual void SetHInstance(HINSTANCE hInstance) = 0;
        virtual HINSTANCE GetHInstance() = 0;
        virtual LRESULT OnEvent(UINT msg, WPARAM wParam, LPARAM lParam) = 0;
        virtual BOOL Destroy() = 0;
        virtual BOOL Show(int nCmdShow) = 0;
        virtual BOOL Update() = 0;
        virtual BOOL Move(int x, int y) = 0;
        virtual BOOL Resize(int w, int h) = 0;
        virtual HICON SetHIcon(HICON hIcon) = 0;
        virtual HCURSOR SetHCursor(HCURSOR hCursor) = 0;
        virtual HBRUSH SetHbrBackground(HBRUSH hbrBackground) = 0;
    };
    class Window :public IWindow {
    protected:
        HWND hwnd;
        HINSTANCE hInstance;
    public:
        void Release() override {
            this->Destroy();
            delete this;
        }
        void SetHwnd(HWND hwnd) override {
            this->hwnd = hwnd;
        }
        HWND GetHwnd() override {
            return this->hwnd;
        }
        void SetHInstance(HINSTANCE hInstance) override {
            this->hInstance = hInstance;
        }
        HINSTANCE GetHInstance() override {
            return this->hInstance;
        }
        LRESULT OnEvent(UINT msg, WPARAM wParam, LPARAM lParam) override {
            switch (msg)
            {
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            default:
                break;
            }
            return DefWindowProc(this->hwnd, msg, wParam, lParam);
        }

        BOOL Destroy() override {
            return DestroyWindow(this->hwnd);
        }
        BOOL Show(int nCmdShow) override {
            return ShowWindow(this->hwnd, nCmdShow);
        }
        BOOL Update() override {
            return UpdateWindow(this->hwnd);
        }
        BOOL Move(int x, int y) override {
            return  SetWindowPos(this->hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        BOOL Resize(int w, int h) override {
            return  SetWindowPos(this->hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
        }
        HICON SetHIcon(HICON hIcon) override {
            
            return (HICON)SetClassLongPtr(this->GetHwnd(), GCLP_HICON, (LONG_PTR)hIcon);
        }
        HCURSOR SetHCursor(HCURSOR hCursor) override {
            return HCURSOR();
        }
        HBRUSH SetHbrBackground(HBRUSH hbrBackground) override {
            return HBRUSH();
        }
    public:
        static BOOL Register(const TCHAR* name, WNDPROC wndproc) {
            return DSUI_RegisterClass(name, wndproc, CS_HREDRAW | CS_VREDRAW);
        }
        static BOOL Register(const TCHAR* name, WNDPROC wndproc, UINT style) {
            return DSUI_RegisterClass(name, wndproc, style);
        }
        BOOL Create(HWND hWndParent, const TCHAR* className, const TCHAR* title) {
            this->hwnd = DSUI_CreateWindow(this, hWndParent, className, title, WS_TILEDWINDOW, NULL);
            return this->hwnd ? TRUE : FALSE;
        }
        BOOL Create(HWND hWndParent, const TCHAR* className, const TCHAR* title, DWORD dwStyle, DWORD dwExStyle) {
            this->hwnd = DSUI_CreateWindow(this, hWndParent, className, title, dwStyle, dwExStyle);
            return this->hwnd ? TRUE : FALSE;
        }
    };
#ifdef __cplusplus
}
#endif

#endif