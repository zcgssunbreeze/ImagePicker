
// ImagePickerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImagePicker.h"
#include "ImagePickerDlg.h"
#include "afxdialogex.h"

#include "ParamSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CImagePickerDlg 对话框



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


// CImagePickerDlg 消息处理程序

BOOL CImagePickerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	MyInitFunc();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImagePickerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImagePickerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//-----------------------------------------------------------------------
/**
\brief：   屏蔽掉对话框程序 return 和 escape 两个按键的作用
\param：  pMsg  消息结构体
\return：  成功:TRUE   失败:FALSE
**/
//-----------------------------------------------------------------------
BOOL CImagePickerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
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
\brief：  自定义的初始化函数，里面进行一些初始化工作
\param：
\return：
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
\brief：  更改按钮的字体，把按钮的字体搞大
\param：
\return：
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::ChangeFont()
{
	CDC *pdc = GetDC();

	///////////////////   修改按钮的字体  ///////////////////////////////////////////////////////
	m_btnFont.CreatePointFont(80, _T("微软雅黑"), pdc);
	m_btnNextFont.CreatePointFont(200, _T("微软雅黑"), pdc);

	GetDlgItem(IDC_MAIN_BTN_PARAM_SET)->SetFont(&m_btnFont);
	GetDlgItem(IDC_MAIN_BTN_START)->SetFont(&m_btnFont);
	GetDlgItem(IDC_MAIN_BTN_STOP)->SetFont(&m_btnFont);
	GetDlgItem(IDC_MAIN_BTN_NEXT)->SetFont(&m_btnNextFont);
	//////////////////////////////////////////////////////////////////////////////////////////////////

	ReleaseDC(pdc);
}

//-----------------------------------------------------------------------
/**
\brief：  根据bStart的值来更新按钮的可用状态
\param：  
\return：
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
\brief：  对图片分发需要的参数进行设置
\param：  
\return： 
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnParamSet()
{
	// TODO:  在此添加控件通知处理程序代码
	CParamSetDlg dlg;

	dlg.DoModal();
}

//-----------------------------------------------------------------------
/**
\brief：   启动线程进行图片分发
\param：  
\return：  
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnStart()
{
	// TODO:  在此添加控件通知处理程序代码
	/*MoveFile(L"d:\\1064.iso", L"d:\\software\\1064.iso");
	MoveFile(L"d:\\CentOS-7-x86_64-DVD-1611.iso", L"d:\\software\\CentOS-7-x86_64-DVD-1611.iso");
	MoveFile(L"D:\\CentOS-8-x86_64-1905-dvd1.iso", L"D:\\software\\CentOS-8-x86_64-1905-dvd1.iso");

	MessageBox(L"文件移动完毕!");*/

	m_bStart = TRUE;
	UpdateBtnStatus();
}

//-----------------------------------------------------------------------
/**
\brief：  停止分发图片的线程
\param：  
\return： 
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnStop()
{
	// TODO:  在此添加控件通知处理程序代码


	m_bStart = FALSE;
	UpdateBtnStatus();
}

//-----------------------------------------------------------------------
/**
\brief：  导入下一批图片
\param：
\return：
**/
//-----------------------------------------------------------------------
void CImagePickerDlg::OnBnClickedMainBtnNext()
{
	// TODO:  在此添加控件通知处理程序代码
}
