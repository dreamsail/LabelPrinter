#pragma once
#ifndef DS_USB_H
#define DS_USB_H

#include <Windows.h>
#include <cfgmgr32.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDLL 
#define DLL_API __declspec(dllexport)
#else 
#ifdef _DLL
#define DLL_API __declspec(dllimport)
#else
#define DLL_API
#endif  //_DLL
#endif //_WINDLL


	typedef struct DS_USB_VID_PID {
		UINT16 vid;
		UINT16 pid;
	}DS_USB_VID_PID;

	typedef struct DS_USB_DEVICE {
		DEVINST  DevInst;
		TCHAR InstanceId[MAX_PATH];
		TCHAR Interface[MAX_PATH];
		TCHAR FriendlyName[MAX_PATH];
		TCHAR SymbolicName[MAX_PATH];
		DS_USB_VID_PID VID_PID;
	}DS_USB_DEVICE;


	DLL_API BOOL WINAPI DSUsb_Init();

	/*
	* 查询_VidPidList数组是否包含指定vidpid
	* _VidPidList vidpid数组
	* _VidPidNum _VidPidList数组数量
	* _VidPid 要查询的vidpid
	* 返回是否包含
	*/
	DLL_API BOOL WINAPI DSUsb_VidPidIncludes(DS_USB_VID_PID* _VidPidList, size_t _VidPidNum, DS_USB_VID_PID _VidPid);

	DLL_API size_t WINAPI DSUsb_GetUsbDeviceList(DS_USB_DEVICE _DeviceList[], size_t _MaxListLen,DS_USB_VID_PID _FilterList[], size_t _FilterLen);

	DLL_API void WINAPI DSUsb_GetDevicePropertys(DS_USB_DEVICE* _Device);


	/*
	* 设备消息的回调函数 
	* _Action 到达 or 离开
	typedef DWORD (WINAPI *PCM_NOTIFY_CALLBACK)(
		HCMNOTIFICATION hNotify,			//消息句柄
		PVOID Context,						//传递的自定义上下文
		CM_NOTIFY_ACTION Action,			//消息
				//CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL 到达
	 			//CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL 离开
		PCM_NOTIFY_EVENT_DATA EventData,	//设备消息
		DWORD EventDataSize					//设备消息结构体尺寸
		);
	*/
	/*
	* 设备消息回调注册函数
	* _Fun 回调函数
	* _PContext 传递的自定义上下文
	*/

	DLL_API HCMNOTIFICATION WINAPI DSUsb_RegisterDeviceArrivalRemoval(PCM_NOTIFY_CALLBACK _Fun, void* _PContext);

	//设备实例id中提取vidpid
	DLL_API BOOL WINAPI DSUsb_DeviceInstanceIdToVidPid(TCHAR* _InstanceId, DS_USB_VID_PID* _VidPid);
	//设备路径中提取vidpid
	DLL_API BOOL WINAPI DSUsb_DeviceInterfaceToVidPid(TCHAR* _DeviceInterface, DS_USB_VID_PID* _VidPid);


#ifdef __cplusplus
}
#endif
#endif // ! DS_USB_H