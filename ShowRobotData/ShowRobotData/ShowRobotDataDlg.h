
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
#include "ShowForceDlg.h"
#include "NumberEdit.h"
#include "Joystick.h"
#include "Resource.h"
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
	virtual ~CShowRobotDataDlg();
// �Ի�������
	enum { IDD = IDD_SHOWROBOTDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	CShowForceDlg * m_pShowForceDlg;
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
	bool bIsMouseDown[6];
	int ForceSense[6];
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
	void addForce(int i);
	void stopForce(int i);
	int alive[6];
	int checkalive[6];
	bool keystopflag[6];
	bool autodecrese;
	int stopinterval[6];
	afx_msg LRESULT OnDrawRobotData(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnCloseChildDlgMessage(WPARAM WParam, LPARAM LParam);

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
	void updateStaticText(int channel);
	BOOL m_autodecrese;  //�����������Զ�˥����
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedButton2();

	// //1��ֻ����Ӧ�������֡����š�С���㡣�Լ�backspace����2��ֻ������һ�����š�С���㣻3��С���㲻���ڸ��ź��棻4������ֻ�ܹ��ڵ�һλ���룻����������ĳ��ȣ��������ź�С���㣩
	CEdit m_NumberEdit;


	afx_msg void OnBnClickedButtonChooseForcesource();
	BOOL m_isnJS;
	CJoystick* joystick;
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio3();
};


