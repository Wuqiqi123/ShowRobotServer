
// ShowRobotDataDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "ChartCtrl/ChartCtrl.h" 
#include "ChartCtrl/ChartTitle.h"
#include "ChartCtrl/ChartAxisLabel.h"
#include "ChartCtrl/ChartLineSerie.h"
#include "ChartCtrl/ChartAxis.h"
#include "afxcmn.h"

struct RobotData
{
	double JointsNow[4];
	double JointsNext[4];
	double JointsVelNow[4];
	double JointsVelNext[4];
	double Origin6axisForce[6];
	double JointsTorque[4];
	double CartesianPositionNow[4];
	double CartesianPositionNext[4];
	double CartesianVelNow[4];
	double CartesianVelNext[4];
};

// CShowRobotDataDlg �Ի���
class CShowRobotDataDlg : public CDialogEx
{
// ����
public:
	CShowRobotDataDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SHOWROBOTDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	CChartCtrl m_ChartCtrl1;
	CChartCtrl m_ChartCtrl2;
	CChartCtrl m_ChartCtrl3;
	CChartCtrl m_ChartCtrl4;

	CChartLineSerie* m_pLineSerie1[3];
	CChartLineSerie* m_pLineSerie2[3];
	CChartLineSerie* m_pLineSerie3[3];
	CChartLineSerie* m_pLineSerie4[3];

	double m_HightSpeedChartArray1[3][1024];  //Y�����
	double m_HightSpeedChartArray2[3][1024];
	double m_HightSpeedChartArray3[3][1024];
	double m_HightSpeedChartArray4[3][1024];
	double m_X[1024];   //X�����
	unsigned int m_count;
	const size_t m_c_arrayLength=500;
	afx_msg void OnBnClickedStartserver();

	// //��������Ϊ���������ܵ���������Ϣ
	CListBox m_recvMessage;
	afx_msg void OnLbnSelchangeList1Showmessage();
	afx_msg LRESULT OnDrawRobotData(WPARAM WParam, LPARAM LParam);

public:
	void update(CString s); 	

	
private: CEdit* send_edit;

public:
	bool fileopenflag;
	CEdit m_TCPPort;
	CIPAddressCtrl m_TCPIPaddr;
	void DrawData(RobotData myrobotdata);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


