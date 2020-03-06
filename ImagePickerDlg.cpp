
// ImagePickerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImagePicker.h"
#include "ImagePickerDlg.h"
#include "afxdialogex.h"

#include "ParamSetDlg.h"
#include "MtVerify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//// 检查文件是否上传完毕的次数，100次后退出 ////////
#define MAX_CHECK_FILE_SIZE_TIMES 100
//// 检查文件是否上传完毕的间隔，10毫秒 ////////////
#define CHECK_FILE_SIZE_INTERVAL 10
//// 用轮询目录的方式，轮询目录的间隔，100毫秒 //////
#define FIND_DIR_INTERVAL 100

/********* 目录监控，收稿分稿线程 ***********/
UINT MonitorDir(LPVOID pParam);

/********* 寻论目录，收稿分稿线程 ***********/
UINT CheckDir(LPVOID pParam);

/********* 检查文件是否完整并拷贝 ***********/
static void CheckMoveFile(THREADPARAM *lpDirParam, FILE_NOTIFY_INFORMATION* pNotify);

/********* 检查文件是否上传完毕 *************/
BOOL CheckTransComplete(LPCTSTR lpFileName);

/******* 获取GetLastError的信息并显示 *******/
static void ShowErrorInfo(DWORD dwErrorCode);

static char * wchar2char(const wchar_t* wchar);

static wchar_t * char2wchar(const char* cchar);

/********* 用来结束线程的同步量 *************/
/*
  需要结束线程时，调用setevent，在监控目录线程
  中会判断这个同步量的状态，一旦处于激发态，立
  即退出目录监控循环
*********************************************/
HANDLE g_hStopEvent;

/****** 用来将文件计数器清零的同步量 ********/
/*
  图片稿件到达收稿目录后，程序将图片分发到临时
  目录和备份目录，当临时目录到达一定数量，编辑
  认为需要点击下一批图片进行编辑时，激发此同步
  量，同时将文件计数器清零，重新进行累计。
*********************************************/
HANDLE g_hNextEvent;

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

CImagePickerDlg::~CImagePickerDlg()
{
	/// 关闭同步量的句柄
	CloseHandle(g_hStopEvent);

	/// 关闭文件计数器同步量的句柄
	CloseHandle(g_hNextEvent);

	/// 销毁收稿分稿线程的参数指针
	delete m_pstThreadParam;

	/// 销毁线程指针
	if (m_pMonitorThread != NULL)
	{
		delete m_pMonitorThread;
		m_pMonitorThread = NULL;
	}
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
\param：   pMsg  消息结构体
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
	m_pMonitorThread = NULL;
	m_pstThreadParam = NULL;
	g_hStopEvent = NULL;

	m_pstThreadParam = new THREADPARAM();
	m_pstThreadParam->pBtnNextWnd = GetDlgItem(IDC_MAIN_BTN_NEXT);

	g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_hNextEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	ChangeFont();

	m_bStart = FALSE;
	UpdateBtnStatus();

	//MoveFile(L"d:\\imagepicktemp\\*.*", L"d:\\recv\\*.*");
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
	ReadSetParameters();

	ResetEvent(g_hStopEvent);
	//BEGIN_THREAD(m_pMonitorThread, MonitorDir, m_pstThreadParam)
	BEGIN_THREAD(m_pMonitorThread, CheckDir, m_pstThreadParam)

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
	SetEvent(g_hStopEvent);
	
	WAIT_THREAD_END(m_pMonitorThread)

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

	SetEvent(g_hNextEvent);
}

//-----------------------------------------------------------------------
/**
\brief   读取设置参数文件

\return  无
*/
//-----------------------------------------------------------------------
void CImagePickerDlg::ReadSetParameters()
{
	CString strPath = _T(".\\设置参数.ini");

	::GetPrivateProfileString(_T("图片目录参数"), _T("收稿目录"), _T("D:/recv"), m_pstThreadParam->m_szRecvDir, MAX_PATH, strPath);
	::GetPrivateProfileString(_T("图片目录参数"), _T("选稿目录"), _T("D:/pick"), m_pstThreadParam->m_szPickDir, MAX_PATH, strPath);
	::GetPrivateProfileString(_T("图片目录参数"), _T("备份目录"), _T("D:/back"), m_pstThreadParam->m_szBackDir, MAX_PATH, strPath);
	::GetPrivateProfileString(_T("图片目录参数"), _T("临时目录"), _T("D:/imagepicktemp"), m_pstThreadParam->m_szTempDir, MAX_PATH, strPath);
}

//-----------------------------------------------------------------------
/**
\brief   监控目录,收稿分稿线程
\param   线程参数指针
\return  成功结束返回 True
*/
//-----------------------------------------------------------------------
UINT MonitorDir(LPVOID pParam)
{
	THREADPARAM *pInfo = (THREADPARAM *)pParam;
	HANDLE hRecvDir = NULL;

	////////////  监控函数 ReadDirectoryChangesW 需要的结构体 ///////////////////
	char szBuf[2 * (sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH)] = { 0 };
	FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION *)szBuf;
	/////////////////////////////////////////////////////////////////////////////

	DWORD dwFilesCounter = 0;
	DWORD dwBytesRet = 0;
	
	BOOL bRet = TRUE;

	hRecvDir = CreateFile(
		pInfo->m_szRecvDir,                            // pointer to the file name
		GENERIC_READ | GENERIC_WRITE,                  // access (read/write) mode
		FILE_SHARE_READ | FILE_SHARE_WRITE,            // share mode
		NULL,                                          // security descriptor
		OPEN_EXISTING,                                 // how to create
		FILE_FLAG_BACKUP_SEMANTICS,                    // file attributes
		NULL                                           // file with attributes to copy
		);
	
	if (hRecvDir == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(L"打开监控目录出错，请检查收稿目录是否存在或能否正确访问！");

		return EXIT_FAILURE;
	}

	while (TRUE)
	{
		ZeroMemory(pNotify, sizeof(FILE_NOTIFY_INFORMATION));
		bRet = ReadDirectoryChangesW(hRecvDir, pNotify, sizeof(szBuf), FALSE, FILE_NOTIFY_CHANGE_FILE_NAME, &dwBytesRet, NULL, NULL);

		switch (pNotify->Action)
		{
		case FILE_ACTION_ADDED:
			TRACE(" ====== file add ========\n");
			CheckMoveFile(pInfo, pNotify);
			break;
		default:
			break;
		}

		/******* 程序是否退出 *******************************************************************/
		if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0)
		{
			TRACE("@@@ exit thread @@@\n");

			break;
		}
	}


	return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------
/**
\brief   轮询目录,收稿分稿线程
\param   线程参数指针
\return  成功结束返回 True
*/
//-----------------------------------------------------------------------
UINT CheckDir(LPVOID pParam)
{
	THREADPARAM *pInfo = (THREADPARAM *)pParam;

	HANDLE hFind;
	WIN32_FIND_DATA stFindData;
	DWORD dwFilesCounter = 0;

	WCHAR szRecvDir[MAX_PATH] = { 0 };
	WCHAR szRecvFilePathName[MAX_PATH] = { 0 };
	WCHAR szPickFilePathName[MAX_PATH] = { 0 };
	WCHAR szBackFilePathName[MAX_PATH] = { 0 };
	WCHAR szTempFilePathName[MAX_PATH] = { 0 };
	WCHAR szTextOnBtnNext[32] = { 0 };

	wsprintf(szRecvDir, L"%ws\\*.*", pInfo->m_szRecvDir);

	while (TRUE)
	{
		hFind = FindFirstFile(szRecvDir, &stFindData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			continue;
		}

		do
		{
			// 忽略"."和".."两个结果 
			if (wcscmp(stFindData.cFileName, L".") == 0 || wcscmp(stFindData.cFileName, L"..") == 0 || (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}
			else
			{
				wsprintf(szRecvFilePathName, L"%ws\\%s", pInfo->m_szRecvDir, stFindData.cFileName);
				TRACE(TEXT("#### FILE NAME = %s #####\n"), szRecvFilePathName);

				if (CheckTransComplete(szRecvFilePathName))
				{
					wsprintf(szTempFilePathName, L"%ws\\%s", pInfo->m_szTempDir, stFindData.cFileName);
					wsprintf(szBackFilePathName, L"%ws\\%s", pInfo->m_szBackDir, stFindData.cFileName);

					CopyFile(szRecvFilePathName, szTempFilePathName, FALSE);
					CopyFile(szRecvFilePathName, szBackFilePathName, FALSE);
					DeleteFile(szRecvFilePathName);

					dwFilesCounter++;
					wsprintf(szTextOnBtnNext, L"下一批图片（%d）", dwFilesCounter);
					pInfo->pBtnNextWnd->SetWindowText(szTextOnBtnNext);
				}
			}
		} while (FindNextFile(hFind, &stFindData));

		/******* 用户是否点击下一批图片，图片计数器是否清零 *************************************/
		if (WaitForSingleObject(g_hNextEvent, 0) == WAIT_OBJECT_0)
		{
			TRACE("Clear the dwFilesCounter to zero \n");
			dwFilesCounter = 0;
			wsprintf(szTextOnBtnNext, L"下一批图片（%d）", dwFilesCounter);
			pInfo->pBtnNextWnd->SetWindowText(szTextOnBtnNext);
			ResetEvent(g_hNextEvent);
		}

		/******* 程序是否退出 *******************************************************************/
		if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0)
		{
			TRACE("@@@ exit thread @@@\n");
			break;
		}

		Sleep(FIND_DIR_INTERVAL);
	}

	return EXIT_SUCCESS;
}

static void CheckMoveFile(THREADPARAM *lpDirParam, FILE_NOTIFY_INFORMATION* pNotify)
{
	PFILE_NOTIFY_INFORMATION p = pNotify;

	WCHAR szRecvFilePathName[MAX_PATH] = { 0 };
	WCHAR szPickFilePathName[MAX_PATH] = { 0 };
	WCHAR szBackFilePathName[MAX_PATH] = { 0 };
	WCHAR szTempFilePathName[MAX_PATH] = { 0 };

	//while (p->NextEntryOffset != 0)
	//{
		TRACE(" ===== FILE NAME ========\n");
		wsprintf(szRecvFilePathName, L"%ws\\%s", lpDirParam->m_szRecvDir, p->FileName);
		wsprintf(szTempFilePathName, L"%ws\\%s", lpDirParam->m_szTempDir, p->FileName);
		wsprintf(szBackFilePathName, L"%ws\\%s", lpDirParam->m_szBackDir, p->FileName);

		if (CheckTransComplete(szRecvFilePathName))
		{
			CopyFile(szRecvFilePathName, szTempFilePathName, FALSE);
			CopyFile(szRecvFilePathName, szBackFilePathName, FALSE);
			DeleteFile(szRecvFilePathName);
		}
		else
		{
			//printf("接收文件超时，请重新传送 %ws \n", lpRecvFileName);
		}

		//p = p + p->NextEntryOffset;
	//}
}

//// 检查文件是否上传完毕 /////////////////////////
////
//// 参数：待检查的文件名 完整路径+文件名
//// 返回值：上传完毕返回TRUE,超过检查次数返回FALSE;
////
//////////////////////////////////////////////////
BOOL CheckTransComplete(LPCTSTR lpFileName)
{
	int iCheckTimes = 0;

	WIN32_FIND_DATA fd;
	HANDLE hFind;
	__int64 i64FileSize[3] = { 0 };
	//LARGE_INTEGER liFileSize[3] = { 0 };

	while (iCheckTimes <= MAX_CHECK_FILE_SIZE_TIMES)
	{
		hFind = FindFirstFile(lpFileName, &fd);

		i64FileSize[iCheckTimes % 3] = ((__int64)fd.nFileSizeHigh << 32) + fd.nFileSizeLow;
		//liFileSize[iCheckTimes % 3].LowPart = fd.nFileSizeLow;
		//liFileSize[iCheckTimes % 3].HighPart = fd.nFileSizeHigh;
		//printf("filesize %lld \n", i64FileSize[iCheckTimes % 3]);
		TRACE("filesize %d \n", i64FileSize[iCheckTimes % 3]);
		//TRACE("filesize %d \n", liFileSize[iCheckTimes % 3].QuadPart);

		if ((i64FileSize[0]>0) && (i64FileSize[0] == i64FileSize[1]) && (i64FileSize[1] == i64FileSize[2]))
		{
			return TRUE;
		}
		
		/*if (liFileSize[0].QuadPart == liFileSize[1].QuadPart &&  liFileSize[1].QuadPart == liFileSize[2].QuadPart)
		{
			return TRUE;
		}*/

		iCheckTimes++;

		Sleep(CHECK_FILE_SIZE_INTERVAL);
	}

	return FALSE;
}

static void ShowErrorInfo(DWORD dwErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
		);

	// Display the string.
	AfxMessageBox((LPCTSTR)lpMsgBuf);

	// Free the buffer.
	LocalFree(lpMsgBuf);
}

//-----------------------------------------------------------------------
/**
\brief   宽字符转ansi字符
\param   宽字符指针
\return  ansi字符指针
*/
//-----------------------------------------------------------------------
static char * wchar2char(const wchar_t* wchar)
{
	char * m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);

	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';

	return m_char;
}

//-----------------------------------------------------------------------
/**
\brief   ansi字符转宽字符
\param   ansi字符指针
\return  宽字符指针
*/
//-----------------------------------------------------------------------
static wchar_t * char2wchar(const char* cchar)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}