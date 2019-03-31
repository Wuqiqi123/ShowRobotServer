#include "StdAfx.h"
#include "Joystick.h"

#pragma comment (lib,"dinput8.lib")
//�˲������Ա���_DirectInput8Create@20����
#pragma comment (lib,"dxguid.lib")


DWORD WINAPI GetJSDataThread(LPVOID);
CJoystick::CJoystick(void)
{
	m_lpDIDevice = NULL;
    m_lpDI = NULL;
    m_hWnd = NULL;
    m_hInstance = GetModuleHandle(NULL) ;//��ȡʵ�����
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
     //�ͷ�DI��DIDevice����
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
     //����DI8�ӿ�
    if(NULL == m_lpDI)
    {
        hr=DirectInput8Create(m_hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(VOID**)&m_lpDI,NULL);
		if FAILED(hr)
        {
			OutputDebugString(LPCWSTR("Create ʧ�� - in CDIJoystick::Initialise/n"));
			return false;
        }
     }
	//ö���豸										//�ص�����				//��ֵGUID
	hr = m_lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL,DIEnumDevicesCallback,&JoystickGUID,DIEDFL_ATTACHEDONLY);//ɨ�谲װ�õĺ����Ӻõ��豸
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("ö���豸ʧ�� - in CDIJoystick::Initialise/n"));
         return false;
     }
	 //����DI8�豸
     if(!m_lpDIDevice)
     {
         hr = m_lpDI->CreateDevice(JoystickGUID,&m_lpDIDevice,NULL);
         if FAILED(hr)
         {
              OutputDebugString(LPCWSTR("�����豸ʧ�� - in CDIJoystick::Initialise/n"));
              return false;
         }
     }
	 //����Э���ȼ����� ��̨ģʽ | ��ռģʽ
	 hr = m_lpDIDevice->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
    
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("����Э���ȼ�ʧ�� - in CDIJoystick::Initialise/n"));
         return false;
     }
	 //�������ݸ�ʽ
     hr = m_lpDIDevice->SetDataFormat( &c_dfDIJoystick);
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("�������ݸ�ʽʧ�� - in CDIJoystick::Initialise/n"));
         return false;
	 }
	 hr = m_lpDIDevice->EnumObjects(EnumObjectsCallback, (VOID*)this, DIDFT_ALL );
     if FAILED(hr)
     {
         OutputDebugString(LPCWSTR("ö�ٶ���ʧ�� - in CDIJoystick::Initialise/n"));
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
     CJoystick * js = (CJoystick*)pContext;    //����ȡ��JS����ָ��
     //������Ϸ����������
    if( pdidoi->dwType & DIDFT_AXIS )          //���ö�ٵĶ���Ϊ��
    {
        DIPROPRANGE diprg;                     //�����᷶Χ�ṹ
        diprg.diph.dwSize       = sizeof(DIPROPRANGE);
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        diprg.diph.dwHow        = DIPH_BYID;
        diprg.diph.dwObj        = pdidoi->dwType; // ö�ٵ���
        diprg.lMin              = -1024;           //��Сֵ
        diprg.lMax              = +1024;           //���ֵ
    // �����᷶Χ 
         hr = js->m_lpDIDevice->SetProperty( DIPROP_RANGE, &diprg.diph);
        if( FAILED(hr))
         {
         OutputDebugString(LPCWSTR("�����᷶Χʧ�� - in CDIJoystick::EnumObjectsCallback/n"));
         return DIENUM_STOP;
         }
//�����������ԣ������ʹ�õ��ǵ�ƽʽ����Ϸ�ֱ�����Ҫע�͵�һ�²���
         DIPROPDWORD dipdw;                        //�����ṹ
         dipdw.diph.dwSize       = sizeof( dipdw );
         dipdw.diph.dwHeaderSize = sizeof( dipdw.diph );
        diprg.diph.dwObj        = pdidoi->dwType; // ö�ٵ���
         dipdw.diph.dwHow        = DIPH_DEVICE;
         dipdw.dwData            = 1000;           //10%������
         hr =  js->m_lpDIDevice->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
        if( FAILED(hr))
         {
         OutputDebugString(LPCWSTR("��������ʧ�� - in CDIJoystick::EnumObjectsCallback/n"));
         return DIENUM_STOP;
         }
    }
    return DIENUM_CONTINUE;
}

HRESULT CJoystick::PollDevice(void)
{
   HRESULT     hr;
   if( NULL == m_lpDIDevice )    //δ����豸
        return S_OK;
        hr = m_lpDIDevice->Poll();// ��ѭ�豸��ȡ��ǰ״̬
    if( FAILED(hr) ) 
    {
        // �������жϣ�����ͨ����ѭ����κ�״ֵ̬��
        // ���Բ���Ҫ�κ����ã�ֻҪ�ٴλ���豸���С�
        //hr = m_lpDIDevice->Acquire();
        //while( hr == DIERR_INPUTLOST )
        // {
        //      static int iCount = 0;
        //      if (iCount>30) exit(-1);    //�ۻ�30�λ�ȡ�豸ʧ�ܣ��˳�����
        //      iCount++;
        //      OutputDebugString(LPCWSTR("��ʧ�豸����ѭʧ�� - in CJoystick::PollDevice/n"));
        //      hr = m_lpDIDevice->Acquire();
        //      if( SUCCEEDED(hr) ) iCount = 0; 
        // }        // hrҲ��Ϊ�����Ĵ���.
        //return S_OK;

		hr = m_lpDIDevice->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = m_lpDIDevice->Acquire();
		return S_OK;

    }
    // �������״̬���洢����Ա���� m_diJs ��
	if( FAILED( hr = m_lpDIDevice->GetDeviceState( sizeof(DIJOYSTATE), &m_diJs ) ) )
        return hr; // ����ѭ�������豸��Ϊ �ѻ�� ״̬

    //if( FAILED( hr = m_lpDIDevice->GetDeviceState( sizeof(DIJOYSTATE2), &m_JS ) ) )
    //    return hr; // ����ѭ�������豸��Ϊ �ѻ�� ״̬
    return S_OK;
}

void CJoystick::Startlisten()
{
	if (hThread == NULL)
	{
		hThread = CreateThread(NULL, 0, GetJSDataThread, this, 0, NULL);
	}
		
}

HANDLE g_hMutexForJS;  //���������

DWORD WINAPI GetJSDataThread(LPVOID p)
{
	g_hMutexForJS = CreateMutex(NULL, FALSE, NULL);   //���������Ļ���������������������κ��߳�ռ��
	CJoystick* JS  = (CJoystick*)p;
	while (true)
	{
		if (FAILED(JS->PollDevice())) //��ѭ
		{
			AfxMessageBox(_T("��ȡ�豸״̬����"), MB_OK);
			return 0;
		}
		TCHAR strText[512] = { 0 }; // Device state text
		WaitForSingleObject(g_hMutexForJS, INFINITE);    //ʹ�û�����������g_QueueData���ж�ȡ�Ͳ���ֿ�
		JS->innerJSForceData.x = (JS->m_diJs.lX - 32768) * 1000 / 32768;
		JS->innerJSForceData.y = (JS->m_diJs.lY - 32768) * 1000 / 32768;
		JS->innerJSForceData.z = (JS->m_diJs.lZ - 32768) * 1000 / 32768;
		JS->innerJSForceData.R = (JS->m_diJs.lRz ) ;
		ReleaseMutex(g_hMutexForJS);
		Sleep(10);
	}

	return 0;
}