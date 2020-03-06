
// ImagePickerDlg.cpp : ʵ���ļ�
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

//// ����ļ��Ƿ��ϴ���ϵĴ�����100�κ��˳� ////////
#define MAX_CHECK_FILE_SIZE_TIMES 100
//// ����ļ��Ƿ��ϴ���ϵļ����10���� ////////////
#define CHECK_FILE_SIZE_INTERVAL 10
//// ����ѯĿ¼�ķ�ʽ����ѯĿ¼�ļ����100���� //////
#define FIND_DIR_INTERVAL 100

/********* Ŀ¼��أ��ո�ָ��߳� ***********/
UINT MonitorDir(LPVOID pParam);

/********* Ѱ��Ŀ¼���ո�ָ��߳� ***********/
UINT CheckDir(LPVOID pParam);

/********* ����ļ��Ƿ����������� ***********/
static void CheckMoveFile(THREADPARAM *lpDirParam, FILE_NOTIFY_INFORMATION* pNotify);

/********* ����ļ��Ƿ��ϴ���� *************/
BOOL CheckTransComplete(LPCTSTR lpFileName);

/******* ��ȡGetLastError����Ϣ����ʾ *******/
static void ShowErrorInfo(DWORD dwErrorCode);

static char * wchar2char(const wchar_t* wchar);

static wchar_t * char2wchar(const char* cchar);

/********* ���������̵߳�ͬ���� *************/
/*
  ��Ҫ�����߳�ʱ������setevent���ڼ��Ŀ¼�߳�
  �л��ж����ͬ������״̬��һ�����ڼ���̬����
  ���˳�Ŀ¼���ѭ��
*********************************************/
HANDLE g_hStopEvent;

/****** �������ļ������������ͬ���� ********/
/*
  ͼƬ��������ո�Ŀ¼�󣬳���ͼƬ�ַ�����ʱ
  Ŀ¼�ͱ���Ŀ¼������ʱĿ¼����һ���������༭
  ��Ϊ��Ҫ�����һ��ͼƬ���б༭ʱ��������ͬ��
  ����ͬʱ���ļ����������㣬���½����ۼơ�
*********************************************/
HANDLE g_hNextEvent;

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

CImagePickerDlg::~CImagePickerDlg()
{
	/// �ر�ͬ�����ľ��
	CloseHandle(g_hStopEvent);

	/// �ر��ļ�������ͬ�����ľ��
	CloseHandle(g_hNextEvent);

	/// �����ո�ָ��̵߳Ĳ���ָ��
	delete m_pstThreadParam;

	/// �����߳�ָ��
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
\param��   pMsg  ��Ϣ�ṹ��
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
	ReadSetParameters();

	ResetEvent(g_hStopEvent);
	//BEGIN_THREAD(m_pMonitorThread, MonitorDir, m_pstThreadParam)
	BEGIN_THREAD(m_pMonitorThread, CheckDir, m_pstThreadParam)

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
	SetEvent(g_hStopEvent);
	
	WAIT_THREAD_END(m_pMonitorThread)

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

	SetEvent(g_hNextEvent);
}

//-----------------------------------------------------------------------
/**
\brief   ��ȡ���ò����ļ�

\return  ��
*/
//-----------------------------------------------------------------------
void CImagePickerDlg::ReadSetParameters()
{
	CString strPath = _T(".\\���ò���.ini");

	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("�ո�Ŀ¼"), _T("D:/recv"), m_pstThreadParam->m_szRecvDir, MAX_PATH, strPath);
	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("ѡ��Ŀ¼"), _T("D:/pick"), m_pstThreadParam->m_szPickDir, MAX_PATH, strPath);
	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("����Ŀ¼"), _T("D:/back"), m_pstThreadParam->m_szBackDir, MAX_PATH, strPath);
	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("��ʱĿ¼"), _T("D:/imagepicktemp"), m_pstThreadParam->m_szTempDir, MAX_PATH, strPath);
}

//-----------------------------------------------------------------------
/**
\brief   ���Ŀ¼,�ո�ָ��߳�
\param   �̲߳���ָ��
\return  �ɹ��������� True
*/
//-----------------------------------------------------------------------
UINT MonitorDir(LPVOID pParam)
{
	THREADPARAM *pInfo = (THREADPARAM *)pParam;
	HANDLE hRecvDir = NULL;

	////////////  ��غ��� ReadDirectoryChangesW ��Ҫ�Ľṹ�� ///////////////////
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
		AfxMessageBox(L"�򿪼��Ŀ¼���������ո�Ŀ¼�Ƿ���ڻ��ܷ���ȷ���ʣ�");

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

		/******* �����Ƿ��˳� *******************************************************************/
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
\brief   ��ѯĿ¼,�ո�ָ��߳�
\param   �̲߳���ָ��
\return  �ɹ��������� True
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
			// ����"."��".."������� 
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
					wsprintf(szTextOnBtnNext, L"��һ��ͼƬ��%d��", dwFilesCounter);
					pInfo->pBtnNextWnd->SetWindowText(szTextOnBtnNext);
				}
			}
		} while (FindNextFile(hFind, &stFindData));

		/******* �û��Ƿ�����һ��ͼƬ��ͼƬ�������Ƿ����� *************************************/
		if (WaitForSingleObject(g_hNextEvent, 0) == WAIT_OBJECT_0)
		{
			TRACE("Clear the dwFilesCounter to zero \n");
			dwFilesCounter = 0;
			wsprintf(szTextOnBtnNext, L"��һ��ͼƬ��%d��", dwFilesCounter);
			pInfo->pBtnNextWnd->SetWindowText(szTextOnBtnNext);
			ResetEvent(g_hNextEvent);
		}

		/******* �����Ƿ��˳� *******************************************************************/
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
			//printf("�����ļ���ʱ�������´��� %ws \n", lpRecvFileName);
		}

		//p = p + p->NextEntryOffset;
	//}
}

//// ����ļ��Ƿ��ϴ���� /////////////////////////
////
//// �������������ļ��� ����·��+�ļ���
//// ����ֵ���ϴ���Ϸ���TRUE,��������������FALSE;
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
\brief   ���ַ�תansi�ַ�
\param   ���ַ�ָ��
\return  ansi�ַ�ָ��
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
\brief   ansi�ַ�ת���ַ�
\param   ansi�ַ�ָ��
\return  ���ַ�ָ��
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