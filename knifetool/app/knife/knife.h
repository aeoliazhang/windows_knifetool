
// knife.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "SubDlgArray.h"

// CknifeApp:
// See knife.cpp for the implementation of this class
//

class CknifeApp : public CWinAppEx
{
public:
	CknifeApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// Implementation

	DECLARE_MESSAGE_MAP()



};

extern CknifeApp theApp;
extern CSubDlgArray  g_SubDlgArray;