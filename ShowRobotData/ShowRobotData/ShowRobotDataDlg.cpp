
// ShowRobotDataDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ShowRobotData.h"
#include "ShowRobotDataDlg.h"
#include "afxdialogex.h"
#include <queue>
#include <fstream>

#define AUTOSTOPTIME 2000    //��λ�Ǻ���
#define TIMEINTERVAL 150    //��λ�Ǻ���
#define AUTODECRNUM (AUTOSTOPTIME/TIMEINTERVAL) 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
#define UM_DRAWROBOTDATA    WM_USER+100    //�Զ���һ����Ϣ
UINT server_thd(LPVOID p);   //�����̺߳���
CString IP;  //����Ϊȫ�ֱ���
SOCKET listen_sock;
//SOCKET sockforreality;
//SOCKET sockforHelix;

DWORD WINAPI ServerThreadForReality(LPVOID lp);
DWORD WINAPI ServerThreadForHelix(LPVOID lp);

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CShowRobotDataDlg �Ի���



CShowRobotDataDlg::CShowRobotDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowRobotDataDlg::IDD, pParent)
	, m_autodecrese(FALSE)
{
	m_pShowForceDlg = NULL;
	m_autodecrese = false;
	for (int i = 0; i < 6; i++)
	{
		bIsMouseDown[i] = false;
		ForceSense[i] = 0;
		keystopflag[i] = true;
		alive[i] = 0;
		checkalive[i] = 0;
	}
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CShowRobotDataDlg::~CShowRobotDataDlg()
{
	if (m_pShowForceDlg != NULL)
	{
		delete m_pShowForceDlg;
		m_pShowForceDlg = NULL;
	}
}

void CShowRobotDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ChartCtrl1, m_ChartCtrl1);
	DDX_Control(pDX, IDC_ChartCtrl2, m_ChartCtrl2);
	DDX_Control(pDX, IDC_ChartCtrl3, m_ChartCtrl3);
	DDX_Control(pDX, IDC_ChartCtrl4, m_ChartCtrl4);
	DDX_Control(pDX, IDC_LIST1_SHOWMESSAGE, m_recvMessage);
	DDX_Control(pDX, IDC_EDIT2_TCPPort, m_TCPPort);
	DDX_Control(pDX, IDC_IPADDRESS1, m_TCPIPaddr);
	DDX_Radio(pDX, IDC_RADIO1, m_autodecrese);
	DDX_Control(pDX, IDC_EDIT3, m_NumberEdit);
}

BEGIN_MESSAGE_MAP(CShowRobotDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTSERVER, &CShowRobotDataDlg::OnBnClickedStartserver)
	ON_LBN_SELCHANGE(IDC_LIST1_SHOWMESSAGE, &CShowRobotDataDlg::OnLbnSelchangeList1Showmessage)
	ON_MESSAGE(UM_DRAWROBOTDATA, &CShowRobotDataDlg::OnDrawRobotData)
	ON_MESSAGE(WM_CLOSECHILDDLG, &CShowRobotDataDlg::OnCloseChildDlgMessage)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RADIO2, &CShowRobotDataDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO1, &CShowRobotDataDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_BUTTON2, &CShowRobotDataDlg::OnBnClickedButton2)
	//ON_MESSAGE(WM_CLOSECHILDDLG, &CShowRobotDataDlg::OnCloseChildDlgMessage)
	//ON_STN_CLICKED(IDC_STATIC_F_FACTOR, &CShowRobotDataDlg::OnStnClickedStaticFFactor)
END_MESSAGE_MAP()


// CShowRobotDataDlg ��Ϣ�������

BOOL CShowRobotDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CChartAxis *pAxis = NULL;
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl1.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl2.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl3.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-120, 120);

	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxis->SetAutomatic(true);
	pAxis = m_ChartCtrl4.CreateStandardAxis(CChartCtrl::LeftAxis);
	//pAxis->SetAutomatic(true);
	pAxis->SetMinMax(-360, 360);
	///////��������
	TChartString str1;

	str1 = _T("#1�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl1.GetTitle()->AddString(str1);
	str1 = _T("��������");
	CChartAxisLabel *pLabel = m_ChartCtrl1.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl1.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#2�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl2.GetTitle()->AddString(str1);
	str1 = _T("��������");
	pLabel = m_ChartCtrl2.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl2.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#3�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl3.GetTitle()->AddString(str1);
	str1 = _T("��������");
	pLabel = m_ChartCtrl3.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl3.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	str1 = _T("#4�ĽǶȣ��ٶȣ�����");
	m_ChartCtrl4.GetTitle()->AddString(str1);
	str1 = _T("��������");
	pLabel = m_ChartCtrl4.GetBottomAxis()->GetLabel();
	pLabel->SetText(str1);
	str1 = _T("λ��deg���ٶ�deg/s������N��m");
	pLabel = m_ChartCtrl4.GetLeftAxis()->GetLabel();
	pLabel->SetText(str1);

	for (int i = 0; i < 3; i++)
	{
		m_pLineSerie1[i] = m_ChartCtrl1.CreateLineSerie();
		m_pLineSerie2[i] = m_ChartCtrl2.CreateLineSerie();
		m_pLineSerie3[i] = m_ChartCtrl3.CreateLineSerie();
		m_pLineSerie4[i] = m_ChartCtrl4.CreateLineSerie();
	}
	/////////////////////////////////////////////////
	SetTimer(1, 150, NULL);  //���ö�ʱ������ʱ����Ϊ100ms
	if (!m_autodecrese)
		SetTimer(2, 200, NULL);  //���ö�ʱ������ʱ����Ϊ300ms


	fileopenflag = false;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CShowRobotDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CShowRobotDataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CShowRobotDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CString2Char(CString str, char ch[])//�˺��������ַ�ת��������ʵ�ִ���
{
	int i;
	char *tmpch;
	int wLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);//�õ�Char�ĳ���
	tmpch = new char[wLen + 1];                                             //��������ĵ�ַ��С
	WideCharToMultiByte(CP_ACP, 0, str, -1, tmpch, wLen, NULL, NULL);       //��CStringת����char*


	for (i = 0; tmpch[i] != '\0'; i++) ch[i] = tmpch[i];
	ch[i] = '\0';
}

std::ofstream oFile;
void CShowRobotDataDlg::OnBnClickedStartserver()
{
	//�ں���֮ǰ��ӱ���Excel���ļ�   
	oFile.open("Robotdata.csv",std::ios::out|std::ios::trunc);  //���ģʽ������ļ��Ѿ����ھ�ɾ������ļ�
	oFile << "���ڵĽǶ�[0]" << "," << "���ڵ��ٶ�[0]" << "," << "���ڵ�����[0]" << "," << "�¸�ʱ�̵ĽǶ�[0]" << "," << "�¸�ʱ�̵��ٶ�[0]" << ",";
	oFile << "���ڵĽǶ�[1]" << "," << "���ڵ��ٶ�[1]" << "," << "���ڵ�����[1]" << "," << "�¸�ʱ�̵ĽǶ�[1]" << "," << "�¸�ʱ�̵��ٶ�[1]" << ",";
	oFile << "���ڵĽǶ�[2]" << "," << "���ڵ��ٶ�[2]" << "," << "���ڵ�����[2]" << "," << "�¸�ʱ�̵ĽǶ�[2]" << "," << "�¸�ʱ�̵��ٶ�[2]" << ",";
	oFile << "���ڵĽǶ�[3]" << "," << "���ڵ��ٶ�[3]" << "," << "���ڵ�����[3]" << "," << "�¸�ʱ�̵ĽǶ�[3]" << "," << "�¸�ʱ�̵��ٶ�[3]";
	oFile << std::endl;
	fileopenflag = true;
 	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);   //�����׽��ֿ�2.0�汾
	if (err!=0)
	{
		update(_T("�����׽��ֿ�2.0ʧ��"));
	}
	send_edit = (CEdit *)GetDlgItem(IDC_EDIT1);
	send_edit->SetFocus();
	char name[128];
	hostent* pHost;
	gethostname(name, 128);//��������� 
	pHost = gethostbyname(name);//��������ṹ 
	IP = inet_ntoa(*(struct in_addr*)pHost->h_addr);    //��ȡ������IP��ַ
	DWORD dwIP;
	char ch_ip1[20];
	CString2Char(IP, ch_ip1);//ע�⣡����������ַ���ʽת���������˺������ܣ�CString����ת��ΪChar���ͣ�ʵ�ִ����ں������
	dwIP = inet_addr(ch_ip1);
	unsigned char *pIP = (unsigned char*)&dwIP;
	m_TCPIPaddr.SetAddress(*(pIP), *(pIP + 1), *(pIP + 2), *(pIP+3));
	
	update(_T("��������IP��ַ��") + IP);

	/////////////////////�����ǻ�ͼ����ĳ�ʼ��
	for (int i = 0; i < 3; i++)
	{
		ZeroMemory(&m_HightSpeedChartArray1[i], sizeof(double)*m_c_arrayLength);
		ZeroMemory(&m_HightSpeedChartArray2[i], sizeof(double)*m_c_arrayLength);
		ZeroMemory(&m_HightSpeedChartArray3[i], sizeof(double)*m_c_arrayLength);
		ZeroMemory(&m_HightSpeedChartArray4[i], sizeof(double)*m_c_arrayLength);
	}

	for (int i = 0; i < 3; i++)
	{
		m_pLineSerie1[i]->SetSeriesOrdering(poNoOrdering);
		m_pLineSerie2[i]->SetSeriesOrdering(poNoOrdering);
		m_pLineSerie3[i]->SetSeriesOrdering(poNoOrdering);
		m_pLineSerie4[i]->SetSeriesOrdering(poNoOrdering);
	}
	for (size_t i = 0; i<m_c_arrayLength; ++i)
	{
		m_X[i] = i;   //X��Ĳ�����ʼ��
	}
	m_count = m_c_arrayLength;
	for (int i = 0; i < 3; i++)
	{
		m_pLineSerie1[i]->ClearSerie();
		m_pLineSerie2[i]->ClearSerie();
		m_pLineSerie3[i]->ClearSerie();
		m_pLineSerie4[i]->ClearSerie();
	}
	TChartStringStream strs1, strs2, strs3;
	strs1 << _T("�Ƕ�ֵ");
		m_pLineSerie1[0]->SetName(strs1.str());//SetName�����ý��ں��潲��
		m_pLineSerie2[0]->SetName(strs1.str());
		m_pLineSerie3[0]->SetName(strs1.str());
		m_pLineSerie4[0]->SetName(strs1.str());
	strs2 << _T("�ٶ�ֵ");
		m_pLineSerie1[1]->SetName(strs2.str());//SetName�����ý��ں��潲��
		m_pLineSerie2[1]->SetName(strs2.str());
		m_pLineSerie3[1]->SetName(strs2.str());
		m_pLineSerie4[1]->SetName(strs2.str());
	strs3 << _T("����");
		m_pLineSerie1[2]->SetName(strs3.str());//SetName�����ý��ں��潲��
		m_pLineSerie2[2]->SetName(strs3.str());
		m_pLineSerie3[2]->SetName(strs3.str());
		m_pLineSerie4[2]->SetName(strs3.str());
    m_ChartCtrl1.GetLegend()->SetVisible(true);
	m_ChartCtrl2.GetLegend()->SetVisible(true);
	m_ChartCtrl3.GetLegend()->SetVisible(true);
	m_ChartCtrl4.GetLegend()->SetVisible(true);
	///////////////////��ͼ����ĳ�ʼ������
	AfxBeginThread(server_thd, (LPVOID)GetSafeHwnd(), THREAD_PRIORITY_HIGHEST);//�����߳�
}


void CShowRobotDataDlg::OnLbnSelchangeList1Showmessage()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

//��list���ܿ��д�ӡ��Ϣ
void CShowRobotDataDlg::update(CString s)
{
	m_recvMessage.AddString(s);
}

/// \brief ��������  
/// \param ptr ����ָ��  
/// \param data ����ֵ  
/// 
void LeftMoveArray(double* ptr, size_t length, double data)
{
	for (size_t i = 1; i<length; ++i)
	{
		ptr[i - 1] = ptr[i];
	}
	ptr[length - 1] = data;
}
/////�������飬����sizeλ
void LeftMoveArrayWithQueue(double* ptr, size_t length, RobotData DealQueueData[],int size,int WhichData)
{
	for (size_t i = 0; i<(length-size); ++i)
	{
		ptr[i] = ptr[size+i];
	}
	switch (WhichData)
	{
	case 11:   //��һ����ĽǶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[0];
		}
		break;
	case 12:   //��һ������ٶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[0];
		}
		break;
	case 13:   //��һ�����������Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[0];
		}
		break;
	case 21:   //�ڶ�����ĽǶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[1];
		}
		break;
	case 22:   //�ڶ�������ٶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[1];
		}
		break;
	case 23:   //�ڶ������������Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[1];
		}
		break;
	case 31:   //��������ĽǶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[2];
		}
		break;
	case 32:   //��������ĽǶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[2];
		}
		break;
	case 33:   //���������������Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[2];
		}
		break;
	case 41:   //���ĸ���ĽǶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsNow[3];
		}
		break;
	case 42:   //���ĸ���ĽǶ���Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsVelNow[3];
		}
		break;
	case 43:   //���ĸ����������Ϣ
		for (int i = 0; i < size; i++)
		{
			ptr[length - size + i] = DealQueueData[i].JointsTorque[3];
		}
		break;
	}


}

/////X�������ƫ��
void LeftMoveArrayXWithQueue(double* ptr, size_t length, unsigned int& data, int size)
{
	for (size_t i = 0; i<(length-size); ++i)
	{
		ptr[i] = ptr[size+i];
	}
	for (int i = 0; i < size; i++)
	{
		ptr[length - size + i] = ++data;
	}
}


std::queue<RobotData> g_QueueData;  //ȫ�ֶ���
HANDLE g_hMutex;  //���������
UINT server_thd(LPVOID p)//�߳�Ҫ���õĺ���
{
	HWND hWnd = (HWND)p;
	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	CShowRobotDataDlg * dlg = (CShowRobotDataDlg *)AfxGetApp()->GetMainWnd();
	char ch_ip[20];
	CString2Char(IP, ch_ip);//ע�⣡����������ַ���ʽת���������˺������ܣ�CString����ת��ΪChar���ͣ�ʵ�ִ����ں������
	//local_addr.sin_addr.s_addr = htonl(INADDR_ANY);//��ȡ����IP��ַ
	local_addr.sin_addr.s_addr = inet_addr(ch_ip);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(8888);   //�˿ں�Ϊ8888
	dlg->SetDlgItemText(IDC_EDIT2_TCPPort, _T("8888"));     //д��

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)//�����׽���
	{
		dlg->update(_T("���������׽���ʧ��"));
	}
	if (bind(listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))//���׽���
	{
		dlg->update(_T("�󶨴���"));
	}

    listen(listen_sock, 2);     //��ʼ����,����ʹ�õ�������ģʽ

	//����һ������
	//WinExec("F:\\HelixShowRobot\\HelixSCARA\\HelixSCARA\\bin\\Debug\\HelixSCARA.exe",SW_SHOW);
	while (1)
	{	
		SOCKET *ClientSocket = new SOCKET;
		if ((*ClientSocket = accept(listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)//�����׽���
		{
			dlg->update(_T("accept ʧ��"));
		}
		else
		{
			CString port;
			port.Format(_T("%d"), int(ntohs(client_addr.sin_port)));
			CString IP;
			CString HelixIP("127.0.0.1");
			IP = CString(inet_ntoa(client_addr.sin_addr));
			dlg->update(_T("�����ӿͻ��ˣ�") + IP + "  �˿ڣ�" + port);
			if (IP == HelixIP)    //�����ϵ�helixͨ��
			{
				CreateThread(NULL, 0, ServerThreadForHelix, ClientSocket, 0, NULL);
			}
			else
			{
				CreateThread(NULL, 0, ServerThreadForReality, ClientSocket, 0, NULL);
			}

		}
	}
	
	////////////��������
	//g_hMutex = CreateMutex(NULL, FALSE, NULL);   //���������Ļ���������������������κ��߳�ռ��
	//RobotData MyRobotData;
	//char recbuf[sizeof(MyRobotData)];
	//memset(recbuf, 0, sizeof(MyRobotData));
	//while (1)
	//{		
	//	if ((res = recv(sock, recbuf, sizeof(recbuf), 0)) == -1)    //���rev����Ҳ������ģʽ
	//	{
	//		dlg->update(_T("ʧȥ�ͻ��˵�����"));
	//		break;
	//	}
	//	else
	//	{
	//		memset(&MyRobotData, 0, sizeof(MyRobotData));
	//		memcpy(&MyRobotData, recbuf, sizeof(MyRobotData));

	//		//////��������ļ�Ϊexcel
	//		oFile.precision(6); //���þ���
	//		//���ִ����������һЩ���㣬����Щ�۵��޳���
	//		if (MyRobotData.JointsNow[0] <= 1000 && MyRobotData.JointsNow[0] >= -1000 && MyRobotData.JointsVelNow[0] <= 100 && MyRobotData.JointsVelNow[0] >= -100)
	//		{
	//			oFile << MyRobotData.JointsNow[0] << "," << MyRobotData.JointsVelNow[0] << "," << MyRobotData.JointsTorque[0] << "," << MyRobotData.JointsNext[0] << "," << MyRobotData.JointsVelNext[0] << ",";
	//			oFile << MyRobotData.JointsNow[1] << "," << MyRobotData.JointsVelNow[1] << "," << MyRobotData.JointsTorque[1] << "," << MyRobotData.JointsNext[1] << "," << MyRobotData.JointsVelNext[1] << ",";
	//			oFile << MyRobotData.JointsNow[2] << "," << MyRobotData.JointsVelNow[2] << "," << MyRobotData.JointsTorque[2] << "," << MyRobotData.JointsNext[2] << "," << MyRobotData.JointsVelNext[2] << ",";
	//			oFile << MyRobotData.JointsNow[3] << "," << MyRobotData.JointsVelNow[3] << "," << MyRobotData.JointsTorque[3] << "," << MyRobotData.JointsNext[3] << "," << MyRobotData.JointsVelNext[3];
	//			oFile << std::endl;
	//		}

	//		//////��������ļ�Ϊexcel����

	//		//****ʹ�ö��еķ�ʽ���������			
	//		WaitForSingleObject(g_hMutex, INFINITE);    //ʹ�û�����������g_QueueData���ж�ȡ�Ͳ���ֿ�
	//		if (MyRobotData.JointsNow[0] <= 1000 && MyRobotData.JointsNow[0] >= -1000 && MyRobotData.JointsVelNow[0] <= 100 && MyRobotData.JointsVelNow[0] >= -100)
	//		{
	//			g_QueueData.push(MyRobotData);
	//		}
	//		ReleaseMutex(g_hMutex);
	//		//****ʹ�ö��еķ�ʽ������������

	//		////******ʹ�÷�����Ϣ�ķ�ʽ���������
	//		//::PostMessageA(hWnd,UM_DRAWROBOTDATA,(WPARAM)&MyRobotData,1);
	//		////******ʹ�÷�����Ϣ�ķ�ʽ������������
	//	}
	//}
	WSACleanup();
	return 0;
}

DWORD WINAPI ServerThreadForReality(LPVOID lp)
{
	SOCKET *ClientSocketRea = (SOCKET*)lp;
	////////////��������
	g_hMutex = CreateMutex(NULL, FALSE, NULL);   //���������Ļ���������������������κ��߳�ռ��
	RobotData MyRobotData;
	char recbuf[sizeof(MyRobotData)];
	int res;
	CShowRobotDataDlg * dlg = (CShowRobotDataDlg *)AfxGetApp()->GetMainWnd();
	memset(recbuf, 0, sizeof(MyRobotData));
	while (1)
	{
		if ((res = recv(*ClientSocketRea, recbuf, sizeof(recbuf), 0)) == -1)    //���rev����Ҳ������ģʽ
		{
			dlg->update(_T("ʧȥ�ͻ��˵�����"));
			break;
		}
		else
		{
			memset(&MyRobotData, 0, sizeof(MyRobotData));
			memcpy(&MyRobotData, recbuf, sizeof(MyRobotData));

			//////��������ļ�Ϊexcel
			oFile.precision(6); //���þ���
			//���ִ����������һЩ���㣬����Щ�۵��޳���
			if (MyRobotData.JointsNow[0] <= 1000 && MyRobotData.JointsNow[0] >= -1000 && MyRobotData.JointsVelNow[0] <= 100 && MyRobotData.JointsVelNow[0] >= -100)
			{
				oFile << MyRobotData.JointsNow[0] << "," << MyRobotData.JointsVelNow[0] << "," << MyRobotData.JointsTorque[0] << "," << MyRobotData.JointsNext[0] << "," << MyRobotData.JointsVelNext[0] << ",";
				oFile << MyRobotData.JointsNow[1] << "," << MyRobotData.JointsVelNow[1] << "," << MyRobotData.JointsTorque[1] << "," << MyRobotData.JointsNext[1] << "," << MyRobotData.JointsVelNext[1] << ",";
				oFile << MyRobotData.JointsNow[2] << "," << MyRobotData.JointsVelNow[2] << "," << MyRobotData.JointsTorque[2] << "," << MyRobotData.JointsNext[2] << "," << MyRobotData.JointsVelNext[2] << ",";
				oFile << MyRobotData.JointsNow[3] << "," << MyRobotData.JointsVelNow[3] << "," << MyRobotData.JointsTorque[3] << "," << MyRobotData.JointsNext[3] << "," << MyRobotData.JointsVelNext[3];
				oFile << std::endl;
			}

			//////��������ļ�Ϊexcel����

			//****ʹ�ö��еķ�ʽ���������			
			WaitForSingleObject(g_hMutex, INFINITE);    //ʹ�û�����������g_QueueData���ж�ȡ�Ͳ���ֿ�
			if (MyRobotData.JointsNow[0] <= 1000 && MyRobotData.JointsNow[0] >= -1000 && MyRobotData.JointsVelNow[0] <= 100 && MyRobotData.JointsVelNow[0] >= -100)
			{
				g_QueueData.push(MyRobotData);
			}
			ReleaseMutex(g_hMutex);
			//****ʹ�ö��еķ�ʽ������������

			////******ʹ�÷�����Ϣ�ķ�ʽ���������
			//::PostMessageA(hWnd,UM_DRAWROBOTDATA,(WPARAM)&MyRobotData,1);
			////******ʹ�÷�����Ϣ�ķ�ʽ������������
		}
	}
	return 0;
}

DWORD WINAPI ServerThreadForHelix(LPVOID lp)
{
	return 0;
}

//û��ʹ���������
LRESULT CShowRobotDataDlg::OnDrawRobotData(WPARAM wParam, LPARAM lParam)
{
	RobotData *myRobotData;
	myRobotData = (RobotData*)wParam;
	++m_count;
	m_pLineSerie1[0]->ClearSerie();
	m_pLineSerie1[1]->ClearSerie();
	m_pLineSerie1[2]->ClearSerie();

	m_pLineSerie2[0]->ClearSerie();
	m_pLineSerie2[1]->ClearSerie();
	m_pLineSerie2[2]->ClearSerie();

	m_pLineSerie3[0]->ClearSerie();
	m_pLineSerie3[1]->ClearSerie();
	m_pLineSerie3[2]->ClearSerie();

	m_pLineSerie4[0]->ClearSerie();
	m_pLineSerie4[1]->ClearSerie();
	m_pLineSerie4[2]->ClearSerie();

	LeftMoveArray(m_HightSpeedChartArray1[0], m_c_arrayLength, myRobotData->JointsNow[0]);  //�Ƕ�ֵ
	LeftMoveArray(m_HightSpeedChartArray1[1], m_c_arrayLength, myRobotData->JointsVelNow[0]);  //�ٶ�ֵ
	LeftMoveArray(m_HightSpeedChartArray1[2], m_c_arrayLength, myRobotData->JointsTorque[0]);  //����

	LeftMoveArray(m_HightSpeedChartArray2[0], m_c_arrayLength, myRobotData->JointsNow[1]);  //�Ƕ�ֵ
	LeftMoveArray(m_HightSpeedChartArray2[1], m_c_arrayLength, myRobotData->JointsVelNow[1]);  //�ٶ�ֵ
	LeftMoveArray(m_HightSpeedChartArray2[2], m_c_arrayLength, myRobotData->JointsTorque[1]);  //����

	LeftMoveArray(m_HightSpeedChartArray3[0], m_c_arrayLength, myRobotData->JointsNow[2]);  //�Ƕ�ֵ
	LeftMoveArray(m_HightSpeedChartArray3[1], m_c_arrayLength, myRobotData->JointsVelNow[2]);  //�ٶ�ֵ
	LeftMoveArray(m_HightSpeedChartArray3[2], m_c_arrayLength, myRobotData->JointsTorque[2]);  //����

	LeftMoveArray(m_HightSpeedChartArray4[0], m_c_arrayLength, myRobotData->JointsNow[3]);  //�Ƕ�ֵ
	LeftMoveArray(m_HightSpeedChartArray4[1], m_c_arrayLength, myRobotData->JointsVelNow[3]);  //�ٶ�ֵ
	LeftMoveArray(m_HightSpeedChartArray4[2], m_c_arrayLength, myRobotData->JointsTorque[3]);  //����

	LeftMoveArray(m_X, m_c_arrayLength, m_count);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

	m_pLineSerie2[0]->AddPoints(m_X, m_HightSpeedChartArray2[0], m_c_arrayLength);
	m_pLineSerie2[1]->AddPoints(m_X, m_HightSpeedChartArray2[1], m_c_arrayLength);
	m_pLineSerie2[2]->AddPoints(m_X, m_HightSpeedChartArray2[2], m_c_arrayLength);

	m_pLineSerie3[0]->AddPoints(m_X, m_HightSpeedChartArray3[0], m_c_arrayLength);
	m_pLineSerie3[1]->AddPoints(m_X, m_HightSpeedChartArray3[1], m_c_arrayLength);
	m_pLineSerie3[2]->AddPoints(m_X, m_HightSpeedChartArray3[2], m_c_arrayLength);

	m_pLineSerie4[0]->AddPoints(m_X, m_HightSpeedChartArray4[0], m_c_arrayLength);
	m_pLineSerie4[1]->AddPoints(m_X, m_HightSpeedChartArray4[1], m_c_arrayLength);
	m_pLineSerie4[2]->AddPoints(m_X, m_HightSpeedChartArray4[2], m_c_arrayLength);

	return 1;
}

/*û��ʹ���������*/
void CShowRobotDataDlg::DrawData(RobotData myrobotdata)
{
	++m_count;

	m_pLineSerie1[0]->ClearSerie();
	m_pLineSerie1[1]->ClearSerie();
	m_pLineSerie1[2]->ClearSerie();

	LeftMoveArray(m_HightSpeedChartArray1[0], m_c_arrayLength, myrobotdata.JointsNow[0]);  //�Ƕ�ֵ
	LeftMoveArray(m_HightSpeedChartArray1[1], m_c_arrayLength, myrobotdata.JointsVelNow[0]);  //�ٶ�ֵ
	LeftMoveArray(m_HightSpeedChartArray1[2], m_c_arrayLength, myrobotdata.JointsTorque[0]);  //����

	LeftMoveArray(m_X, m_c_arrayLength, m_count);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

	LeftMoveArray(m_X, m_c_arrayLength, m_count);

	m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
	m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
	m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

}


RobotData DealQueueData[100];    //����ȫ�ֱ����Ƚ�ʡÿ�����·����ڴ��ʱ��
void CShowRobotDataDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == 1)
	{
		////****ʹ�ö��еķ�ʽ���������			
		WaitForSingleObject(g_hMutex, INFINITE);
		int DataSize = g_QueueData.size();   //���ض�����Ԫ�ظ���
		for (int i = 0; i < DataSize; i++)
		{
			DealQueueData[i] = g_QueueData.front();   //ȡ��
			g_QueueData.pop();
		}
		ReleaseMutex(g_hMutex);
		////****ʹ�ö��еķ�ʽ������������
		//++m_count;
		for (int i = 0; i < 3; i++)
		{
			m_pLineSerie1[i]->ClearSerie();
			m_pLineSerie2[i]->ClearSerie();
			m_pLineSerie3[i]->ClearSerie();
			m_pLineSerie4[i]->ClearSerie();
		}

		LeftMoveArrayWithQueue(m_HightSpeedChartArray1[0], m_c_arrayLength, DealQueueData, DataSize, 11);  //�Ƕ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray1[1], m_c_arrayLength, DealQueueData, DataSize, 12);  //�ٶ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray1[2], m_c_arrayLength, DealQueueData, DataSize, 13);  //����

		LeftMoveArrayWithQueue(m_HightSpeedChartArray2[0], m_c_arrayLength, DealQueueData, DataSize, 21);  //�Ƕ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray2[1], m_c_arrayLength, DealQueueData, DataSize, 22);  //�ٶ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray2[2], m_c_arrayLength, DealQueueData, DataSize, 23);  //����

		LeftMoveArrayWithQueue(m_HightSpeedChartArray3[0], m_c_arrayLength, DealQueueData, DataSize, 31);  //�Ƕ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray3[1], m_c_arrayLength, DealQueueData, DataSize, 32);  //�ٶ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray3[2], m_c_arrayLength, DealQueueData, DataSize, 33);  //����

		LeftMoveArrayWithQueue(m_HightSpeedChartArray4[0], m_c_arrayLength, DealQueueData, DataSize, 31);  //�Ƕ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray4[1], m_c_arrayLength, DealQueueData, DataSize, 32);  //�ٶ�ֵ
		LeftMoveArrayWithQueue(m_HightSpeedChartArray4[2], m_c_arrayLength, DealQueueData, DataSize, 33);  //����

		LeftMoveArrayXWithQueue(m_X, m_c_arrayLength, m_count, DataSize);

		m_pLineSerie1[0]->AddPoints(m_X, m_HightSpeedChartArray1[0], m_c_arrayLength);
		m_pLineSerie1[1]->AddPoints(m_X, m_HightSpeedChartArray1[1], m_c_arrayLength);
		m_pLineSerie1[2]->AddPoints(m_X, m_HightSpeedChartArray1[2], m_c_arrayLength);

		m_pLineSerie2[0]->AddPoints(m_X, m_HightSpeedChartArray2[0], m_c_arrayLength);
		m_pLineSerie2[1]->AddPoints(m_X, m_HightSpeedChartArray2[1], m_c_arrayLength);
		m_pLineSerie2[2]->AddPoints(m_X, m_HightSpeedChartArray2[2], m_c_arrayLength);

		m_pLineSerie3[0]->AddPoints(m_X, m_HightSpeedChartArray3[0], m_c_arrayLength);
		m_pLineSerie3[1]->AddPoints(m_X, m_HightSpeedChartArray3[1], m_c_arrayLength);
		m_pLineSerie3[2]->AddPoints(m_X, m_HightSpeedChartArray3[2], m_c_arrayLength);

		m_pLineSerie4[0]->AddPoints(m_X, m_HightSpeedChartArray4[0], m_c_arrayLength);
		m_pLineSerie4[1]->AddPoints(m_X, m_HightSpeedChartArray4[1], m_c_arrayLength);
		m_pLineSerie4[2]->AddPoints(m_X, m_HightSpeedChartArray4[2], m_c_arrayLength);
	}
	else if (nIDEvent == 2)  //�жϰ����ǲ���һֱ�ڰ��ŵĶ�ʱ��
	{
		for (int i = 0; i < 6; i++)
		{
			int RealInteral;
			if ((keystopflag[i] || alive[i] == checkalive[i]) && ForceSense[i] != 0)  //����û�б�����
			{
				RealInteral = (stopinterval[i]>2 ? stopinterval[i] : 2);
				if (ForceSense[i] > RealInteral)
				{
					ForceSense[i] = ForceSense[i] - RealInteral;
					if (m_pShowForceDlg!=NULL)
						m_pShowForceDlg->ReDrawOpenGL(ForceSense);
				}
				else if (ForceSense[i] < -RealInteral)
				{
					ForceSense[i] = ForceSense[i] + RealInteral;
					if (m_pShowForceDlg != NULL)
						m_pShowForceDlg->ReDrawOpenGL(ForceSense);
				}
				else
				{
					ForceSense[i] = 0;
					if (m_pShowForceDlg != NULL)
						m_pShowForceDlg->ReDrawOpenGL(ForceSense);
				}
				switch (i)
				{
				case 0:	{ updateStaticText(i); break; }
				case 1:	{ updateStaticText(i); break; }
				case 2:	{ updateStaticText(i); break; }
				case 3:	{ updateStaticText(i); break; }
				case 4:	{ updateStaticText(i); break; }
				case 5:	{ updateStaticText(i); break; }
				}
			}
			else    //������ֵ�����ʱ�䱻����ȥ�ˣ�������ţ���ı�״̬ʹ����ε�״̬���ϴεĲ�һ��
			{
					checkalive[i] = alive[i];   //����ˢ�»��ŵ�״̬
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CShowRobotDataDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (fileopenflag)
	{
		oFile.close();
	}
	CDialogEx::OnClose();
}


/*����������
������
q----w----e----r----t----y
Fx---Fy---Fz---Mx---My---Mz
������
z----x----c----v----b----n
Fx---Fy---Fz---Mx---My---Mz
*/ 
BOOL CShowRobotDataDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case'Q': addForce(1); break;
		case'W': addForce(2); break;
		case'E': addForce(3); break;
		case'R': addForce(4); break;
		case'T': addForce(5); break;
		case'Y': addForce(6); break;
		case'Z': addForce(-1); break;
		case'X': addForce(-2); break;
		case'C': addForce(-3); break;
		case'V': addForce(-4); break;
		case'B': addForce(-5); break;
		case'N': addForce(-6); break;
		default:break;
		}
		return true;
	}
	else if (pMsg->message == WM_KEYUP)
	{
		switch (pMsg->wParam)
		{
		case'Q': stopForce(1); break;
		case'W': stopForce(2); break;
		case'E': stopForce(3); break;
		case'R': stopForce(4); break;
		case'T': stopForce(5); break;
		case'Y': stopForce(6); break;
		case'Z': stopForce(-1); break;
		case'X': stopForce(-2); break;
		case'C': stopForce(-3); break;
		case'V': stopForce(-4); break;
		case'B': stopForce(-5); break;
		case'N': stopForce(-6); break;
		default:break;
		}
		return true;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//*

//
void CShowRobotDataDlg::addForce(int i)
{

	int channel = abs(i)-1;
	keystopflag[channel] = false;	
	alive[channel]++;
	int direction = 0;
	if (i > 0) direction = 1;  //����
	else direction = -1;       //����

	if (direction > 0)
	{
		ForceSense[channel]++;
		if (m_pShowForceDlg != NULL)
			m_pShowForceDlg->ReDrawOpenGL(ForceSense);
	}     
	else
	{
		ForceSense[channel]--;
		if (m_pShowForceDlg != NULL)
			m_pShowForceDlg->ReDrawOpenGL(ForceSense);
	}

	stopinterval[channel] = abs(ForceSense[channel] / AUTODECRNUM);
	updateStaticText(channel);
}
void CShowRobotDataDlg::stopForce(int i)
{
	int channel = abs(i) - 1;
	keystopflag[channel] = true;
}

void  CShowRobotDataDlg::updateStaticText(int channel)
{
	CString str;
	str.Format(_T("%d"), ForceSense[channel]);   //ȡ�����˻���������m_Robotֱ������ϵ��ǰ��X������
	switch (channel)
	{
	case 0:{SetDlgItemText(IDC_STATIC_FX, str);break;}
	case 1:{SetDlgItemText(IDC_STATIC_FY, str);break;}
	case 2:{SetDlgItemText(IDC_STATIC_FZ, str);break;}
	case 3:{SetDlgItemText(IDC_STATIC_MX, str);break;}
	case 4:{SetDlgItemText(IDC_STATIC_MY, str);break;}
	case 5:{SetDlgItemText(IDC_STATIC_MZ, str);break;}
	}
}

void CShowRobotDataDlg::OnBnClickedRadio1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	autodecrese = true;
	SetTimer(2, TIMEINTERVAL, NULL);  //���ö�ʱ������ʱ����Ϊ100ms
}


void CShowRobotDataDlg::OnBnClickedRadio2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	autodecrese = false;
	KillTimer(2);
}

void CShowRobotDataDlg::OnBnClickedButton2()   //����������ϵ
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	// ���ȼ��ָ���Ƿ�Ϊ�գ���Ϊ�գ���ʾ�Ի���δ������������Ҫ�����ڴ沢��������
	//if (m_pShowForceDlg == NULL)
	//{
	//	m_pShowForceDlg = new CShowForceDlg(this);
	//	m_pShowForceDlg->Create(IDD_DIALOG_FORCE, this);
	//}
	//int nDlgWidth = 400;
	//int nDlgHeight = 200;

	//CRect rcClient;
	//GetClientRect(&rcClient);

	//// �Ի������
	//m_pShowForceDlg->MoveWindow((rcClient.Width() - nDlgWidth) / 2, (rcClient.Height() - nDlgHeight) / 2, nDlgWidth, nDlgHeight, TRUE);

	//// �Ի�����ʾ
	//m_pShowForceDlg->ShowWindow(SW_SHOW);

	if (m_pShowForceDlg == NULL)
	{
		m_pShowForceDlg = new CShowForceDlg(this);
		m_pShowForceDlg->Create(IDD_DIALOG_FORCE,this);
		m_pShowForceDlg->ShowWindow(SW_SHOWNORMAL); //��ʾ��ģ̬�Ի���
	}
}

LRESULT CShowRobotDataDlg::OnCloseChildDlgMessage(WPARAM wParam, LPARAM lParam)
{
	delete m_pShowForceDlg;
	m_pShowForceDlg = NULL;
	return 0;
}

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()
}

