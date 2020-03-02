
// ImagePickerDlg.h : ͷ�ļ�
//

#pragma once


// CImagePickerDlg �Ի���
class CImagePickerDlg : public CDialogEx
{
// ����
public:
	CImagePickerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_IMAGEPICKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	BOOL m_bStart;         /// ���������Ҫ�������ð�ť�Ŀ���״̬
	CFont m_btnFont;       /// ��ť������
	CFont m_btnNextFont;   /// ��һ��ͼƬ��ť������

public:
	void MyInitFunc();
	void ChangeFont();         /// �������壬�Ѱ�ť��������
	void UpdateBtnStatus();    /// ����bStart��ֵ�����°�ť�Ŀ���״̬

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
