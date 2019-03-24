// ShowForceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowRobotData.h"
#include "ShowForceDlg.h"
#include "afxdialogex.h"

// CShowForceDlg 对话框

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


// CShowForceDlg 消息处理程序


void CShowForceDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

	HWND parhwnd = GetParent()->m_hWnd;//取得父窗口句柄
	::PostMessage(parhwnd, WM_CLOSECHILDDLG, (WPARAM)0, 0);//向父窗口发消息
	CDialogEx::OnOK();
}
