#pragma once
#include "afxcmn.h"


// CSsdtDlg dialog

class CSsdtDlg : public CDialog
{
	DECLARE_DYNAMIC(CSsdtDlg)

public:
	CSsdtDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSsdtDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

// Dialog Data
	enum { IDD = IDD_DIALOG_SSDT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	void  InitListCtrl();
	void  RefreshListCtrl();
private:
	CListCtrl m_listctrl;

};
