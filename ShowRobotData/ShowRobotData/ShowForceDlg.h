#pragma once
#define WM_CLOSECHILDDLG    WM_USER+110   //自定义一个消息
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Resource.h"
// CShowForceDlg 对话框
struct My3DPoint{
	GLdouble X;
	GLdouble Y;
	GLdouble Z;
};

class CShowForceDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowForceDlg)

public:
	CShowForceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowForceDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_FORCE };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	HDC hDC;
	HGLRC m_hGLRC; //用作创建RC后的句柄（地址）
	int m_GLPixelIndex;
	CPoint MDown; //用于记录鼠标按下时的点的坐标
	CPoint MMove;  //用于记录鼠标移动过程中的点的坐标
	GLfloat m_xAngle;
	GLfloat m_yAngle;
	GLfloat m_zAngle;
	GLfloat m_xPos;
	GLfloat m_yPos;
	GLfloat m_zPos;
	float m_Scale;
	int mousedown;  //标志位，0-鼠标没有按下，1-鼠标左键按下

	// 用于绘制图形
	void initDrawOpenGL();
	void ReDrawOpenGL(int ForceSense[6]);
	void RotateModel();
	// 用于处理当Picture Control框改变时OpenGL的视图中的图形不变形
	void Reshape();
	// 定义（设置）并为DC选择最佳像素格式
	bool SetWindowPixelFormat(HDC hDC);
	// 创建渲染描述表RC（Render Context）并设置为当前RC
	bool CreateViewGLRC(HDC hDC);
	afx_msg void OnPaint();
	void Render3DCylinder(My3DPoint &m_point1, My3DPoint &m_point2, double radius, double slices, double stacks);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	bool InitMyOpenGL(void);
	void GLGrid(My3DPoint& pt1, My3DPoint& pt2, int num);
	void CShowForceDlg::GLDrawSpaceAxes(void);
	void GLDrawCubeCoordinates(void);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
