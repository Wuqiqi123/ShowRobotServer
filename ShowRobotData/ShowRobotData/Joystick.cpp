#include "StdAfx.h"
#include "Joystick.h"
#include "ShowRobotDataDlg.h"

#pragma comment (lib,"dinput8.lib")
//此操作可以避免_DirectInput8Create@20错误
#pragma comment (lib,"dxguid.lib")

#define JSBIASNUM 5

DWORD WINAPI GetJSDataThread(LPVOID);
CJoystick::CJoystick(void)
{
	m_lpDIDevice = NULL;
    m_lpDI = NULL;
    m_hWnd = NULL;
    m_hInstance = GetModuleHandle(NULL) ;//获取实例句柄
	hThread = NULL;
	innerJSForceData.x = 0;
	innerJSForceData.y = 0;
	innerJSForceData.z = 0;
	innerJSForceData.R = 0;
	for (int i = 0; i < 4; i++)
	{
		bias[i] = 0;
		offset[i] = 0.5;
	}
}

CJoystick::~CJoystick(void)
{
	 if(m_lpDIDevice)
          m_lpDIDevice->Unacquire();
     //释放DI和DIDevice对象
     if(m_lpDIDevice)
     {
         m_lpDIDevice->Release();
         m_lpDIDevice = NULL;  
     }
     if(m_lpDI)
     {
         m_lpDI->Release();
         m_lpDI = NULL;    
     }
	 if (hThread)
	 {
		 CloseHandle(hThread);
		 hThread = NULL;
	 }
}

bool CJoystick::Initialise(void)
{
	HRESULT hr;
     //建立DI8接口
    if(NULL == m_lpDI)
    {
        hr=DirectInput8Create(m_hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(VOID**)&m_lpDI,NULL);
		if FAILED(hr)
        {
			OutputDebugString(LPCWSTR("Create 失败 - in CDIJoystick::Initialise/n"));
			return false;
        }
     }
	//枚举设备										//回调函数				//赋值GUID
	hr = m_lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL,DIEnumDevicesCallback,&JoystickGUID,DIEDFL_ATTACHEDONLY);//扫描安装好的和连接好的设备
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("枚举设备失败 - in CDIJoystick::Initialise/n"));
         return false;
     }
	 //创建DI8设备
     if(!m_lpDIDevice)
     {
         hr = m_lpDI->CreateDevice(JoystickGUID,&m_lpDIDevice,NULL);
         if FAILED(hr)
         {
              OutputDebugString(LPCWSTR("创建设备失败 - in CDIJoystick::Initialise/n"));
              return false;
         }
     }
	 //设置协作等级—— 后台模式 | 独占模式
	 hr = m_lpDIDevice->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("设置协作等级失败 - in CDIJoystick::Initialise/n"));
         return false;
     }
	 //设置数据格式
     hr = m_lpDIDevice->SetDataFormat( &c_dfDIJoystick);
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("设置数据格式失败 - in CDIJoystick::Initialise/n"));
         return false;
	 }
	 hr = m_lpDIDevice->EnumObjects(EnumObjectsCallback, (VOID*)this, DIDFT_ALL );
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("枚举对象失败 - in CDIJoystick::Initialise/n"));
         return false;
	 }
	 Sleep(200);
	 ////////////////////////////////////////////////////
	 /*下面开始计算初始的偏置值*/
	 double biastmp[4] = {0,0,0,0};
	 for (int i = 0; i < JSBIASNUM+1;i++)
	 {
		 if (FAILED(PollDevice())) //轮循
		 {
			 AfxMessageBox(_T("读取设备状态错误"), MB_OK);
			 return 0;
		 }
		 if (i > 0)
		 {
			 biastmp[0] += m_diJs.lX;
			 biastmp[1] += m_diJs.lY;
			 biastmp[2] += m_diJs.lZ;
			 biastmp[3] += m_diJs.lRz;
		 }
		 Sleep(10);
	 }
	 for (int i = 0; i < 4; i++)
	 {
		 bias[i] = biastmp[i] / (JSBIASNUM);
	 }
	 
		 
	return 1;
}

BOOL CALLBACK CJoystick::DIEnumDevicesCallback(const DIDEVICEINSTANCE* lpddi, VOID* pvRef)
{
     *(GUID*) pvRef = lpddi->guidInstance;
    return DIENUM_STOP;  
}

BOOL CALLBACK CJoystick::EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
     HRESULT hr;
     CJoystick * js = (CJoystick*)pContext;    //首先取得JS对象指针
     //设置游戏杆输入特性
    if( pdidoi->dwType & DIDFT_AXIS )          //如果枚举的对象为轴
    {
        DIPROPRANGE diprg;                     //设置轴范围结构
        diprg.diph.dwSize       = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow        = DIPH_BYID;
        diprg.diph.dwObj        = pdidoi->dwType; // 枚举的轴
        diprg.lMin              = -1024;           //最小值
        diprg.lMax              = +1024;           //最大值
    // 设置轴范围 
         hr = js->m_lpDIDevice->SetProperty( DIPROP_RANGE, &diprg.diph);
        if( FAILED(hr))
         {
			OutputDebugString(LPCWSTR("设置轴范围失败 - in CDIJoystick::EnumObjectsCallback/n"));
			return DIENUM_STOP;
         }
//设置死区属性，如果你使用的是电平式的游戏手柄，需要注释掉一下部分
         DIPROPDWORD dipdw;                        //死区结构
         dipdw.diph.dwSize       = sizeof( dipdw );
         dipdw.diph.dwHeaderSize = sizeof( dipdw.diph );
         diprg.diph.dwObj        = pdidoi->dwType; // 枚举的轴
         dipdw.diph.dwHow        = DIPH_DEVICE;
         dipdw.dwData            = 1000;           //10%的死区
         hr =  js->m_lpDIDevice->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
        if( FAILED(hr))
         {
			 OutputDebugString(LPCWSTR("设置死区失败 - in CDIJoystick::EnumObjectsCallback/n"));
			 return DIENUM_STOP;
         }
    }
    return DIENUM_CONTINUE;
}

HRESULT CJoystick::PollDevice(void)
{
   HRESULT     hr;
   if( NULL == m_lpDIDevice )    //未获得设备
        return S_OK;
        hr = m_lpDIDevice->Poll();// 轮循设备读取当前状态
    if( FAILED(hr) ) 
    {
        // 输入流中断，不能通过轮循获得任何状态值。
        // 所以不需要任何重置，只要再次获得设备就行。
        //hr = m_lpDIDevice->Acquire();
        //while( hr == DIERR_INPUTLOST )
        // {
        //      static int iCount = 0;
        //      if (iCount>30) exit(-1);    //累积30次获取设备失败，退出程序。
        //      iCount++;
        //      OutputDebugString(LPCWSTR("丢失设备，轮循失败 - in CJoystick::PollDevice/n"));
        //      hr = m_lpDIDevice->Acquire();
        //      if( SUCCEEDED(hr) ) iCount = 0; 
        // }        // hr也许为其他的错误.
        //return S_OK;

		hr = m_lpDIDevice->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = m_lpDIDevice->Acquire();
		return S_OK;

    }
    // 获得输入状态，存储到成员变量 m_diJs 中
	if( FAILED( hr = m_lpDIDevice->GetDeviceState( sizeof(DIJOYSTATE), &m_diJs ) ) )
        return hr; // 在轮循过程中设备将为 已获得 状态

    //if( FAILED( hr = m_lpDIDevice->GetDeviceState( sizeof(DIJOYSTATE2), &m_JS ) ) )
    //    return hr; // 在轮循过程中设备将为 已获得 状态
    return S_OK;
}

void CJoystick::Startlisten()
{
	if (hThread == NULL)
	{
		hThread = CreateThread(NULL, 0, GetJSDataThread, this, 0, NULL);
	}
		
}

HANDLE g_hMutexForJS;  //互斥量句柄
extern RobotData HelixRobotData;
extern HANDLE g_ThreadSema;  //创建内核对象，用来初始化信号量
extern bool isjoystickNULL;

/*
初始原位  32255  34328   33288   74
向前位置  32511  4096    33288   155
向后位置  37189  65015   33288   -24
向左位置  5876   28927   33288   -8
向右位置  65535  32255   33288   204
右旋      33028  32768   33288   1024
左旋      37969  37969   33288   -1000
32768
*/
 
DWORD WINAPI GetJSDataThread(LPVOID p)
{
	g_hMutexForJS = CreateMutex(NULL, FALSE, NULL);   //创建无名的互斥量，这个互斥量不被任何线程占有
	CJoystick* JS  = (CJoystick*)p;
	while (JS!=NULL)
	{
		WaitForSingleObject(g_hMutexForJS, INFINITE);    //保护不能在使用这些值的时候释放该对象
		if (isjoystickNULL == true)
			break;
		if (FAILED(JS->PollDevice())) //轮循
		{
			AfxMessageBox(_T("读取设备状态错误"), MB_OK);
			return 0;
		}

		HelixRobotData.Origin6axisForce[1] = JS->innerJSForceData.y = (JS->m_diJs.lX - JS->bias[0]) * 20 / JS->bias[0];  //转化到-20~20
		HelixRobotData.Origin6axisForce[0] = JS->innerJSForceData.x = (JS->m_diJs.lY - JS->bias[1]) * 20 / JS->bias[1]; //转化到-20~20
		HelixRobotData.Origin6axisForce[2] = JS->innerJSForceData.z = -(JS->m_diJs.lZ - JS->bias[2]) * 20 / JS->bias[2]; //转化到-20~20
		HelixRobotData.Origin6axisForce[5] = JS->innerJSForceData.R = -(JS->m_diJs.lRz - JS->bias[3]) * 10 / JS->bias[3]; //转化到-10~10
		if (abs(HelixRobotData.Origin6axisForce[0])<JS->offset[0])
		{
			HelixRobotData.Origin6axisForce[0] = 0;
		}
		if (abs(HelixRobotData.Origin6axisForce[1])<JS->offset[1])
		{
			HelixRobotData.Origin6axisForce[1] = 0;
		}
		if (abs(HelixRobotData.Origin6axisForce[2])<JS->offset[2])
		{
			HelixRobotData.Origin6axisForce[2] = 0;
		}
		if (abs(HelixRobotData.Origin6axisForce[3])<JS->offset[3])
		{
			HelixRobotData.Origin6axisForce[3] = 0;
		}
		ReleaseMutex(g_hMutexForJS);
		ReleaseSemaphore(g_ThreadSema, 1, NULL);  //信号量资源数加一
		Sleep(10);
	}

	return 0;
}

