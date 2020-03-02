
// ImagePickerDlg.h : 头文件
//

#pragma once


// CImagePickerDlg 对话框
class CImagePickerDlg : public CDialogEx
{
// 构造
public:
	CImagePickerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_IMAGEPICKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	BOOL m_bStart;         /// 这个变量主要用来设置按钮的可用状态
	CFont m_btnFont;       /// 按钮的字体
	CFont m_btnNextFont;   /// 下一批图片按钮的字体

public:
	void MyInitFunc();
	void ChangeFont();         /// 更改字体，把按钮的字体搞大
	void UpdateBtnStatus();    /// 根据bStart的值来更新按钮的可用状态

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
