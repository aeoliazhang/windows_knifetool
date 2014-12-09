#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CSysModuleDlg dialog

class CSysModuleDlg : public CDialog
{
	DECLARE_DYNAMIC(CSysModuleDlg)

public:
	CSysModuleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSysModuleDlg();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

// Dialog Data
	enum { IDD = IDD_DIALOG_SYSMODULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	void EnumSystemModules(SYS_MODULE_INFO_LIST &SysModInfoList);
	void InitDlgCtrls();
	void InitListCtrl();
	void InitMenu();
	void DestroyMenu();
	void ShowMenu();
	void OnMenuCommand(UINT uID);
	void RefreshList();

private:
	CListCtrl m_listctrl;
	CMenu     m_Menu;
	CStatic   m_stcModsCnt;
public:
	afx_msg void OnNMRClickListSysmodule(NMHDR *pNMHDR, LRESULT *pResult);
};
