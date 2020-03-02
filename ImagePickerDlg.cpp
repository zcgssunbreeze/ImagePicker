
// ImagePickerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImagePicker.h"
#include "ImagePickerDlg.h"
#include "afxdialogex.h"

#include "ParamSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CImagePickerDlg �Ի���



CImagePickerDlg::CImagePickerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImagePickerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImagePickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImagePickerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_MAIN_BTN_PARAM_SET, &CImagePickerDlg::OnBnClickedMainBtnParamSet)
	ON_BN_CLICKED(IDC_MAIN_BTN_START, &CImagePickerDlg::OnBnClickedMainBtnStart)
	ON_BN_CLICKED(IDC_MAIN_BTN_STOP, &CImagePickerDlg::OnBnClickedMainBtnStop)
	ON_BN_CLICKED(IDC_MAIN_BTN_NEXT, &CImagePickerDlg::OnBnClickedMainBtnNext)
END_MESSAGE_MAP()


// CImagePickerDlg ��Ϣ�������

BOOL CImagePickerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	MyInitFunc();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CImagePickerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CImagePickerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CImagePickerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//-----------------------------------------------------------------------
/**
\brief��   ���ε��Ի������ return �� escape ��������������
\param��  pMsg  ��Ϣ�ṹ��
\return��  �ɹ�:TRUE   ʧ��:FALSE
**/
//-----------------------------------------------------------------------
BOOL CImagePickerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

//-----------------------------------------------------------------------
/**
\brief��  �Զ���ĳ�ʼ���������������һЩ��ʼ������
\param��
\return��
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::MyInitFunc()
{
	ChangeFont();

	m_bStart = FALSE;
	UpdateBtnStatus();
}

//-----------------------------------------------------------------------
/**
\brief��  ���İ�ť�����壬�Ѱ�ť��������
\param��
\return��
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::ChangeFont()
{
	CDC *pdc = GetDC();

	///////////////////   �޸İ�ť������  ///////////////////////////////////////////////////////
	m_btnFont.CreatePointFont(80, _T("΢���ź�"), pdc);
	m_btnNextFont.CreatePointFont(200, _T("΢���ź�"), pdc);

	GetDlgItem(IDC_MAIN_BTN_PARAM_SET)->SetFont(&m_btnFont);
	GetDlgItem(IDC_MAIN_BTN_START)->SetFont(&m_btnFont);
	GetDlgItem(IDC_MAIN_BTN_STOP)->SetFont(&m_btnFont);
	GetDlgItem(IDC_MAIN_BTN_NEXT)->SetFont(&m_btnNextFont);
	//////////////////////////////////////////////////////////////////////////////////////////////////

	ReleaseDC(pdc);
}

//-----------------------------------------------------------------------
/**
\brief��  ����bStart��ֵ�����°�ť�Ŀ���״̬
\param��  
\return��
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::UpdateBtnStatus()
{
	GetDlgItem(IDC_MAIN_BTN_PARAM_SET)->EnableWindow(!m_bStart);
	GetDlgItem(IDC_MAIN_BTN_START)->EnableWindow(!m_bStart);
	GetDlgItem(IDC_MAIN_BTN_STOP)->EnableWindow(m_bStart);
	GetDlgItem(IDC_MAIN_BTN_NEXT)->EnableWindow(m_bStart);
}

//-----------------------------------------------------------------------
/**
\brief��  ��ͼƬ�ַ���Ҫ�Ĳ�����������
\param��  
\return�� 
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnParamSet()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CParamSetDlg dlg;

	dlg.DoModal();
}

//-----------------------------------------------------------------------
/**
\brief��   �����߳̽���ͼƬ�ַ�
\param��  
\return��  
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnStart()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	/*MoveFile(L"d:\\1064.iso", L"d:\\software\\1064.iso");
	MoveFile(L"d:\\CentOS-7-x86_64-DVD-1611.iso", L"d:\\software\\CentOS-7-x86_64-DVD-1611.iso");
	MoveFile(L"D:\\CentOS-8-x86_64-1905-dvd1.iso", L"D:\\software\\CentOS-8-x86_64-1905-dvd1.iso");

	MessageBox(L"�ļ��ƶ����!");*/

	m_bStart = TRUE;
	UpdateBtnStatus();
}

//-----------------------------------------------------------------------
/**
\brief��  ֹͣ�ַ�ͼƬ���߳�
\param��  
\return�� 
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnStop()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������


	m_bStart = FALSE;
	UpdateBtnStatus();
}

//-----------------------------------------------------------------------
/**
\brief��  ������һ��ͼƬ
\param��
\return��
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnNext()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
