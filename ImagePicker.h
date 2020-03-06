
// ImagePicker.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

/****************** 定义一个启动线程的宏 *******************/
#define BEGIN_THREAD(Thread_Name, Thread_Func, Thread_Param) \
	if(Thread_Name!=NULL) {			\
	delete Thread_Name;				\
	Thread_Name=NULL;				\
			}												 \
	Thread_Name=AfxBeginThread(Thread_Func, reinterpret_cast<LPVOID>(Thread_Param), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED); \
	Thread_Name->m_bAutoDelete = FALSE; \
	Thread_Name->ResumeThread();
/*******************************************************/

/***************** 定义一个等待线程结束的宏 ****************/
#define WAIT_THREAD_END(Thread_Name)    \
	DWORD dwResult;                            \
    while (TRUE){                                   \
		dwResult=::MsgWaitForMultipleObjects(1, &Thread_Name->m_hThread, FALSE, 500, QS_ALLEVENTS);\
		if(dwResult==WAIT_OBJECT_0){	\
			break;										 \
		}													\
		else if(dwResult==WAIT_TIMEOUT){\
			/*AfxMessageBox("等待线程结束超时!", MB_OK|MB_ICONERROR);*/\
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
// 有关此类的实现，请参阅 ImagePicker.cpp
//

class CImagePickerApp : public CWinApp
{
public:
	CImagePickerApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CImagePickerApp theApp;