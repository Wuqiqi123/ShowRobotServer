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
	//一般的成员变量
     HINSTANCE m_hInstance;               // 实例句柄
     HWND m_hWnd;                         // 窗口句柄
     LPDIRECTINPUT8  m_lpDI;              // DI8接口指针
     LPDIRECTINPUTDEVICE8  m_lpDIDevice;  // DIDevice8接口指针
     DIJOYSTATE   m_diJs;                  //存储Joystick状态信息
	 DIJOYSTATE2  m_JS;  
     GUID JoystickGUID;                   //GUID
	 //成员函数
     bool Initialise(void);               //初始化函数
     //枚举设备
     static BOOL CALLBACK DIEnumDevicesCallback(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);      //枚举对象
     static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
     HRESULT PollDevice(void);        // 轮循设备函数，在轮循过程中读取设备状态

	 HANDLE hThread;
	 void Startlisten(void);
	 JSForceData innerJSForceData;
};

