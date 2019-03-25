#pragma once
#define WM_CLOSECHILDDLG    WM_USER+110   //�Զ���һ����Ϣ
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
// CShowForceDlg �Ի���
struct My3DPoint{
	GLdouble X;
	GLdouble Y;
	GLdouble Z;
};

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
	virtual BOOL OnInitDialog();

	HDC hDC;
	HGLRC m_hGLRC; //��������RC��ľ������ַ��
	int m_GLPixelIndex;
	CPoint MDown; //���ڼ�¼��갴��ʱ�ĵ������
	CPoint MMove;  //���ڼ�¼����ƶ������еĵ������
	int mousedown;  //��־λ��0-���û�а��£�1-����������

	// ���ڻ���ͼ��
	void DrawOpenGL();
	// ���ڴ���Picture Control��ı�ʱOpenGL����ͼ�е�ͼ�β�����
	void Reshape();
	// ���壨���ã���ΪDCѡ��������ظ�ʽ
	bool SetWindowPixelFormat(HDC hDC);
	// ������Ⱦ������RC��Render Context��������Ϊ��ǰRC
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
};
