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
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mousedown = 0;
	m_Scale = 1.f;
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
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
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

	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	hDC = ::GetDC(pWnd->GetSafeHwnd());
	if (SetWindowPixelFormat(hDC) == FALSE)
		return 0;
	if (CreateViewGLRC(hDC) == FALSE)
		return 0;
	if (InitMyOpenGL() == false)
		return 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

// ���壨���ã���ΪDCѡ��������ظ�ʽ
bool CShowForceDlg::SetWindowPixelFormat(HDC hDC)
{
	static PIXELFORMATDESCRIPTOR pixelDesc =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd  
		1,                              // version number  
		PFD_DRAW_TO_WINDOW |            // support window  
		PFD_SUPPORT_OPENGL |            // support OpenGL  
		PFD_DOUBLEBUFFER,                // double buffered  
		PFD_TYPE_RGBA,                  // RGBA type  
		24,                             // 24-bit color depth  
		0, 0, 0, 0, 0, 0,               // color bits ignored  
		0,                              // no alpha buffer  
		0,                              // shift bit ignored  
		0,                              // no accumulation buffer  
		0, 0, 0, 0,                     // accum bits ignored  
		16,                             // 16-bit z-buffer  
		0,                              // no stencil buffer  
		0,                              // no auxiliary buffer  
		PFD_MAIN_PLANE,                 // main layer  
		0,                              // reserved  
		0, 0, 0                         // layer masks ignored  
	};
	m_GLPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
	if (m_GLPixelIndex == 0)
	{
		return false;
	}
	if (::SetPixelFormat(hDC, m_GLPixelIndex, &pixelDesc) == FALSE)
	{
		return FALSE;
	}
	int n = ::GetPixelFormat(hDC);
	::DescribePixelFormat(hDC, n, sizeof(pixelDesc), &pixelDesc);

	return TRUE;
}


// ������Ⱦ������RC��Render Context��������Ϊ��ǰRC
bool CShowForceDlg::CreateViewGLRC(HDC hDC)
{
	m_hGLRC = ::wglCreateContext(hDC);
	if (m_hGLRC == 0)
		return FALSE;
	if (::wglMakeCurrent(hDC, m_hGLRC) == FALSE)
		return FALSE;
	//Specify Black as the clear color  
	::glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//Specify the back of the buffer as clear depth  
	::glClearDepth(1.0f);
	//Enable Depth Testing  
	::glEnable(GL_DEPTH_TEST);
	return TRUE;
}

bool CShowForceDlg::InitMyOpenGL(void)
{

	return true;
}


void CShowForceDlg::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
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
		Reshape();
		DrawOpenGL();
	}
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� 
	//CDialogEx::OnPaint();
}

void CShowForceDlg::Reshape()
{

	CRect rect;
	GetDlgItem(IDC_STATIC)->GetWindowRect(rect);
	glViewport(0, 0, rect.Width(), rect.Height());
	glLoadIdentity();
}
// ���ڻ���ͼ��
void CShowForceDlg::DrawOpenGL()
{
	GLfloat m_xPos = 0.0f;
	GLfloat m_yPos = 0.0f;
	GLfloat m_zPos = 0.0f;
	GLfloat m_xAngle = 0.0f;
	GLfloat m_yAngle = 0.0f;
	GLfloat m_zAngle = 0.0f;
	if (m_hGLRC)
		wglMakeCurrent(hDC, m_hGLRC);
	else
		return;

	//glRotatef(m_zAngle, 0.0f, 0.0f, 1.0f);
	glScalef(m_Scale, m_Scale, m_Scale);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();                             //���û�����ϵ��ԭ���ƶ�����Ļ������λ��
	glTranslatef(m_xPos, m_yPos, m_zPos);
	glRotatef(30, 1.0f, 0.0f, 0.0f);
	glRotatef(30, 0.0f, 1.0f, 0.0f);
	glRotatef(30, 0.0f, 0.0f, 1.0f);
	glScalef(m_Scale, m_Scale, m_Scale);
	glColor3f(1.0, 1.0, 0.0);
	//glutWireTeapot(2);
	glColor3f(1.f, 0.f, 0.f);
	glLineWidth(2.f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.f);
	glVertex3f(1.0, 0.0f, 0.f);
	glEnd();
	char a = 'x';
	glRasterPos3f(1.1, 0.f, 0.f);//������ʾλ��
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, a);

	glColor3f(0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.f);
	glVertex3f(0.f, 1.0f, 0.f);
	glEnd();
	a = 'y';
	glRasterPos3f(0.f, 1.1f, 0.f);//������ʾλ��
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, a);

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.f);
	glVertex3f(0.f, 0.0f, 1.f);
	glEnd();
	a = 'z';
	glRasterPos3f(0.f, 0.f, 1.1f);//������ʾλ��
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, a);
//	GLDrawSpaceAxes();
	//GLDrawCubeCoordinates();
	//glutSolidSphere(7.85f, 30, 30);//�뾶7.85 λ��ԭ�� 30�����ߺ�γ��
	::SwapBuffers(hDC);

}

#define AXES_LEN 10 //�����᳤
// ���ڴ���Picture Control��ı�ʱOpenGL����ͼ�е�ͼ�β�����

void CShowForceDlg::GLGrid(My3DPoint& pt1, My3DPoint& pt2, int num)
{
	const float _xLen = (pt2.X - pt1.X) / num;
	const float _yLen = (pt2.Y - pt1.Y) / num;
	const float _zLen = (pt2.Z - pt1.Z) / num;
	glLineWidth(1.0f);
	glLineStipple(1, 0x0303);//������ʽ
	glBegin(GL_LINES);
	glEnable(GL_LINE_SMOOTH);
	//glColor3f(0.0f,0.0f, 1.0f); //��ɫ����
	int xi = 0;
	int yi = 0;
	int zi = 0;
	//����ƽ����X��ֱ��
	for (zi = 0; zi <= num; zi++)
	{
		float z = _zLen * zi + pt1.Z;
		for (yi = 0; yi <= num; yi++)

		{
			float y = _yLen * yi + pt1.Y;
			glVertex3f(pt1.X, y, z);
			glVertex3f(pt2.X, y, z);
		}
	}
	//����ƽ����Y��ֱ��
	for (zi = 0; zi <= num; zi++)
	{
		float z = _zLen * zi + pt1.Z;
		for (xi = 0; xi <= num; xi++)
		{
			float x = _xLen * xi + pt1.X;
			glVertex3f(x, pt1.Y, z);
			glVertex3f(x, pt2.Y, z);
		}
	}
	//����ƽ����Z��ֱ��
	for (yi = 0; yi <= num; yi++)
	{
		float y = _yLen * yi + pt1.Y;
		for (xi = 0; xi <= num; xi++)

		{
			float x = _xLen * xi + pt1.X;
			glVertex3f(x, y, pt1.Z);
			glVertex3f(x, y, pt2.Z);
		}
	}
	glEnd();

}

void CShowForceDlg::GLDrawSpaceAxes(void)
{
	GLUquadricObj *objCylinder = gluNewQuadric();
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere(0.25, 6, 6);
	glColor3f(0.0f, 0.0f, 1.0f);
	gluCylinder(objCylinder, 0.1, 0.1, AXES_LEN, 10, 5);
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(objCylinder, 0.3, 0.0, 0.6, 10, 5);
	glPushMatrix();
	glTranslatef(0, 0.5, AXES_LEN);
	glRotatef(90, 0.0, 1.0, 0.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	glRotatef(-90, 1.0, 0.0, 0.0);
	gluCylinder(objCylinder, 0.1, 0.1, AXES_LEN, 10, 5);
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(objCylinder, 0.3, 0.0, 0.6, 10, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, AXES_LEN, 0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(objCylinder, 0.1, 0.1, AXES_LEN, 10, 5);
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(objCylinder, 0.3, 0.0, 0.6, 10, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(AXES_LEN, 0.5, 0);
	glPopMatrix();
}

void CShowForceDlg::GLDrawCubeCoordinates(void)
{
	/*****�������*****/
	/*****ʹ����ɫ���������һЩ��ݣ� ��Ҫ��Ե����
	�Լ����໥�ص��Ķ���εķ���ݡ�*****/
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//������������ϵ
	GLUquadricObj *objCylinder = gluNewQuadric();
	glRotatef(-45, 0.0, 1.0, 0.0);

	//ȷ������ϵԭ��
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(-5, 0, -5);
	glutSolidSphere(0.2, 20, 20);
	//glutSolidTorus(0.2,1,10,10);Բ��

	glPopMatrix();

	//��������
	My3DPoint cpoint1 = { -5, 0, -5 };
	My3DPoint cpoint2 = { 5, 0, 5 };
	glColor3f(0.9f, 0.9f, 0.9f);

	GLGrid(cpoint1, cpoint2, 10);
	//������

	glPushMatrix();

	{
		glRotatef(90, 1.0, 0.0, 0.0);
		glTranslatef(0.0f, -5, -5);
		My3DPoint cpoint3 = { -5, 00, -5 };
		My3DPoint cpoint4 = { 5, 00, 5 };
		glColor3f(0.9f, 0.9f, 0.0f);
		GLGrid(cpoint3, cpoint4, 10);

	}
	glPopMatrix();

	//��������
	glPushMatrix();
	glRotatef(90, 0.0, 0.0, 1.0);
	glTranslatef(5, 5, -0);
	My3DPoint cpoint5 = { -5, 0, -5 };
	My3DPoint cpoint6 = { 5, 0, 5 };
	glColor3f(0.0f, 0.9f, 0.0f);
	GLGrid(cpoint5, cpoint6, 10);
	glPopMatrix();

	//��������
	glPushMatrix();
	glColor3f(0.0f, 1, 0.0f);
	glTranslatef(-5, 0, -5);
	gluCylinder(objCylinder, 0.05, 0.05, AXES_LEN, 10, 5);
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(objCylinder, 0.2, 0.0, 0.5, 10, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 0, -5);
	glTranslatef(0, 0.2, AXES_LEN);
	glRotatef(90, 0.0, 1.0, 0.0);
	glPopMatrix();

	//��������
	glPushMatrix();
	glColor3f(1, 0, 0.0f);
	glTranslatef(-5, 0, -5);
	glRotatef(90, 0.0, 1.0, 0.0);
	gluCylinder(objCylinder, 0.05, 0.05, AXES_LEN, 10, 5);
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(objCylinder, 0.2, 0.0, 0.5, 10, 5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-5, 0, -5);
	glRotatef(90, 0.0, 1.0, 0.0);
	glTranslatef(0, 0.2, AXES_LEN);
	glRotatef(90, 0.0, 1.0, 0.0);
	glPopMatrix();

	//��������
	glPushMatrix();
	glColor3f(1, 1, 0.0f);
	glTranslatef(-5, 0, -5);
	glRotatef(-90, 1.0, 0.0, 0.0);
	gluCylinder(objCylinder, 0.05, 0.05, AXES_LEN, 10, 5);
	glTranslatef(0, 0, AXES_LEN);
	gluCylinder(objCylinder, 0.2, 0.0, 0.5, 10, 5);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-5, 0, -5);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.6, AXES_LEN);
	glRotatef(90, 0.0, 1.0, 0.0);
	glRotatef(90, 0.0, 0.0, 1.0);
	glPopMatrix();

	/*****ȡ�������*****/
	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_POLYGON_SMOOTH);
}

void CShowForceDlg::Render3DCylinder(My3DPoint &m_point1, My3DPoint &m_point2, double radius, double slices, double stacks)
{
	//����Ŀ������
	GLdouble  dir_x = m_point2.X - m_point1.X;
	GLdouble  dir_y = m_point2.Y - m_point1.Y;
	GLdouble  dir_z = m_point2.Z - m_point1.Z;
	//���Ŀ������ģ
	GLdouble  bone_length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	GLUquadricObj *  quad_obj = NULL;
	if (quad_obj == NULL)
		quad_obj = gluNewQuadric();
	gluQuadricDrawStyle(quad_obj, GLU_FILL);
	gluQuadricNormals(quad_obj, GLU_SMOOTH);
	glPushMatrix();
	// ƽ�Ƶ���ʼ��
	glTranslated(m_point1.X, m_point1.Y, m_point1.Z);
	// ���㳤��
	double  length;
	length = sqrt(dir_x*dir_x + dir_y*dir_y + dir_z*dir_z);
	if (length < 0.0001) {
		dir_x = 0.0; dir_y = 0.0; dir_z = 1.0;  length = 1.0;
	}
	dir_x /= length;  dir_y /= length;  dir_z /= length;
	// ������ʼ���� 
	GLdouble  up_x, up_y, up_z;
	up_x = 0.0;
	up_y = 1.0;
	up_z = 0.0;
	// ��������(dir_x,dir_y,dir_z)������(up_x,up_y,up_z)�ķ�����(side_x ,side_y,side_z)  
	double  side_x, side_y, side_z;
	side_x = up_y * dir_z - up_z * dir_y;
	side_y = up_z * dir_x - up_x * dir_z;
	side_z = up_x * dir_y - up_y * dir_x;
	length = sqrt(side_x*side_x + side_y*side_y + side_z*side_z);
	if (length < 0.0001) {
		side_x = 1.0; side_y = 0.0; side_z = 0.0;  length = 1.0;
	}
	side_x /= length;  side_y /= length;  side_z /= length;
	up_x = dir_y * side_z - dir_z * side_y;
	up_y = dir_z * side_x - dir_x * side_z;
	up_z = dir_x * side_y - dir_y * side_x;
	// ����任����
	GLdouble  m[16] = { side_x, side_y, side_z, 0.0,
		up_x, up_y, up_z, 0.0,
		dir_x, dir_y, dir_z, 0.0,
		0.0, 0.0, 0.0, 1.0 };
	glMultMatrixd(m);
	// Բ�������
	GLdouble radius1 = radius;		// �뾶
	GLdouble slices1 = slices;		//	����
	GLdouble stack1 = stacks;		// �ݹ����
	gluCylinder(quad_obj, radius1, radius, bone_length, slices1, stack1);
	glPopMatrix();
	gluDeleteQuadric(quad_obj);//���ٶ��η��̶����ͷ��ڴ�
}

void CShowForceDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (mousedown)
	{
		MMove.x += point.x - MDown.x; //����ڴ���x�᷽���ϵ������ӵ��ӵ���x��ļн��ϣ��Ϳ�������ת
		if ((MMove.x >= 180) | (MMove.x <= -180))
		{
			MMove.x = 0;
		}

		MMove.y += point.y - MDown.y;  //����ڴ���y�᷽���ϵĸı�ӵ��ӵ�y�������ϣ��Ϳ�������ת
		if ((MMove.y >= 90) | (MMove.y <= -90))
		{
			MMove.y = 0;
		}
		MDown.x = point.x, MDown.y = point.y;  //����ʱ��������Ϊ��ֵ��Ϊ��һ�μ���������׼

	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CShowForceDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MDown.x = point.x;
	MDown.y = point.y;
	mousedown = 1;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CShowForceDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	mousedown = 0;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CShowForceDlg::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// �˹���Ҫ�� Windows Vista ����߰汾��
	// _WIN32_WINNT ���ű��� >= 0x0600��
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (zDelta >= 0)   //�Ŵ� zDelta����ֵ
	{
		m_Scale += 0.1 *zDelta / 120;
	}
	else   //��С��zDelta�Ǹ�ֵ
	{
		if ((m_Scale + 0.1 *zDelta / 120)>0.01)  //�����жϼ�ȥ��ֵ֮���������ӻ�����С���ţ�������С����ֵ�������Ǹ���
			m_Scale += 0.1 *zDelta / 120;
	}
	CRect Rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&Rect);
	ScreenToClient(Rect);
	InvalidateRect(Rect, TRUE);
	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}


BOOL CShowForceDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//zDelta��120�ı��������������߸�
	if (zDelta >= 0)   //�Ŵ� zDelta����ֵ
	{
		m_Scale += 0.1 *zDelta / 120;
	}
	else   //��С��zDelta�Ǹ�ֵ
	{
		if ((m_Scale + 0.1 *zDelta / 120)>0.01)  //�����жϼ�ȥ��ֵ֮���������ӻ�����С���ţ�������С����ֵ�������Ǹ���
			m_Scale += 0.1 *zDelta / 120;
	}
	CRect Rect;
	GetDlgItem(IDC_STATIC)->GetClientRect(&Rect);
	ScreenToClient(Rect);
	InvalidateRect(Rect, TRUE); 
	//RedrawWindow();
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}
