
// ImagePickerDlg.h : ͷ�ļ�
//

#pragma once

//// �˽ṹ����Ϊ�������������ƶ��ļ����̣߳��������ո�Ŀ¼����ѡ��Ŀ¼��������Ŀ¼��������Ϊ��ѡ�ļ���һ��ԭʼ���ݣ�
typedef struct _tagThreadParam
{
	WCHAR m_szRecvDir[MAX_PATH];
	WCHAR m_szPickDir[MAX_PATH];
	WCHAR m_szBackDir[MAX_PATH];
	WCHAR m_szTempDir[MAX_PATH];

	CWnd  *pBtnNextWnd;
}THREADPARAM;


// CImagePickerDlg �Ի���
class CImagePickerDlg : public CDialogEx
{
// ����
public:
	CImagePickerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CImagePickerDlg();                      //��׼��������

// �Ի�������
	enum { IDD = IDD_IMAGEPICKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	BOOL  m_bStart;        /// ���������Ҫ�������ð�ť�Ŀ���״̬
	CFont m_btnFont;       /// ��ť������
	CFont m_btnNextFont;   /// ��һ��ͼƬ��ť������
	CFont m_staticFont;    /// static�ؼ���ʾ����Ŀ¼������

	/// �ո�ָ��̵߳Ķ���
	CWinThread *m_pMonitorThread;
	/// �ո�ָ��̵߳Ĳ���
	THREADPARAM *m_pstThreadParam;

public:
	/// �Զ����ʼ������
	void MyInitFunc();

	/// �������壬�Ѱ�ť��������
	void ChangeFont();

	/// ����bStart��ֵ�����°�ť�Ŀ���״̬
	void UpdateBtnStatus();    

	/// ��ȡ���ò����ļ�
	void ReadSetParameters();

	/// ��static�ؼ�����ʾ���õĹ���Ŀ¼��Ϣ
	void DispWorkDirectory();

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
