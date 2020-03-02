#pragma once


// CParamSetDlg �Ի���

class CParamSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParamSetDlg)

public:
	CParamSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamSetDlg();

// �Ի�������
	enum { IDD = IDD_DLG_PARAM_SET };
	CString m_strImageRecvDir;
	CString m_strImagePickDir;
	CString m_strImageBackDir;

public:
	void MyInitFunc();

	/// ������ò����ļ��Ƿ����
	BOOL CheckFileExist(LPCTSTR lpFileName);

	/// �����ò����ļ�
	void ReadSetParameters();

	/// д���ò����ļ�
	void WriteSetParameters();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSetBtnSave();
	afx_msg void OnBnClickedSetBtnExit();
};
