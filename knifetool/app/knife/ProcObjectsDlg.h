#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CProcObjectsDlg dialog

class CProcObjectsDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcObjectsDlg)

public:
	CProcObjectsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcObjectsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROC_OBJECTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

public:
	void SetPid(DWORD dwPid);

private:
	void InitListCtrl();
	void RefreshList();
	void InitMenu();
	void DestroyMenu();
	void ShowMenu();
	void OnMenuCommand(UINT uID);

private:
	DWORD     m_dwPid;
	CMenu	  m_Menu;	
	CListCtrl m_listctrl;
	CStatic m_stcObjectCnt;
public:
	afx_msg void OnNMRClickListObjects(NMHDR *pNMHDR, LRESULT *pResult);
};
