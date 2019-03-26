#include "stdafx.h"
#include "NumberEdit.h"


// CNumberEdit

IMPLEMENT_DYNAMIC(CNumberEdit, CEdit)

CNumberEdit::CNumberEdit()
{
	//默认精度可以输入6个字符，包括 - 和 .
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
		//截获 Delete按键 ，不允许用此键删除
		if (46 == pMsg->wParam)
		{
			return TRUE;
		}
	}

	if (pMsg->message == WM_CHAR)
	{
		//获取当前输入前已经输入的数字字符个数
		GetWindowText(str);
		nPos = str.GetLength();

		//如果小于等于0则刚开始没有字符
		if (nPos <= 0)
		{
			m_countNum = 0;
		}

		//获取光标上次的位置
		int  curPosion = LOWORD(GetSel());

		int startChar = 0;
		int endChar = 0;
		GetSel(startChar, endChar);				//获取选中的字符位置
		int deleteNum = endChar - startChar;

		//Backspace键及Delete键
		if ((8 == pMsg->wParam) || (127 == pMsg->wParam))
		{

			//如果光标不在首位置，而且在字符长度大于1，则可以按backspace键
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

			//无法在此获得delete键消息
			//如果光标不在最后，且按下delete键时,后面有字符可以按Delete键 ，
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

			//其他情况直接return
			return FALSE;
		}

		//Enturn键
		if (pMsg->wParam == VK_RETURN)
		{
			GetFocus()->UpdateData(TRUE);
			return FALSE;//为母窗口保留处理，主要为焦点设置
		}

		//只允许输入一个小数点
		if ((46 == pMsg->wParam) && CheckUnique(46))
		{
			return TRUE;
		}


		//当前字符是’.’ 及 当前光标在第一位及当前还没有’-’，才可以输入-
		if ((46 == pMsg->wParam) && !CheckUnique(46))
		{
			if (curPosion == 0 || curPosion == m_precision)
			{
				return TRUE;
			}

			if (CheckUnique(45) && curPosion == 1)	//小数点不允许在负号后面
			{
				return TRUE;
			}

			return CEdit::PreTranslateMessage(pMsg);; //则接受这个消息，实现输入’.’
		}


		//只允许输入一个’-’
		if (((45 == pMsg->wParam) && CheckUnique(45)))
		{
			return TRUE;
		}


		//当前字符是’-’ 及 当前光标在第一位及当前还没有’-’，才可以输入-
		if ((45 == pMsg->wParam) && !CheckUnique(45))
		{
			if (curPosion != 0)		//光标不在第一位则不能输入 -
			{
				return TRUE;
			}

			return CEdit::PreTranslateMessage(pMsg); //则接受这个消息，实现输入’-’
		}


		//只允许输入负号，数字,点号。其它不处理直接返回
		if (45 != pMsg->wParam && !(pMsg->wParam >= 48 && pMsg->wParam <= 57) && 46 != pMsg->wParam)
		{
			return TRUE;
		}


		//只能输入6个字符包括(-  .)
		if (nPos >= m_precision)
		{
			return TRUE;
		}


		//只能连续输入多少个数字
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
