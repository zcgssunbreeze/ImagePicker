
// ImagePicker.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

/****************** ����һ�������̵߳ĺ� *******************/
#define BEGIN_THREAD(Thread_Name, Thread_Func, Thread_Param) \
	if(Thread_Name!=NULL) {			\
	delete Thread_Name;				\
	Thread_Name=NULL;				\
			}												 \
	Thread_Name=AfxBeginThread(Thread_Func, reinterpret_cast<LPVOID>(Thread_Param), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED); \
	Thread_Name->m_bAutoDelete = FALSE; \
	Thread_Name->ResumeThread();
/*******************************************************/

/***************** ����һ���ȴ��߳̽����ĺ� ****************/
#define WAIT_THREAD_END(Thread_Name)    \
	DWORD dwResult;                            \
    while (TRUE){                                   \
		dwResult=::MsgWaitForMultipleObjects(1, &Thread_Name->m_hThread, FALSE, 500, QS_ALLEVENTS);\
		if(dwResult==WAIT_OBJECT_0){	\
			break;										 \
		}													\
		else if(dwResult==WAIT_TIMEOUT){\
			/*AfxMessageBox("�ȴ��߳̽�����ʱ!", MB_OK|MB_ICONERROR);*/\
			break;                                      \
		}                                                   \
		else{											   \
			MSG msg;								\
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){\
				::TranslateMessage(&msg);\
				::DispatchMessage(&msg);\
			    }										\
		     }											\
		}		
/*******************************************************/


// CImagePickerApp: 
// �йش����ʵ�֣������ ImagePicker.cpp
//

class CImagePickerApp : public CWinApp
{
public:
	CImagePickerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CImagePickerApp theApp;