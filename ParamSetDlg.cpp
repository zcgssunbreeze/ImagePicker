// ParamSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImagePicker.h"
#include "ParamSetDlg.h"
#include "afxdialogex.h"


// CParamSetDlg 对话框

IMPLEMENT_DYNAMIC(CParamSetDlg, CDialogEx)

CParamSetDlg::CParamSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParamSetDlg::IDD, pParent)
	, m_strImageRecvDir(_T(""))
	, m_strImagePickDir(_T(""))
	, m_strImageBackDir(_T(""))
{

}

CParamSetDlg::~CParamSetDlg()
{
}

void CParamSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SET_EDITBROWSE_IMAGE_RECV_DIR, m_strImageRecvDir);
	DDX_Text(pDX, IDC_SET_EDITBROWSE_IMAGE_PICK_DIR, m_strImagePickDir);
	DDX_Text(pDX, IDC_EDITBROWSE_IMAGE_BACK_DIR, m_strImageBackDir);
}


BEGIN_MESSAGE_MAP(CParamSetDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SET_BTN_SAVE, &CParamSetDlg::OnBnClickedSetBtnSave)
	ON_BN_CLICKED(IDC_SET_BTN_EXIT, &CParamSetDlg::OnBnClickedSetBtnExit)
END_MESSAGE_MAP()


// CParamSetDlg 消息处理程序
BOOL CParamSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	MyInitFunc();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//-----------------------------------------------------------------------
/**
\brief：   屏蔽掉对话框程序 return 和 escape 两个按键的作用
\param：   pMsg  消息结构体
\return：  成功:TRUE   失败:FALSE
**/
//-----------------------------------------------------------------------
BOOL CParamSetDlg::PreTranslateMessage(MSG* pMsg)
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
\brief   自定义初始化函数，进行一起初始化工作，如检查参数文件是否存在，不存在
就生成一个，并填入默认值，存在就读取参数并在控件中显示
\return  无
*/
//-----------------------------------------------------------------------
void CParamSetDlg::MyInitFunc()
{
	ReadSetParameters();

	UpdateData(FALSE);
}

void CParamSetDlg::OnBnClickedSetBtnSave()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	WriteSetParameters();

	MessageBox(_T("参数保存成功!"), _T("成功"), MB_OK);

	EndDialog(IDOK);
}


void CParamSetDlg::OnBnClickedSetBtnExit()
{
	// TODO:  在此添加控件通知处理程序代码
	EndDialog(IDCANCEL);
}

//-----------------------------------------------------------------------
/**
\brief   检查参数文件是否存在，如果不存在，可能需要生成一个
\input   设置参数文件名
\return  无
*/
//-----------------------------------------------------------------------
BOOL CParamSetDlg::CheckFileExist(LPCTSTR lpFileName)
{
	CFileFind finder;

	BOOL bRet = finder.FindFile(lpFileName);

	return bRet ? TRUE : FALSE;
}

//-----------------------------------------------------------------------
/**
\brief   读取设置参数

\return  无
*/
//-----------------------------------------------------------------------
void CParamSetDlg::ReadSetParameters()
{
	CString strPath = _T(".\\设置参数.ini");

	::GetPrivateProfileString(_T("图片目录参数"), _T("收稿目录"), _T("D:/recv"), m_strImageRecvDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	::GetPrivateProfileString(_T("图片目录参数"), _T("选稿目录"), _T("D:/pick"), m_strImagePickDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	::GetPrivateProfileString(_T("图片目录参数"), _T("备份目录"), _T("D:/back"), m_strImageBackDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	::GetPrivateProfileString(_T("图片目录参数"), _T("临时目录"), _T("D:/imagepicktemp"), m_strImageTempDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
}

//-----------------------------------------------------------------------
/**
\brief   保持设置参数

\return  无
*/
//-----------------------------------------------------------------------
void CParamSetDlg::WriteSetParameters()
{
	CString strPath = _T(".\\设置参数.ini");

	::WritePrivateProfileString(_T("图片目录参数"), _T("收稿目录"), m_strImageRecvDir, strPath);
	::WritePrivateProfileString(_T("图片目录参数"), _T("选稿目录"), m_strImagePickDir, strPath);
	::WritePrivateProfileString(_T("图片目录参数"), _T("备份目录"), m_strImageBackDir, strPath);

	/// 提取选稿目录的盘符
	m_strImageTempDir = m_strImagePickDir.Left(3) + _T("imagepicktemp");

	::WritePrivateProfileString(_T("图片目录参数"), _T("临时目录"), m_strImageTempDir, strPath);
}