#pragma once
#ifndef CUSTOM_WM_H
#define CUSTOM_WM_H


#define WM_RUN_STATE				WM_USER		+1000   //查询运行状态，避免重复运行程序
/*   */ #define RUN_STATE__RUN		0xFFFF8800          //程序只有这个状态 无返回就是未运行，返回错误就是非本程序
#define WM_USB_DEVICE_NOTIFY        WM_USER     +1001	//usb设备热插拔消息
#define WM_ADD_LABEL                WM_USER     +1002   //添加标签
#define WM_PRINT_DEVICE_STATE		WM_USER     +1003	//发送打印状态
#define WM_SYSTEM_NOTIFY			WM_USER     +1004	//系统托盘消息

#endif // ! CUSTOM_WM_H