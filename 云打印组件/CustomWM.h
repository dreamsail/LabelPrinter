#pragma once
#ifndef CUSTOM_WM_H
#define CUSTOM_WM_H


#define WM_RUN_STATE				WM_USER		+1000   //��ѯ����״̬�������ظ����г���
/*   */ #define RUN_STATE__RUN		0xFFFF8800          //����ֻ�����״̬ �޷��ؾ���δ���У����ش�����ǷǱ�����
#define WM_USB_DEVICE_NOTIFY        WM_USER     +1001	//usb�豸�Ȳ����Ϣ
#define WM_ADD_LABEL                WM_USER     +1002   //��ӱ�ǩ
#define WM_PRINT_DEVICE_STATE		WM_USER     +1003	//���ʹ�ӡ״̬
#define WM_SYSTEM_NOTIFY			WM_USER     +1004	//ϵͳ������Ϣ

#endif // ! CUSTOM_WM_H