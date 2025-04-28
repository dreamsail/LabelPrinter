#pragma once
#ifndef HTTP_SERVER_THREAD_H
#define HTTP_SERVER_THREAD_H
#include <httplib.h>
#include <util/IThread.h>
#include <util/StrConv.h>

class HttpServerThread :
    public ds::IThread
{
public:
    // Í¨¹ý IThread ¼Ì³Ð
    void Release() override;
    void Run() override;
    void SetHandle(HANDLE handle) override;
    HANDLE GetHandle() override;
public:
    std::string ip;
    UINT16 port;
    HWND hwnd;
    HANDLE handle;
    httplib::Server svr;
public:
    HttpServerThread();
    HttpServerThread(std::string _Ip, UINT16 _Port, HWND _Hwnd);
    ~HttpServerThread();
    BOOL Middleware(const httplib::Request& req, httplib::Response& res);
    BOOL Cors(const httplib::Request& req, httplib::Response& res);
    void SetConfig(std::string _Ip, UINT16 _Port, HWND _Hwnd);
};
#endif // !HTTP_SERVER_THREAD_H