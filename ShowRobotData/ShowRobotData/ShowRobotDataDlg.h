
// ShowRobotDataDlg.h : 头文件
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

// CShowRobotDataDlg 对话框
class CShowRobotDataDlg : public CDialogEx
{
// 构造
public:
	CShowRobotDataDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CShowRobotDataDlg();
// 对话框数据
	enum { IDD = IDD_SHOWROBOTDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	CShowForceDlg * m_pShowForceDlg;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	double m_HightSpeedChartArray1[3][1024];  //Y轴参数
	double m_HightSpeedChartArray2[3][1024];
	double m_HightSpeedChartArray3[3][1024];
	double m_HightSpeedChartArray4[3][1024];
	double m_X[1024];   //X轴参数
	unsigned int m_count;
	const size_t m_c_arrayLength=500;
	afx_msg void OnBnClickedStartserver();

	// //本程序作为服务器接受的主机的消息
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
	BOOL m_autodecrese;  //力传感器的自动衰减器
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedButton2();

	// //1、只能响应输入数字、负号、小数点。以及backspace键；2、只能输入一个负号、小数点；3、小数点不能在负号后面；4、负号只能够在第一位输入；可设置输入的长度（包括负号和小数点）
	CEdit m_NumberEdit;


	afx_msg void OnBnClickedButtonChooseForcesource();
	BOOL m_isnJS;
	CJoystick* joystick;
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio3();
};


