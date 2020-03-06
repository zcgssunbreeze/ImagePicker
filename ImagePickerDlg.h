
// ImagePickerDlg.h : 头文件
//

#pragma once

//// 此结构体作为参数传给检查和移动文件的线程，包含了收稿目录名，选稿目录名，备份目录名（是作为挑选文件的一个原始备份）
typedef struct _tagThreadParam
{
	WCHAR m_szRecvDir[MAX_PATH];
	WCHAR m_szPickDir[MAX_PATH];
	WCHAR m_szBackDir[MAX_PATH];
	WCHAR m_szTempDir[MAX_PATH];

	CWnd  *pBtnNextWnd;
}THREADPARAM;


// CImagePickerDlg 对话框
class CImagePickerDlg : public CDialogEx
{
// 构造
public:
	CImagePickerDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CImagePickerDlg();                      //标准析构函数

// 对话框数据
	enum { IDD = IDD_IMAGEPICKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	BOOL  m_bStart;        /// 这个变量主要用来设置按钮的可用状态
	CFont m_btnFont;       /// 按钮的字体
	CFont m_btnNextFont;   /// 下一批图片按钮的字体
	CFont m_staticFont;    /// static控件显示工作目录的字体

	/// 收稿分稿线程的对象
	CWinThread *m_pMonitorThread;
	/// 收稿分稿线程的参数
	THREADPARAM *m_pstThreadParam;

public:
	/// 自定义初始化函数
	void MyInitFunc();

	/// 更改字体，把按钮的字体搞大
	void ChangeFont();

	/// 根据bStart的值来更新按钮的可用状态
	void UpdateBtnStatus();    

	/// 读取设置参数文件
	void ReadSetParameters();

	/// 在static控件上显示设置的工作目录信息
	void DispWorkDirectory();

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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedMainBtnParamSet();
	afx_msg void OnBnClickedMainBtnStart();
	afx_msg void OnBnClickedMainBtnStop();
	afx_msg void OnBnClickedMainBtnNext();
};
