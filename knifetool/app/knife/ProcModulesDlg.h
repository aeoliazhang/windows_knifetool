#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CProcModulesDlg dialog

class CProcModulesDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcModulesDlg)

public:
	CProcModulesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcModulesDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMRClickListModules(NMHDR *pNMHDR, LRESULT *pResult);

// Dialog Data
	enum { IDD = IDD_DIALOG_PROC_MODULES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void SetPid(DWORD dwPid);

private:
	void InitListCtrl();
	void RefreshList();
	void InitMenu();
	void DestroyMenu();
	void ShowMenu();
	void OnMenuCommand(UINT uID);
	DWORD GetSelectedItemDllBase();

private:
	CListCtrl m_listctrl;
	CStatic   m_stcModCnt;
	DWORD     m_dwPid;
	CMenu	  m_Menu;	
};
