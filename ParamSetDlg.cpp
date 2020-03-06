// ParamSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImagePicker.h"
#include "ParamSetDlg.h"
#include "afxdialogex.h"


// CParamSetDlg �Ի���

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


// CParamSetDlg ��Ϣ�������
BOOL CParamSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	MyInitFunc();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

//-----------------------------------------------------------------------
/**
\brief��   ���ε��Ի������ return �� escape ��������������
\param��   pMsg  ��Ϣ�ṹ��
\return��  �ɹ�:TRUE   ʧ��:FALSE
**/
//-----------------------------------------------------------------------
BOOL CParamSetDlg::PreTranslateMessage(MSG* pMsg)
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
\brief   �Զ����ʼ������������һ���ʼ����������������ļ��Ƿ���ڣ�������
������һ����������Ĭ��ֵ�����ھͶ�ȡ�������ڿؼ�����ʾ
\return  ��
*/
//-----------------------------------------------------------------------
void CParamSetDlg::MyInitFunc()
{
	ReadSetParameters();

	UpdateData(FALSE);
}

void CParamSetDlg::OnBnClickedSetBtnSave()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	WriteSetParameters();

	MessageBox(_T("��������ɹ�!"), _T("�ɹ�"), MB_OK);

	EndDialog(IDOK);
}


void CParamSetDlg::OnBnClickedSetBtnExit()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDCANCEL);
}

//-----------------------------------------------------------------------
/**
\brief   �������ļ��Ƿ���ڣ���������ڣ�������Ҫ����һ��
\input   ���ò����ļ���
\return  ��
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
\brief   ��ȡ���ò���

\return  ��
*/
//-----------------------------------------------------------------------
void CParamSetDlg::ReadSetParameters()
{
	CString strPath = _T(".\\���ò���.ini");

	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("�ո�Ŀ¼"), _T("D:/recv"), m_strImageRecvDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("ѡ��Ŀ¼"), _T("D:/pick"), m_strImagePickDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("����Ŀ¼"), _T("D:/back"), m_strImageBackDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
	::GetPrivateProfileString(_T("ͼƬĿ¼����"), _T("��ʱĿ¼"), _T("D:/imagepicktemp"), m_strImageTempDir.GetBuffer(MAX_PATH), MAX_PATH, strPath);
}

//-----------------------------------------------------------------------
/**
\brief   �������ò���

\return  ��
*/
//-----------------------------------------------------------------------
void CParamSetDlg::WriteSetParameters()
{
	CString strPath = _T(".\\���ò���.ini");

	::WritePrivateProfileString(_T("ͼƬĿ¼����"), _T("�ո�Ŀ¼"), m_strImageRecvDir, strPath);
	::WritePrivateProfileString(_T("ͼƬĿ¼����"), _T("ѡ��Ŀ¼"), m_strImagePickDir, strPath);
	::WritePrivateProfileString(_T("ͼƬĿ¼����"), _T("����Ŀ¼"), m_strImageBackDir, strPath);

	/// ��ȡѡ��Ŀ¼���̷�
	m_strImageTempDir = m_strImagePickDir.Left(3) + _T("imagepicktemp");

	::WritePrivateProfileString(_T("ͼƬĿ¼����"), _T("��ʱĿ¼"), m_strImageTempDir, strPath);
}