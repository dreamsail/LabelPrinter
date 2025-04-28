#include "HttpServerThread.h"
#include <json/json.h>
#include <Label/Label.h>
#pragma comment(lib,"Label.lib")
#include "CustomWM.h"

Json::Value stringToJson(const std::string& jsonString) {
    Json::Value root;   // 用于存储解析后的 JSON 对象 
    Json::CharReaderBuilder reader; // JSON 解析器 
    std::string errs;   // 存储解析错误信息 

    // 解析 JSON 字符串 
    std::istringstream s(jsonString);
    Json::parseFromStream(reader, s, &root, &errs);
    return root;
}

void HttpServerThread::Release()
{
    this->~HttpServerThread();
    delete this;
}

void HttpServerThread::Run()
{ // 处理跨域预检请求（OPTIONS）
    this->svr.Options("/.*", [this](const httplib::Request& req, httplib::Response& res) {
        if (!this->Middleware(req, res))return;
        res.status = 204; // No Content 
        });

    this->svr.Post("/print", [this](const httplib::Request& req, httplib::Response& res) {
        if (!this->Middleware(req, res))return;

        ds::StrConv strConv;

        strConv.SetU8(req.body.c_str());

        Json::Value root = stringToJson(strConv.ascii);

        for (auto it = root.begin(); it != root.end(); it++)
        {
            const Json::Value& jsonLabel = *it;

            ILabel* label = DSLabel_Create(jsonLabel);
            SendMessage(this->hwnd, WM_ADD_LABEL, 0, (LPARAM)label);
        }
        res.status = 200;
        });
    this->svr.listen(this->ip, this->port);
}

void HttpServerThread::SetHandle(HANDLE handle)
{
    this->handle = handle;
}

HANDLE HttpServerThread::GetHandle()
{
    return this->handle;
}

HttpServerThread::HttpServerThread()
{
    this->ip = "";
    this->port = NULL;
    this->hwnd = NULL;
    this->handle = NULL;
}

HttpServerThread::HttpServerThread(std::string _Ip, UINT16 _Port, HWND _Hwnd)
{
    this->ip = _Ip;
    this->port = _Port;
    this->hwnd = _Hwnd;
    this->handle = NULL;
}

void HttpServerThread::SetConfig(std::string _Ip, UINT16 _Port, HWND _Hwnd)
{
    this->ip = _Ip;
    this->port = _Port;
    this->hwnd = _Hwnd;
}

HttpServerThread::~HttpServerThread()
{
    this->svr.stop();
    WaitForSingleObject(this->handle, INFINITE);
    CloseHandle(this->handle);
}

BOOL HttpServerThread::Middleware(const httplib::Request& req, httplib::Response& res) {
    if (!this->Cors(req, res))return FALSE;
    return TRUE;
}

BOOL HttpServerThread::Cors(const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", req.get_header_value("Origin"));
    res.set_header("Access-Control-Allow-Credentials", "true");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return TRUE;
}


