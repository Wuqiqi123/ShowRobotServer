#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <InitGuid.h>
struct JSForceData
{
	long x;
	long y;
	long z;
	long R;
};

class CJoystick
{
public:
	CJoystick(void);
	virtual ~CJoystick(void);
	//һ��ĳ�Ա����
     HINSTANCE m_hInstance;               // ʵ�����
     HWND m_hWnd;                         // ���ھ��
     LPDIRECTINPUT8  m_lpDI;              // DI8�ӿ�ָ��
     LPDIRECTINPUTDEVICE8  m_lpDIDevice;  // DIDevice8�ӿ�ָ��
     DIJOYSTATE   m_diJs;                  //�洢Joystick״̬��Ϣ
	 DIJOYSTATE2  m_JS;  
     GUID JoystickGUID;                   //GUID
	 //��Ա����
     bool Initialise(void);               //��ʼ������
     //ö���豸
     static BOOL CALLBACK DIEnumDevicesCallback(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);      //ö�ٶ���
     static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
     HRESULT PollDevice(void);        // ��ѭ�豸����������ѭ�����ж�ȡ�豸״̬

	 HANDLE hThread;
	 void Startlisten(void);
	 JSForceData innerJSForceData;
	 double bias[4];  //�ϵ��ʼ��֮��ȡֵ��ƫ��ֵ��ϣ����ƫ��ֵ��������֤ÿ�����ϵ��ʱ�����������0
	 double offset[4]; //���������ʱ����������һЩ�������������������ֵ֮�ڶ�Ϊ0��

};

