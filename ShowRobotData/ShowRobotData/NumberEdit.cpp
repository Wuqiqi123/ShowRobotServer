#include "stdafx.h"
#include "NumberEdit.h"


// CNumberEdit

IMPLEMENT_DYNAMIC(CNumberEdit, CEdit)

CNumberEdit::CNumberEdit()
{
	//Ĭ�Ͼ��ȿ�������6���ַ������� - �� .
	m_precision = 6;
}

CNumberEdit::~CNumberEdit()
{
}


BEGIN_MESSAGE_MAP(CNumberEdit, CEdit)
END_MESSAGE_MAP()



// CNumberEdit message handlers


BOOL CNumberEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	CString str;
	int nPos = 0;

	if (pMsg->message == WM_KEYDOWN)
	{
		//�ػ� Delete���� ���������ô˼�ɾ��
		if (46 == pMsg->wParam)
		{
			return TRUE;
		}
	}

	if (pMsg->message == WM_CHAR)
	{
		//��ȡ��ǰ����ǰ�Ѿ�����������ַ�����
		GetWindowText(str);
		nPos = str.GetLength();

		//���С�ڵ���0��տ�ʼû���ַ�
		if (nPos <= 0)
		{
			m_countNum = 0;
		}

		//��ȡ����ϴε�λ��
		int  curPosion = LOWORD(GetSel());

		int startChar = 0;
		int endChar = 0;
		GetSel(startChar, endChar);				//��ȡѡ�е��ַ�λ��
		int deleteNum = endChar - startChar;

		//Backspace����Delete��
		if ((8 == pMsg->wParam) || (127 == pMsg->wParam))
		{

			//�����겻����λ�ã��������ַ����ȴ���1������԰�backspace��
			if (8 == pMsg->wParam && curPosion != 0 && nPos>1)
			{
				if (deleteNum != 0)
				{
					m_countNum = m_countNum - deleteNum;
				}
				else
				{
					m_countNum--;
				}
				return FALSE;
			}

			//�޷��ڴ˻��delete����Ϣ
			//�����겻������Ұ���delete��ʱ,�������ַ����԰�Delete�� ��
			//if (127==pMsg->wParam && curPosion!=nPos && curPosion<nPos)
			//{
			//	if (deleteNum!=0)
			//	{
			//		m_countNum = m_countNum - deleteNum;
			//	}
			//	else
			//	{
			//		m_countNum --;
			//	}
			//	return FALSE;
			//}

			//�������ֱ��return
			return FALSE;
		}

		//Enturn��
		if (pMsg->wParam == VK_RETURN)
		{
			GetFocus()->UpdateData(TRUE);
			return FALSE;//Ϊĸ���ڱ���������ҪΪ��������
		}

		//ֻ��������һ��С����
		if ((46 == pMsg->wParam) && CheckUnique(46))
		{
			return TRUE;
		}


		//��ǰ�ַ��ǡ�.�� �� ��ǰ����ڵ�һλ����ǰ��û�С�-�����ſ�������-
		if ((46 == pMsg->wParam) && !CheckUnique(46))
		{
			if (curPosion == 0 || curPosion == m_precision)
			{
				return TRUE;
			}

			if (CheckUnique(45) && curPosion == 1)	//С���㲻�����ڸ��ź���
			{
				return TRUE;
			}

			return CEdit::PreTranslateMessage(pMsg);; //����������Ϣ��ʵ�����롯.��
		}


		//ֻ��������һ����-��
		if (((45 == pMsg->wParam) && CheckUnique(45)))
		{
			return TRUE;
		}


		//��ǰ�ַ��ǡ�-�� �� ��ǰ����ڵ�һλ����ǰ��û�С�-�����ſ�������-
		if ((45 == pMsg->wParam) && !CheckUnique(45))
		{
			if (curPosion != 0)		//��겻�ڵ�һλ�������� -
			{
				return TRUE;
			}

			return CEdit::PreTranslateMessage(pMsg); //����������Ϣ��ʵ�����롯-��
		}


		//ֻ�������븺�ţ�����,��š�����������ֱ�ӷ���
		if (45 != pMsg->wParam && !(pMsg->wParam >= 48 && pMsg->wParam <= 57) && 46 != pMsg->wParam)
		{
			return TRUE;
		}


		//ֻ������6���ַ�����(-  .)
		if (nPos >= m_precision)
		{
			return TRUE;
		}


		//ֻ������������ٸ�����
		if (pMsg->wParam >= 48 && pMsg->wParam <= 57)
		{
			m_countNum++;
		}

	}

	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CNumberEdit::CheckUnique(char nChar)
{
	CString str;
	int nPos = 0;
	GetWindowText(str);
	nPos = str.Find(nChar);

	return (nPos >= 0) ? TRUE : FALSE;
}

void CNumberEdit::SetPrecision(int precision)
{
	m_precision = precision;
}
