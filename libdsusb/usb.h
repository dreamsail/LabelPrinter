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
	* ��ѯ_VidPidList�����Ƿ����ָ��vidpid
	* _VidPidList vidpid����
	* _VidPidNum _VidPidList��������
	* _VidPid Ҫ��ѯ��vidpid
	* �����Ƿ����
	*/
	DLL_API BOOL WINAPI DSUsb_VidPidIncludes(DS_USB_VID_PID* _VidPidList, size_t _VidPidNum, DS_USB_VID_PID _VidPid);

	DLL_API size_t WINAPI DSUsb_GetUsbDeviceList(DS_USB_DEVICE _DeviceList[], size_t _MaxListLen,DS_USB_VID_PID _FilterList[], size_t _FilterLen);

	DLL_API void WINAPI DSUsb_GetDevicePropertys(DS_USB_DEVICE* _Device);


	/*
	* �豸��Ϣ�Ļص����� 
	* _Action ���� or �뿪
	typedef DWORD (WINAPI *PCM_NOTIFY_CALLBACK)(
		HCMNOTIFICATION hNotify,			//��Ϣ���
		PVOID Context,						//���ݵ��Զ���������
		CM_NOTIFY_ACTION Action,			//��Ϣ
				//CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL ����
	 			//CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL �뿪
		PCM_NOTIFY_EVENT_DATA EventData,	//�豸��Ϣ
		DWORD EventDataSize					//�豸��Ϣ�ṹ��ߴ�
		);
	*/
	/*
	* �豸��Ϣ�ص�ע�ắ��
	* _Fun �ص�����
	* _PContext ���ݵ��Զ���������
	*/

	DLL_API HCMNOTIFICATION WINAPI DSUsb_RegisterDeviceArrivalRemoval(PCM_NOTIFY_CALLBACK _Fun, void* _PContext);

	//�豸ʵ��id����ȡvidpid
	DLL_API BOOL WINAPI DSUsb_DeviceInstanceIdToVidPid(TCHAR* _InstanceId, DS_USB_VID_PID* _VidPid);
	//�豸·������ȡvidpid
	DLL_API BOOL WINAPI DSUsb_DeviceInterfaceToVidPid(TCHAR* _DeviceInterface, DS_USB_VID_PID* _VidPid);


#ifdef __cplusplus
}
#endif
#endif // ! DS_USB_H