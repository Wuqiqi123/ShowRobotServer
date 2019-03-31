#include "StdAfx.h"
#include "Joystick.h"

#pragma comment (lib,"dinput8.lib")
//此操作可以避免_DirectInput8Create@20错误
#pragma comment (lib,"dxguid.lib")


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

DWORD WINAPI GetJSDataThread(LPVOID p)
{
	g_hMutexForJS = CreateMutex(NULL, FALSE, NULL);   //创建无名的互斥量，这个互斥量不被任何线程占有
	CJoystick* JS  = (CJoystick*)p;
	while (true)
	{
		if (FAILED(JS->PollDevice())) //轮循
		{
			AfxMessageBox(_T("读取设备状态错误"), MB_OK);
			return 0;
		}
		TCHAR strText[512] = { 0 }; // Device state text
		WaitForSingleObject(g_hMutexForJS, INFINITE);    //使用互斥量来保护g_QueueData队列读取和插入分开
		JS->innerJSForceData.x = (JS->m_diJs.lX - 32768) * 1000 / 32768;
		JS->innerJSForceData.y = (JS->m_diJs.lY - 32768) * 1000 / 32768;
		JS->innerJSForceData.z = (JS->m_diJs.lZ - 32768) * 1000 / 32768;
		JS->innerJSForceData.R = (JS->m_diJs.lRz ) ;
		ReleaseMutex(g_hMutexForJS);
		Sleep(10);
	}

	return 0;
}