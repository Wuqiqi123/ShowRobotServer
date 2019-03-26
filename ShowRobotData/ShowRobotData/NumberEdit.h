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
	* @describe:   �����жϵ�ǰEdit�ı����Ƿ����ĳ���ַ�
	* @param[in]:  nChar Ҫ�����ַ�
	* @return:     BOOL  TRUE:�Ѵ���; FALSE:������
	*/
	BOOL CheckUnique(char nChar);


private:
	int  m_countNum;	//������������ֳ��� ���������ź�С����

	int	 m_precision;	//������������ֳ��ȣ�����-��С����

public:

	/**
	* @describe:   �������볤�ȣ��������š�С���㣩
	* @param[in]:  Precision	���볤��
	* @return:     ��
	*/
	void SetPrecision(int precision);
};
