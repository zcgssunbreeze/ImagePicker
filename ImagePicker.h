
// ImagePicker.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


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