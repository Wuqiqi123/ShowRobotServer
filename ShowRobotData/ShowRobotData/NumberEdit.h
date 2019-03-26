#pragma once


// CNumberEdit
class CNumberEdit : public CEdit
{
	DECLARE_DYNAMIC(CNumberEdit)

public:
	CNumberEdit();
	virtual ~CNumberEdit();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);


	/**
	* @describe:   用于判断当前Edit文本中是否包含某个字符
	* @param[in]:  nChar 要检查的字符
	* @return:     BOOL  TRUE:已存在; FALSE:不存在
	*/
	BOOL CheckUnique(char nChar);


private:
	int  m_countNum;	//保存输入的数字长度 不包括负号和小数点

	int	 m_precision;	//限制输入的数字长度，包括-和小数点

public:

	/**
	* @describe:   设置输入长度（包括负号、小数点）
	* @param[in]:  Precision	输入长度
	* @return:     无
	*/
	void SetPrecision(int precision);
};
