#pragma once
#define WM_CLOSECHILDDLG    WM_USER+110   //�Զ���һ����Ϣ

// CShowForceDlg �Ի���

class CShowForceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowForceDlg)

public:
	CShowForceDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowForceDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_FORCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
