// ShowForceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ShowRobotData.h"
#include "ShowForceDlg.h"
#include "afxdialogex.h"

// CShowForceDlg �Ի���

IMPLEMENT_DYNAMIC(CShowForceDlg, CDialogEx)

CShowForceDlg::CShowForceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowForceDlg::IDD, pParent)
{

}

CShowForceDlg::~CShowForceDlg()
{
}

void CShowForceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowForceDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CShowForceDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CShowForceDlg ��Ϣ�������


void CShowForceDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	HWND parhwnd = GetParent()->m_hWnd;//ȡ�ø����ھ��
	::PostMessage(parhwnd, WM_CLOSECHILDDLG, (WPARAM)0, 0);//�򸸴��ڷ���Ϣ
	CDialogEx::OnOK();
}


BOOL CShowForceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
