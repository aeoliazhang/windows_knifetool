#pragma once
#include "afxwin.h"


// CFirstDlg dialog

class CFirstDlg : public CDialog
{
	DECLARE_DYNAMIC(CFirstDlg)

public:
	CFirstDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFirstDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_FIRST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	CStatic m_stcKnifePic;
};
