#pragma once
#define WM_CLOSECHILDDLG    WM_USER+110   //自定义一个消息

// CShowForceDlg 对话框

class CShowForceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowForceDlg)

public:
	CShowForceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowForceDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_FORCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
