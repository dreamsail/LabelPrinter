

#include "usb.h"
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <setupapi.h>
#include <initguid.h>
#include <Usbiodef.h>
#include <Devguid.h>
#include <Devpkey.h>

#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"Cfgmgr32.lib")


BOOL WINAPI DSUsb_Init()
{
	return TRUE;
}


BOOL WINAPI DSUsb_VidPidIncludes(DS_USB_VID_PID* _VidPidList, size_t _VidPidNum, DS_USB_VID_PID _VidPid) {

    for (size_t i = 0; i < _VidPidNum; i++)
    {
        if (_VidPidList[i].vid== _VidPid.vid&& _VidPidList[i].pid == _VidPid.pid)
        {
            return TRUE;
        }
    }
    return FALSE;
}
size_t WINAPI DSUsb_SplitNullTerminated(TCHAR** _Segments, size_t _MaxSegments, TCHAR* _Src, size_t _SrcSize) {
	size_t count = 0;
	size_t offset = 0;

	while (count < _MaxSegments && offset < _SrcSize) {
		if (!_Src[offset]) {
			break;
		}
		_Segments[count] = &_Src[offset];
		offset += lstrlen(_Segments[count]) + 1;
		count++;
	}
	return count;
}



size_t WINAPI DSUsb_GetUsbDeviceList(DS_USB_DEVICE _DeviceList[], size_t _MaxListLen, DS_USB_VID_PID _FilterList[], size_t _FilterLen) {
	TCHAR* buff;
	CONFIGRET cr;
	
	buff = (TCHAR*)malloc(MAX_PATH * 128);
	if (!buff) {
		return 0;
	}

	cr=CM_Get_Device_ID_List(TEXT("{36FC9E60-C465-11CF-8056-444553540000}"), buff, MAX_PATH * 128, CM_GETIDLIST_FILTER_CLASS| CM_GETIDLIST_FILTER_PRESENT);
	if (cr != CR_SUCCESS) {
		free(buff);
		return 0;
	}

	TCHAR* segments[128];
	size_t count = DSUsb_SplitNullTerminated(segments, _MaxListLen, buff, MAX_PATH * 128);
	size_t num=0;
	for (size_t i = 0; i < count; i++)
	{
		DSUsb_DeviceInstanceIdToVidPid(segments[i], &_DeviceList[num].VID_PID);
		if (DSUsb_VidPidIncludes(_FilterList, _FilterLen, _DeviceList[num].VID_PID))
		{
			lstrcpy(_DeviceList[num].InstanceId, segments[i]);
			num++;
		}
	}
	free(buff);
	return num;
}

void WINAPI DSUsb_GetDevicePropertys(DS_USB_DEVICE* _Device) {
	ULONG buffSize;

	buffSize = MAX_PATH;
	CM_Get_Device_Interface_List((LPGUID) & GUID_DEVINTERFACE_USB_DEVICE, _Device->InstanceId, (PBYTE)_Device->Interface, buffSize, 0);

	CM_Locate_DevNode(&_Device->DevInst, _Device->InstanceId, CM_LOCATE_DEVNODE_CANCELREMOVE);

	buffSize = MAX_PATH;
	CM_Get_DevNode_Registry_Property(_Device->DevInst, CM_DRP_FRIENDLYNAME, NULL, (PBYTE)_Device->FriendlyName, &buffSize, 0);
//CM_DRP_iNTER
		buffSize = MAX_PATH;
	CM_Get_DevNode_Registry_Property(_Device->DevInst, CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (PBYTE)_Device->SymbolicName, &buffSize, 0);

}


 HCMNOTIFICATION WINAPI DSUsb_RegisterDeviceArrivalRemoval(PCM_NOTIFY_CALLBACK _Fun, void* _PContext)
{
    CM_NOTIFY_FILTER filter = { 0 };
    filter.cbSize = sizeof(CM_NOTIFY_FILTER);
    filter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE;
    filter.u.DeviceInterface.ClassGuid = GUID_DEVINTERFACE_USB_DEVICE;
    HCMNOTIFICATION hNotify;
    CM_Register_Notification(&filter, _PContext, _Fun, &hNotify);
    return hNotify;
}



 BOOL WINAPI DSUsb_DeviceInstanceIdToVidPid(TCHAR* _InstanceId, DS_USB_VID_PID* _VidPid)
{
    if (_stscanf_s(_InstanceId, TEXT("USB\\VID_%hx&PID_%hx"), &_VidPid->vid, &_VidPid->pid) == 2)
    {
        return TRUE;
    }
    return FALSE;
}

 BOOL WINAPI DSUsb_DeviceInterfaceToVidPid(TCHAR* _DeviceInterface, DS_USB_VID_PID* _VidPid)
{
    if (_stscanf_s(_DeviceInterface, TEXT("\\\\?\\USB#VID_%hx&PID_%hx"), &_VidPid->vid, &_VidPid->pid) == 2)
    {
        return TRUE;
    }
    return FALSE;
}

