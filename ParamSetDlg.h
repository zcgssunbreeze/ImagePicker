#pragma once


// CParamSetDlg 对话框

class CParamSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParamSetDlg)

public:
	CParamSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamSetDlg();

// 对话框数据
	enum { IDD = IDD_DLG_PARAM_SET };
	CString m_strImageRecvDir;
	CString m_strImagePickDir;
	CString m_strImageBackDir;

public:
	void MyInitFunc();

	/// 检查设置参数文件是否存在
	BOOL CheckFileExist(LPCTSTR lpFileName);

	/// 读设置参数文件
	void ReadSetParameters();

	/// 写设置参数文件
	void WriteSetParameters();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSetBtnSave();
	afx_msg void OnBnClickedSetBtnExit();
};
